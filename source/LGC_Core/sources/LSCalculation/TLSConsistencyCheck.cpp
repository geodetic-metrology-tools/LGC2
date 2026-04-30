// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <functional>
#include <map>
#include <random>
#include <unordered_map>

#include <Eigen/LU>
#include <Eigen/QR>
#include <Eigen/SparseCholesky>

#include <Logger.hpp>
#include <TDirectTransformation.h>
#include <TLOR2LOR.h>
#include <TLSAlgorithm.h>
#include <TLSConsistencyCheck.h>
#include <TSparseMatrix.h>
#include <TVAdjustableObject.h>

// constructor
TLSConsCheck::TLSConsCheck(TLGCData &data, const TLSInputMatrices &inputMtr) : projData(data)
{
	Eigen::SparseMatrix<double, Eigen::RowMajor> A(projData.fUEOIndices.EIndex + projData.fUEOIndices.CIndex, projData.fUEOIndices.UIndex);
	A.topRows(projData.fUEOIndices.EIndex) = inputMtr.getFirstDgnMtrx();
	A.bottomRows(projData.fUEOIndices.CIndex) = inputMtr.getCnstrFirstDgnMtrx();

	firstDgnMatrix = A;

	initialize();

	// identify connected groups of kernel based on the nullspace
	connectedNullspaceGroups = identifyConnectedNullspaceGroups();
	if (connectedNullspaceGroups.size() == 0)
	{
		resultStatus = true;
	}
}

void TLSConsCheck::generateErrorMessage()
{
	if (connectedNullspaceGroups.size() > 0)
	{
		logWarning() << "There are " << connectedNullspaceGroups.size() << " connected Groups of unidentifiable objects.";
	}
	else
	{
		logWarning() << "No identifiability-problems detected.";
	}

	int nGroup = 0;
	for (const auto &group : connectedNullspaceGroups)
	{
		generateGroupWarning(group, computeKernelWrtObjectSet(group), nGroup);
		nGroup++;
	}
}
void TLSConsCheck::generateGroupWarning(const std::set<int> &group, const TDenseMatrix &kernel, const int groupNumber)
{
	// list objects in the group and the directions corresponding to the degrees of freedom; ignore components below threshold
	const double plottingThreshold = 1e-12;
	logWarning() << std::string(100, '=');
	int dimKernGroup = kernel.cols();
	std::set<int> pointsInGroup = getPoints(group);
	logWarning() << "Group " << groupNumber << " with" << group.size() << "object(s) (including" << pointsInGroup.size() << "point(s)) and " << dimKernGroup << " Degree(s) of Freedom : ";
	std::stringstream directionHeader;
	directionHeader << std::setw(22) << " Type " << "|" << std::setw(29) << "Object Name" << " | ";
	for (int k = 0; k < dimKernGroup; k++)
		directionHeader << std::setw(17) << "Direction " << "d_" << k << " | ";
	int lineWidth = directionHeader.tellp();
	logWarning() << directionHeader.str();
	logWarning() << std::string(lineWidth, '-');

	for (auto obj : group)
	{
		for (int k = 0; k < (int)objectIndices[obj].size(); k++)
		{
			std::stringstream msg;
			msg.precision(6);
			msg << std::scientific;
			msg << std::setw(1) << objectIndices[obj].size() << "-dim." << std::setw(15) << objectTypes[obj] << " | " << std::setw(28) << objectNames[obj] << " | ";
			for (int j = 0; j < dimKernGroup; j++)
			{
				double d = kernel(objectIndices[obj][k], j);
				msg << std::setw(20) << d * (fabs(d) > plottingThreshold) << " | ";
			}
			logWarning() << msg.str();
		}
		logWarning() << std::string(lineWidth, '-');
	}

	// only meaningful with ≥ 2 points; can the movement be explained as a Helmert transformation?
	if (pointsInGroup.size() > 1)
	{
		logWarning() << "In case these directions can be explained as linear combinations of Helmert transformations acting on these points (in Root coordinates), they "
						"are listed below";
		reportGroupHelmertInterpretation(pointsInGroup, kernel);
	}

	// get external connections
	std::pair<std::set<int>, int> extCon = externalConnections(group);
	std::set<int> external = extCon.first;
	int nConnections = extCon.second;
	if (external.size() > 0)
	{
		logWarning() << "This group is connected via" << nConnections << "measurements/constraints to the following" << external.size() << "objects";
		for (auto object : external)
		{
			logWarning() << std::setw(12) << objectTypes[object] << std::setw(25) << objectNames[object];
		}
	}
	else
	{
		logWarning() << "This group is isolated from the rest of the (adjustable) objects.";
	}
}

TDenseMatrix TLSConsCheck::getMasterDirections(const std::string &pointName)
{
	// compute the jacobian of the virtual helmert transformation acting on the point transformed to root
	// create the "Master" Helmert transformation object acting on the Root frame
	// equal to the identity transformation
	// for our purpose we use the default constructor of TDirectTransformation, which gives an identity transformation
	TDirectTransformation masterTrafo;
	LGCAdjustablePoint point = projData.getPoints().getObject(pointName);
	// transform point to root coordinates
	TLOR2LOR sub2Root(point.getFrameTreePosition(), projData.getTree().begin(), "sub2Root");
	TPositionVector positionInRoot = point.getEstimatedValue();
	sub2Root.transform(positionInRoot);
	TDenseMatrix masterJac = masterTrafo.getPartialDerivativeWrtHelmertParameters(positionInRoot);
	return masterJac;
}

TDenseMatrix TLSConsCheck::getAmbiguousDirectionsInRoot(const std::string &pointName, const TDenseMatrix &nullspaceBlock)
{
	// d p_root = d p_root/d p_sub * d p_sub + sum_i d p_root/d p_i * d p_i
	// where d p_sub and d p_i are the entries of the nullspace block at the point's and chain trafos' unknown indices.

	LGCAdjustablePoint point = projData.getPoints().getObject(pointName);
	int dimBlock = nullspaceBlock.cols();
	TPositionVector positionInSubframe = point.getEstimatedValue();
	TLOR2LOR sub2Root(point.getFrameTreePosition(), projData.getTree().begin(), "sub2Root");
	auto chain = sub2Root.getPartialDerivativesWrtHelmertParameters(positionInSubframe);

	// short-circuit: if no relevant unknown overlaps with a nonzero nullspace entry, the result is zero
	auto hasOverlap = [&](int firstUidx, int numUnkn) {
		return nullspaceBlock.middleRows(firstUidx, numUnkn).cwiseAbs().maxCoeff() > 1e-12;
	};
	bool overlap = (point.getNumUnkn() > 0 && hasOverlap(point.getFirstUidx(), point.getNumUnkn()));
	if (!overlap)
		for (const auto &[trafo, _] : chain)
			if (trafo.getNumUnkn() > 0 && hasOverlap(trafo.getFirstUidx(), trafo.getNumUnkn()))
			{
				overlap = true;
				break;
			}
	if (!overlap) return TDenseMatrix::Zero(3, dimBlock);

	TDenseMatrix ptJacobian = TDenseMatrix::Zero(3, dimBlock);
	if (point.getNumUnkn() > 0)
	{
		TDenseMatrix ptInSubframe = TDenseMatrix::Zero(3, dimBlock);
		ptInSubframe(point.getRelativeUnknIndices(), Eigen::indexing::all) = nullspaceBlock.middleRows(point.getFirstUidx(), point.getNumUnkn());
		ptJacobian = sub2Root.getPartialDerivativeWrtPosition() * ptInSubframe;
	}
	for (const auto &[trafo, partDeriv] : chain)
	{
		if (trafo.getNumUnkn() > 0)
			ptJacobian += partDeriv(Eigen::indexing::all, trafo.getRelativeUnknIndices()) * nullspaceBlock.middleRows(trafo.getFirstUidx(), trafo.getNumUnkn());
	}
	return ptJacobian;
}

std::set<int> TLSConsCheck::objectsOnRow(int row) const
{
	std::set<int> result;
	for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator it(firstDgnMatrix, row); it; ++it)
		result.insert(colToObject[it.col()]);
	return result;
}

void TLSConsCheck::reportGroupHelmertInterpretation(const std::set<int> &pointsInGroup, const TDenseMatrix &kernel)
{
	static const std::array<std::string, 7> trafoNames = {"TX", "TY", "TZ", "RX", "RY", "RZ", "SCL"};

	int dim = kernel.cols();
	int nPoints = pointsInGroup.size();

	TDenseMatrix helmertMovements(3 * nPoints, 7);
	TDenseMatrix ambiguousDirs(3 * nPoints, dim);
	int j = 0;
	for (int k : pointsInGroup)
	{
		helmertMovements.middleRows(3 * j, 3) = getMasterDirections(objectNames.at(k));
		ambiguousDirs.middleRows(3 * j, 3) = getAmbiguousDirectionsInRoot(objectNames.at(k), kernel);
		j++;
	}
	// factor once and reuse across all directions
	auto qr = helmertMovements.fullPivHouseholderQr();

	std::stringstream row;
	row << std::setw(54) << "involved Transformations |";
	for (int c = 0; c < dim; c++)
	{
		Eigen::VectorXd dir = ambiguousDirs.col(c);
		Eigen::VectorXd lin = qr.solve(dir);
		double resN = (helmertMovements * lin - dir).norm();
		double dirN = dir.norm();
		double rel = (dirN > pivotThreshold) ? (resN / dirN) : resN;

		std::string label;
		if (dir.isZero())
		{
			// root coordinates are determinable but subframe / frame freedoms cancel out
			label = "stationary";
		}
		else if (rel >= pivotThreshold)
		{
			label = "notInterpretable";
		}
		else
		{
			for (int t = 0; t < 7; t++)
			{
				if (std::fabs(lin(t)) > pivotThreshold)
				{
					label += trafoNames[t];
					label += ' ';
				}
			}
		}
		row << std::setw(22) << label << "|";
	}
	logWarning() << row.str();
}

std::pair<std::set<std::string>, Eigen::VectorXd> TLSConsCheck::getAffectedPointsAndRootMovements(const std::set<int> &group, const Eigen::VectorXd &nullspaceVector)
{
	// group points are always included even if their root movement is zero (stationary case)
	std::set<std::string> groupPointNames;
	for (int n : getPoints(group))
	{
		if (objectTypes[n] != "Point") throw std::runtime_error("Object " + objectNames[n] + " is not of type POINT.");
		groupPointNames.insert(objectNames[n]);
	}

	std::set<std::string> affectedPoints;
	std::unordered_map<std::string, Eigen::Vector3d> points2Movements;

	for (auto &point : projData.getPoints())
	{
		std::string name = point.getName();
		Eigen::Vector3d dir = getAmbiguousDirectionsInRoot(name, nullspaceVector);
		if (groupPointNames.count(name) == 0 && dir.norm() <= 1e-12) continue;
		affectedPoints.insert(name);
		points2Movements[name] = dir;
	}

	Eigen::VectorXd movements = Eigen::VectorXd::Zero(3 * affectedPoints.size());
	int j = 0;
	for (const auto &name : affectedPoints)
	{
		movements.segment(3 * j, 3) = points2Movements[name];
		j++;
	}
	return {affectedPoints, movements};
}

bool TLSConsCheck::findDirectionsToBlock(std::array<bool, 7> &chosenConstraints, const Eigen::MatrixXd &helmertMovements, const Eigen::MatrixXd &nullspaceDirections)
{
	if (helmertMovements.rows() != nullspaceDirections.rows())
		throw std::runtime_error("Helmert directions and nullspace directions must span subspaces of the same vector space.");
	if (helmertMovements.rows() % 3 != 0)
		throw std::runtime_error("Row count must be a multiple of 3.");

	// one point alone cannot disambiguate translation vs rotation
	if (helmertMovements.rows() / 3 == 1)
	{
		logWarning() << "Only one point affected, impossible to interprete direction in a unique way as linear combination of translations and rotations.";
		return false;
	}
	int dimNullspace = nullspaceDirections.cols();
	if (dimNullspace > 7)
	{
		logWarning() << "Dimension of the group nullspace is greater then 7 and it is impossible to explain all these directions as linear combination of "
						"translations, rotations and scale.";
		return false;
	}

	// 1. project nullspace into Helmert basis and verify each direction is actually representable
	Eigen::MatrixXd nullspaceAsHelmert = helmertMovements.fullPivHouseholderQr().solve(nullspaceDirections);
	Eigen::MatrixXd residual = helmertMovements * nullspaceAsHelmert - nullspaceDirections;
	for (int j = 0; j < dimNullspace; j++)
	{
		double dirNorm = nullspaceDirections.col(j).norm();
		double rel = (dirNorm > pivotThreshold) ? residual.col(j).norm() / dirNorm : residual.col(j).norm();
		if (rel > pivotThreshold)
		{
			logWarning() << "not all ambiguous directions can be represented as helmert movements.";
			return false;
		}
	}
	if (nullspaceAsHelmert.fullPivHouseholderQr().rank() < dimNullspace)
	{
		logWarning() << "not all ambiguous directions can be represented as helmert movements.";
		return false;
	}

	// 2. clean noise; pure translations are X·ker(rotation/scale rows of X)
	Eigen::MatrixXd X = (nullspaceAsHelmert.array().abs() < pivotThreshold).select(0.0, nullspaceAsHelmert);
	Eigen::FullPivLU<Eigen::MatrixXd> luBottom(X.bottomRows(4));
	luBottom.setThreshold(pivotThreshold);
	Eigen::MatrixXd ker = luBottom.kernel();
	Eigen::MatrixXd pureTrans = (ker.cols() > 0) ? Eigen::MatrixXd(X * ker) : Eigen::MatrixXd::Zero(7, 0);

	// 3. for the rotation/scale part, run the greedy on X with translation rows zeroed
	// (equivalent to old "complement of pureTrans then zero translation rows" because pureTrans has zero rotation rows by construction)
	Eigen::MatrixXd rest = X;
	rest.topRows(3).setZero();

	// 4. greedy block selection on each part, OR-combined
	std::array<bool, 7> tBlocks = whatToBlock(pureTrans);
	std::array<bool, 7> rBlocks = whatToBlock(rest);
	for (int j = 0; j < 7; j++) chosenConstraints[j] = tBlocks[j] || rBlocks[j];

	if (std::count(chosenConstraints.begin(), chosenConstraints.end(), true) != dimNullspace)
	{
		logWarning() << "Could not find enough constraints for problematic group.";
		return false;
	}
	return true;
}

std::set<std::set<int>> TLSConsCheck::identifyConnectedNullspaceGroups()
{
	std::unordered_map<int, int> parent;
	for (int obj : nullspaceObjects)
		parent[obj] = obj;

	// find with path compression
	std::function<int(int)> find = [&](int x) { return parent[x] == x ? x : parent[x] = find(parent[x]); };

	// union by nullspace neighbors
	for (int obj : nullspaceObjects)
		for (int neighbor : nullspaceNeighbors[obj])
			if (nullspaceObjects.count(neighbor) > 0)
				parent[find(obj)] = find(neighbor);

	// collect groups
	std::map<int, std::set<int>> groupMap;
	for (int obj : nullspaceObjects)
		groupMap[find(obj)].insert(obj);

	std::set<std::set<int>> result;
	for (auto &[_, group] : groupMap)
		result.insert(std::move(group));
	return result;
}

bool TLSConsCheck::computeNecessaryLIBRConstraints(std::list<TLGCPointConstraintGroup> &proposedPointGroupConstraints)
{
	// go over each group  and transform each ambiguous direction int point movements in root.
	std::vector<std::pair<std::set<std::string>, Eigen::VectorXd>> groupsOfAffectedPoints;
	for (const auto &group : connectedNullspaceGroups)
	{
		TDenseMatrix kernel = computeKernelWrtObjectSet(group);
		for (int c = 0; c < kernel.cols(); c++)
			groupsOfAffectedPoints.push_back(getAffectedPointsAndRootMovements(group, kernel.col(c)));
	}

	// collect all directions affecting the same sets of points (std::set has no default hash, so std::map)
	std::map<std::set<std::string>, Eigen::MatrixXd> groups2AmbiguousDirections;
	std::map<std::set<std::string>, Eigen::MatrixXd> groups2HelmertMovements;

	for (const auto &[pointNames, rootDirections] : groupsOfAffectedPoints)
	{
		int nRows = 3 * pointNames.size();
		Eigen::MatrixXd &newDir = groups2AmbiguousDirections[pointNames];
		newDir.conservativeResize(nRows, newDir.cols() + 1);
		newDir.col(newDir.cols() - 1) = rootDirections;

		Eigen::MatrixXd helmertMovements = Eigen::MatrixXd::Zero(nRows, 7);
		int i = 0;
		for (const auto &point : pointNames)
		{
			helmertMovements.middleRows(3 * i, 3) = getMasterDirections(point);
			i++;
		}
		groups2HelmertMovements[pointNames] = helmertMovements;
	}

	int numberConstraintsAdded = 0;
	for (const auto &[pointNames, helmertMovements] : groups2HelmertMovements)
	{
		const Eigen::MatrixXd &nullspaceDirections = groups2AmbiguousDirections[pointNames];

		std::array<bool, 7> chosenConstraints{};
		bool constraintsFound = findDirectionsToBlock(chosenConstraints, helmertMovements, nullspaceDirections);
		if (!constraintsFound)
		{
			logWarning() << "Could not find blocking constraints for a problematic group.";
			return false;
		}

		// use the found constraints and define a point constraint group
		TLGCPointConstraintGroup newConstraintGroup(projData, chosenConstraints, pointNames);

		proposedPointGroupConstraints.push_back(newConstraintGroup);
		numberConstraintsAdded += newConstraintGroup.getConstraintDimension();
	}

	// final check if enough constraints in total were found
	bool result = false;
	if (numberConstraintsAdded != nullspace.cols())
	{
		logWarning() << "Could not find enough constraints to make problem computable.";
		return false;
	}

	return true;
}

std::array<bool, 7> TLSConsCheck::whatToBlock(const Eigen::MatrixXd &mat)
{
	std::array<bool, 7> result;
	result.fill(false);
	Eigen::MatrixXd remainingDirections = mat;
	int rank = remainingDirections.fullPivHouseholderQr().rank();
	int addedBlocks = 0;

	Eigen::MatrixXd aux = remainingDirections;
	for (int j = 0; j < rank; j++)
	{
		if (aux.cols() == 0)
			break;
		// find biggest row
		int biggestRowIdx;
		aux.rowwise().norm().maxCoeff(&biggestRowIdx);
		result[biggestRowIdx] = true;
		// find linear combinations of aux columns that have zero in the selected row
		Eigen::FullPivLU<Eigen::MatrixXd> lu(aux.row(biggestRowIdx));
		lu.setThreshold(pivotThreshold);
		Eigen::MatrixXd nullCoeffs = lu.kernel();
		if (nullCoeffs.cols() == 0)
		{
			aux = Eigen::MatrixXd::Zero(7, 0);
		}
		else
		{
			aux = aux * nullCoeffs;
		}
		// the kernel basis is not orthonormal, so multiplying it back can leak tiny nonzeros
		// into already-selected rows; force them to stay strictly zero
		for (int k = 0; k < 7; k++)
		{
			if (result[k])
			{
				aux.row(k).setZero();
			}
		}
	}
	return result;
}

void TLSConsCheck::initialize()
{
	for (auto &object : projData.getPoints()) addObject(object, "Point");
	for (auto &object : projData.getLines()) addObject(object, "Line");
	for (auto &object : projData.getAngles()) addObject(object, "Angle");
	for (auto &object : projData.getPlanes()) addObject(object, "Plane");
	for (auto &object : projData.getLength()) addObject(object, "Length");
	for (auto it = projData.getTree().begin(); it != projData.getTree().end(); ++it)
	{
		TAdjustableHelmertTransformation trafo(it.node->data.get()->frame);
		addObject(trafo, "Transformation");
	}

	// column→object lookup
	colToObject.assign(firstDgnMatrix.cols(), -1);
	for (int obj = 0; obj < (int)objectIndices.size(); obj++)
		for (int col : objectIndices[obj]) colToObject[col] = obj;

	nullspace = computeNullspace();

	// objects contributing to the nullspace: those whose unknown rows have any nonzero entry
	nullspaceObjects.clear();
	for (int row = 0; row < nullspace.rows(); row++)
		if (!nullspace.row(row).isZero()) nullspaceObjects.insert(colToObject[row]);

	// link any pair of kernel-contributing objects sharing a measurement/constraint row.
	// the intermediate list collects the row's nullspace-relevant objects once so we can
	// emit the all-pairs edges in the inner double loop without re-filtering.
	nullspaceNeighbors.assign(objectNames.size(), {});
	for (int row = 0; row < firstDgnMatrix.rows(); row++)
	{
		std::vector<int> nullspaceObjectsOnRow;
		for (int obj : objectsOnRow(row))
			if (nullspaceObjects.count(obj) > 0) nullspaceObjectsOnRow.push_back(obj);
		for (int a : nullspaceObjectsOnRow)
			for (int b : nullspaceObjectsOnRow) nullspaceNeighbors[a].insert(b);
	}
}

void TLSConsCheck::addObject(TVAdjustableObject &object, const std::string &objectType)
{
	objectNames.push_back(object.getName());
	objectTypes.push_back(objectType);
	std::vector<int> aux;
	aux.reserve(object.getNumUnkn());
	for (int i = 0; i < object.getNumUnkn(); i++) aux.push_back(object.getFirstUidx() + i);
	objectIndices.push_back(std::move(aux));
}

std::set<int> TLSConsCheck::getPoints(const std::set<int> &group)
{
	std::set<int> points;
	for (int j : group)
	{
		if (objectTypes[j] == "Point")
		{
			points.insert(j);
		}
	}
	return points;
}

TDenseMatrix TLSConsCheck::computeNullspace()
{
	// Shifted inverse iteration for sparse nullspace computation.
	// Exploits the assumption that the nullspace dimension is small (typically 1-7).
	//
	// The eigenvalues of A^T A are the squared singular values of A.
	// Nullspace vectors have eigenvalue 0, all others have eigenvalue >= lambda_min > 0.
	// Applying (A^T A + eps I)^{-1} amplifies nullspace components by 1/eps
	// and non-nullspace components by at most 1/lambda_min.
	// After a few iterations the nullspace dominates completely.

	int nCols = firstDgnMatrix.cols();

	// form A^T A + eps I (sparse, symmetric positive definite)
	double epsilon = 1e-10;
	Eigen::SparseMatrix<double> AtA = firstDgnMatrix.transpose() * firstDgnMatrix;
	for (int i = 0; i < nCols; i++)
		AtA.coeffRef(i, i) += epsilon;

	// sparse Cholesky factorization (computed once)
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
	solver.compute(AtA);
	if (solver.info() != Eigen::Success)
	{
		logWarning() << "Sparse Cholesky factorization failed in nullspace computation, falling back to dense method.";
		// fallback: dense FullPivLU
		TDenseMatrix firstDense = firstDgnMatrix;
		Eigen::FullPivLU<TDenseMatrix> lu(firstDense);
		lu.setThreshold(pivotThreshold);
		return lu.kernel();
	}

	// start with a random block of vectors, dimension n x blockSize
	// blockSize = expected max nullspace dimension + oversampling
	const int initialBlockSize = 15;
	int blockSize = initialBlockSize;
	std::mt19937 rng(42); // fixed seed for reproducibility
	std::normal_distribution<double> dist(0.0, 1.0);
	Eigen::MatrixXd V(nCols, blockSize);
	for (int i = 0; i < nCols; i++)
		for (int j = 0; j < blockSize; j++)
			V(i, j) = dist(rng);

	// inverse iteration: 5 steps are more than sufficient
	// (convergence ratio per step ~ lambda_min / epsilon >> 1)
	for (int iter = 0; iter < 5; iter++)
	{
		// solve (A^T A + eps I) V_new = V using the precomputed Cholesky factor
		V = solver.solve(V);

		// re-orthogonalize via thin QR to keep numerical stability
		Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qr(V);
		qr.setThreshold(pivotThreshold);
		int rank = qr.rank();
		Eigen::MatrixXd Q = qr.householderQ() * Eigen::MatrixXd::Identity(nCols, rank);
		V = Q;
		blockSize = rank;
	}

	// verify each candidate: keep only vectors truly in the nullspace
	std::vector<int> confirmedColumns;
	for (int j = 0; j < V.cols(); j++)
	{
		double residual = (firstDgnMatrix * V.col(j)).norm();
		double vecNorm = V.col(j).norm();
		double relativeResidual = (vecNorm > pivotThreshold) ? (residual / vecNorm) : residual;
		if (relativeResidual < pivotThreshold)
			confirmedColumns.push_back(j);
	}

	int nullspaceDim = confirmedColumns.size();
	if (nullspaceDim == initialBlockSize)
		logWarning() << "Detected nullspace dimension (" << nullspaceDim << ") equals the initial block size. The true nullspace may be larger.";

	TDenseMatrix confirmedNullspace(nCols, nullspaceDim);
	for (int i = 0; i < nullspaceDim; i++)
		confirmedNullspace.col(i) = V.col(confirmedColumns[i]);

	// Sparsify: transform to reduced row echelon form so that each basis vector
	// has a 1 in exactly one pivot row and 0 in all other pivot rows.
	int k = confirmedNullspace.cols();
	if (k > 0)
	{
		// find k linearly independent rows via column-pivoted QR on the transpose
		Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qrBasis(confirmedNullspace.transpose());
		qrBasis.setThreshold(pivotThreshold);
		Eigen::VectorXi pivots = qrBasis.colsPermutation().indices().head(k);
		// extract the k x k submatrix at pivot rows
		Eigen::MatrixXd Vpivot(k, k);
		for (int i = 0; i < k; i++)
			Vpivot.row(i) = confirmedNullspace.row(pivots(i));
		// transform: V_new = V * V_pivot^{-1} gives identity at pivot rows
		confirmedNullspace = confirmedNullspace * Vpivot.fullPivLu().solve(Eigen::MatrixXd::Identity(k, k));
		// clean near-zero entries
		confirmedNullspace = (confirmedNullspace.array().abs() < pivotThreshold).select(0.0, confirmedNullspace);
	}

	return confirmedNullspace;
}
TDenseMatrix TLSConsCheck::computeKernelWrtObjectSet(const std::set<int> &allowedObjects)
{
	// each column is a nullspace basis vector restricted to the allowed objects (zeros elsewhere)
	std::vector<int> allowedIndices;
	for (int obj : allowedObjects)
		for (int idx : objectIndices[obj]) allowedIndices.push_back(idx);

	TDenseMatrix projection = nullspace(allowedIndices, Eigen::indexing::all);
	TDenseMatrix projectedBasis = Eigen::FullPivLU<Eigen::MatrixXd>(projection).image(projection);
	TDenseMatrix result = TDenseMatrix::Zero(nullspace.rows(), projectedBasis.cols());
	result(allowedIndices, Eigen::indexing::all) = projectedBasis;
	return result;
}

std::pair<std::set<int>, int> TLSConsCheck::externalConnections(const std::set<int> &group)
{
	std::set<int> externalConnectedObjects;
	int nConnections = 0;

	for (int row = 0; row < firstDgnMatrix.rows(); row++)
	{
		std::set<int> contributing = objectsOnRow(row);
		bool hasInternal = false, hasExternal = false;
		for (int obj : contributing)
		{
			if (group.count(obj) > 0)
				hasInternal = true;
			else
				hasExternal = true;
		}

		if (hasInternal && hasExternal)
		{
			nConnections++;
			for (int obj : contributing)
				if (group.count(obj) == 0) externalConnectedObjects.insert(obj);
		}
	}
	return {externalConnectedObjects, nConnections};
}

