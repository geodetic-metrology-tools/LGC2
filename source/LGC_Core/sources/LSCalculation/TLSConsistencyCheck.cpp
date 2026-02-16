// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <algorithm>
#include <bitset>
#include <iomanip>
#include <iterator>
#include <map>
#include <numeric>
#include <random>
#include <sstream>

#include <Eigen/Dense>

#include <Logger.hpp>
#include <TLOR2LOR.h>
#include <TLSConsistencyCheck.h>
#include <TLSInputMatrices.h>

namespace
{
const std::array<std::string, 7> helmertNames = {"TX", "TY", "TZ", "RX", "RY", "RZ", "SCL"};
} // namespace

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
	// list the objects of the group with their degrees of freedom and the directions corresponding to the degrees of freedom; ignore components below threshold
	const double plottingThreshold = 1e-12;
	logWarning() << std::string(100, '=');
	int dimKernGroup = (int)kernel.cols();
	std::set<int> pointsInGroup = getPoints(group);
	logWarning() << "Group " << groupNumber << " with" << group.size() << "object(s) (including" << pointsInGroup.size() << "point(s)) and " << dimKernGroup << " Degree(s) of Freedom : ";

	// the name column is as wide as the longest name in the group
	int nameWidth = 28;
	for (auto obj : group)
		nameWidth = std::max(nameWidth, (int)objectNames[obj].size());
	std::stringstream header;
	header << std::setw(22) << " Type " << "|" << std::setw(nameWidth + 1) << "Object Name" << " | " << std::setw(4) << "DOF" << " | ";
	for (int k = 0; k < dimKernGroup; k++)
		header << std::setw(20) << "Direction d_" + std::to_string(k) << " | ";
	int lineWidth = (int)header.tellp();
	logWarning() << header.str();
	logWarning() << std::string(lineWidth, '-');

	for (auto obj : group)
	{
		Eigen::FullPivLU<Eigen::MatrixXd> lu(kernel(objectIndices[obj], Eigen::indexing::all));
		lu.setThreshold(pivotThreshold);
		for (int k = 0; k < (int)objectIndices[obj].size(); k++)
		{
			std::stringstream msg;
			msg.precision(6);
			msg << std::scientific;
			msg << std::setw(1) << objectIndices[obj].size() << "-dim." << std::setw(15) << objectTypes[obj] << " | " << std::setw(nameWidth) << objectNames[obj] << " | "
				<< std::setw(4) << (k == 0 ? std::to_string(lu.rank()) : std::string()) << " | ";
			for (int j = 0; j < dimKernGroup; j++)
			{
				double d = kernel(objectIndices[obj][k], j);
				msg << std::setw(20) << d * (fabs(d) > plottingThreshold) << " | ";
			}
			logWarning() << msg.str();
		}
		logWarning() << std::string(lineWidth, '-');
	}

	// movements of the affected points in Root and their Helmert part
	std::set<std::string> affectedPoints = getAffectedPoints(group, kernel);
	if (!affectedPoints.empty())
	{
		RootMovementAnalysis analysis = analyseRootMovements(affectedPoints, kernel);
		std::stringstream involved;
		involved << std::setw(nameWidth + 31) << "involved Transformations" << " | ";
		for (const auto &label : analysis.labels)
			involved << std::setw(20) << label << " | ";
		logWarning() << involved.str();
		logWarning() << std::string(lineWidth, '-');

		logWarning() << "In Root, these directions move" << affectedPoints.size() << "point(s) in" << analysis.dimRoot << "dimension(s), of which" << analysis.dimHelmert
					 << "are Helmert movements (rotations and scale about the barycentre of the points).";
		if (dimKernGroup > analysis.dimRoot)
			logWarning() << dimKernGroup - analysis.dimRoot << "direction(s) do not move any point in Root.";
		if (analysis.dimRoot > analysis.dimHelmert)
			logWarning() << analysis.dimRoot - analysis.dimHelmert << "direction(s) are not Helmert movements of the points.";
	}

	// get external connections
	std::pair<std::set<int>, int> extCon = externalConnections(group);
	std::set<int> external = extCon.first;
	int nConnections = extCon.second;
	if (external.size() > 0)
	{
		logWarning() << "This group is connected via" << nConnections << "measurements/constraints to the following" << external.size() << "objects";
		int externalNameWidth = 25;
		for (auto object : external)
			externalNameWidth = std::max(externalNameWidth, (int)objectNames[object].size());
		for (auto object : external)
		{
			logWarning() << std::setw(14) << objectTypes[object] << std::setw(externalNameWidth) << objectNames[object];
		}
	}
	else
	{
		logWarning() << "This group is isolated from the rest of the (adjustable) objects.";
	}
}

TDenseMatrix TLSConsCheck::getCentredHelmertMovements(const std::set<std::string> &points)
{
	// positions in Root relative to the barycentre
	Eigen::Matrix3Xd d(3, points.size());
	int j = 0;
	for (const auto &name : points)
	{
		auto &point = projData.getPoints().getObject(name);
		TPositionVector position = point.getEstimatedValue();
		TLOR2LOR(point.getFrameTreePosition(), projData.getTree().begin(), "sub2Root").transform(position);
		d.col(j++) = position.toRealVector();
	}
	d.colwise() -= d.rowwise().mean();
	const double radius = std::sqrt(d.squaredNorm() / points.size());

	// columns TX, TY, TZ, RX, RY, RZ, SCL; rotations and scale about the barycentre, divided by the rms distance to the barycentre
	TDenseMatrix movements = TDenseMatrix::Zero(3 * points.size(), 7);
	for (j = 0; j < d.cols(); j++)
	{
		movements.block<3, 3>(3 * j, 0).setIdentity();
		if (radius > 0)
		{
			for (int axis = 0; axis < 3; axis++)
				movements.block<3, 1>(3 * j, 3 + axis) = Eigen::Vector3d::Unit(axis).cross(d.col(j)) / radius;
			movements.block<3, 1>(3 * j, 6) = d.col(j) / radius;
		}
	}
	return movements;
}

std::set<std::string> TLSConsCheck::getAffectedPoints(const std::set<int> &group, const TDenseMatrix &kernel)
{
	// points of the group and all points whose Root coordinates move with the kernel
	std::set<std::string> affected;
	for (int n : getPoints(group))
		affected.insert(objectNames[n]);
	for (auto &point : projData.getPoints())
	{
		const std::string &name = point.getName();
		if (affected.count(name) == 0 && getAmbiguousDirectionsInRoot(name, kernel).cwiseAbs().maxCoeff() > 1e-12)
			affected.insert(name);
	}
	return affected;
}

TLSConsCheck::RootMovementAnalysis TLSConsCheck::analyseRootMovements(const std::set<std::string> &points, const TDenseMatrix &kernel)
{
	RootMovementAnalysis analysis;
	const int k = (int)kernel.cols();
	if (k == 0 || points.empty())
		return analysis;

	// movements in Root of the kernel columns (left) and of an orthonormal basis of the kernel (right)
	TDenseMatrix directions(kernel.rows(), 2 * k);
	directions << kernel, Eigen::HouseholderQR<Eigen::MatrixXd>(kernel).householderQ() * Eigen::MatrixXd::Identity(kernel.rows(), k);
	TDenseMatrix movements(3 * points.size(), 2 * k);
	int j = 0;
	for (const auto &name : points)
		movements.middleRows(3 * j++, 3) = getAmbiguousDirectionsInRoot(name, directions);

	TDenseMatrix helmert = getCentredHelmertMovements(points);
	Eigen::JacobiSVD<Eigen::MatrixXd> svdHelmert(helmert, Eigen::ComputeThinU | Eigen::ComputeThinV);
	svdHelmert.setThreshold(pivotThreshold);

	// Helmert components of the movement of each kernel column
	for (int c = 0; c < k; c++)
	{
		Eigen::VectorXd movement = movements.col(c);
		double norm = movement.norm();
		if (norm <= 1e-12)
		{
			// the Root coordinates are determined, the freedoms of subframes and frame parameters cancel out
			analysis.labels.push_back("stationary");
			continue;
		}
		Eigen::VectorXd coefficients = svdHelmert.solve(movement);
		if ((helmert * coefficients - movement).norm() > pivotThreshold * norm)
		{
			analysis.labels.push_back("notInterpretable");
			continue;
		}
		std::string label;
		for (int t = 0; t < 7; t++)
			if (std::fabs(coefficients(t)) > pivotThreshold * norm)
				label += (label.empty() ? "" : " ") + helmertNames[t];
		analysis.labels.push_back(label);
	}

	// orthonormal basis of the movements of the kernel basis; directions below the threshold do not move any point
	Eigen::JacobiSVD<Eigen::MatrixXd> svdMovements(movements.rightCols(k), Eigen::ComputeThinU);
	const Eigen::VectorXd &sigma = svdMovements.singularValues();
	double cutoff = std::max(pivotThreshold * sigma(0), 1e-12);
	analysis.dimRoot = (int)(sigma.array() > cutoff).count();
	analysis.rootBasis = svdMovements.matrixU().leftCols(analysis.dimRoot);
	if (analysis.dimRoot == 0)
		return analysis;

	// the singular values of the part of the movements outside a span are the sines of the principal angles;
	// a zero sine is a movement inside the span
	auto dimInside = [&](const Eigen::MatrixXd &spanBasis) {
		Eigen::MatrixXd outside = analysis.rootBasis - spanBasis * (spanBasis.transpose() * analysis.rootBasis);
		Eigen::JacobiSVD<Eigen::MatrixXd> svdOutside(outside);
		return (int)(svdOutside.singularValues().array() <= pivotThreshold).count();
	};
	analysis.dimHelmert = dimInside(svdHelmert.matrixU().leftCols(svdHelmert.rank()));
	analysis.dimTranslation = dimInside(helmert.leftCols(3) / std::sqrt(double(points.size())));

	// the linearised barycentre, momentum and scale constraints of TLGCPointConstraintGroup are the Helmert movements
	// transposed; divided by the number of points, all rows are averages in metres
	analysis.librResponse = helmert.transpose() * analysis.rootBasis / double(points.size());
	analysis.helmertCoefficients = svdHelmert.solve(analysis.rootBasis);
	return analysis;
}

bool TLSConsCheck::chooseLIBRComponents(const TDenseMatrix &librResponse, const TDenseMatrix &helmertCoefficients, int dimTranslation, std::array<bool, 7> &chosenConstraints)
{
	// the chosen LIBR constraints block the movements if their rows of the response form a regular matrix;
	// pure translations are blocked by barycentre constraints, the other movements by momentum and scale constraints;
	// among these choices the components with the largest Helmert coefficients are taken
	chosenConstraints.fill(false);
	const int dim = (int)librResponse.cols();
	if (dim == 0 || dim > 7)
		return false;
	Eigen::JacobiSVD<Eigen::MatrixXd> svdResponse(librResponse);
	const double cutoff = pivotThreshold * svdResponse.singularValues()(0);

	double bestSigma = 0;
	int bestMask = 0;
	for (int mask = 0; mask < (1 << 7); mask++)
	{
		if (std::bitset<7>(mask).count() != size_t(dim) || std::bitset<3>(mask).count() != size_t(dimTranslation))
			continue;
		std::vector<int> rows;
		for (int t = 0; t < 7; t++)
			if (mask & (1 << t))
				rows.push_back(t);
		Eigen::JacobiSVD<Eigen::MatrixXd> svdChosen(librResponse(rows, Eigen::indexing::all));
		if (svdChosen.singularValues()(dim - 1) <= cutoff)
			continue;
		Eigen::JacobiSVD<Eigen::MatrixXd> svdCoefficients(helmertCoefficients(rows, Eigen::indexing::all));
		double sigmaMin = svdCoefficients.singularValues()(dim - 1);
		if (sigmaMin > bestSigma)
		{
			bestSigma = sigmaMin;
			bestMask = mask;
		}
	}
	for (int t = 0; t < 7; t++)
		chosenConstraints[t] = (bestMask & (1 << t)) != 0;
	return bestSigma > 0;
}

bool TLSConsCheck::computeNecessaryLIBRConstraints(std::list<TLGCPointConstraintGroup> &proposedPointGroupConstraints)
{
	// collect the kernels of all groups that move the same set of points
	std::map<std::set<std::string>, TDenseMatrix> points2Kernel;
	for (const auto &group : connectedNullspaceGroups)
	{
		TDenseMatrix kernel = computeKernelWrtObjectSet(group);
		TDenseMatrix &collected = points2Kernel[getAffectedPoints(group, kernel)];
		if (collected.size() == 0)
		{
			collected = kernel;
		}
		else
		{
			collected.conservativeResize(Eigen::NoChange, collected.cols() + kernel.cols());
			collected.rightCols(kernel.cols()) = kernel;
		}
	}

	int numberConstraintsAdded = 0;
	for (const auto &[pointNames, kernel] : points2Kernel)
	{
		// one point alone cannot disambiguate translation vs rotation
		if (pointNames.size() < 2)
		{
			logWarning() << "Only one point affected, impossible to interprete direction in a unique way as linear combination of translations and rotations.";
			return false;
		}
		RootMovementAnalysis analysis = analyseRootMovements(pointNames, kernel);
		if (analysis.dimRoot < kernel.cols())
		{
			logWarning() << kernel.cols() - analysis.dimRoot << "direction(s) do not move any point in Root and cannot be blocked by LIBR constraints.";
			return false;
		}
		if (analysis.dimHelmert < analysis.dimRoot)
		{
			logWarning() << analysis.dimRoot - analysis.dimHelmert << "direction(s) are not Helmert movements of the points and cannot be blocked by LIBR constraints.";
			return false;
		}

		std::array<bool, 7> chosenConstraints{};
		if (!chooseLIBRComponents(analysis.librResponse, analysis.helmertCoefficients, analysis.dimTranslation, chosenConstraints))
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
	if (numberConstraintsAdded != nullspace.cols())
	{
		logWarning() << "Could not find enough constraints to make problem computable.";
		return false;
	}

	return true;
}

TDenseMatrix TLSConsCheck::getAmbiguousDirectionsInRoot(const std::string &pointName, const TDenseMatrix &nullspaceBlock)
{
	// d p_root = d p_root/d p_sub * d p_sub + sum_i d p_root/d p_i * d p_i
	// where d p_sub and d p_i are the entries of the nullspace block at the point's and chain trafos' unknown indices.

	LGCAdjustablePoint point = projData.getPoints().getObject(pointName);
	int dimBlock = (int)nullspaceBlock.cols();
	TPositionVector positionInSubframe = point.getEstimatedValue();
	TLOR2LOR sub2Root(point.getFrameTreePosition(), projData.getTree().begin(), "sub2Root");
	auto chain = sub2Root.getPartialDerivativesWrtHelmertParameters(positionInSubframe);

	// short-circuit: if no relevant unknown overlaps with a nonzero nullspace entry, the result is zero
	auto hasOverlap = [&](int firstUidx, int numUnkn) { return nullspaceBlock.middleRows(firstUidx, numUnkn).cwiseAbs().maxCoeff() > 1e-12; };
	bool overlap = (point.getNumUnkn() > 0 && hasOverlap(point.getFirstUidx(), point.getNumUnkn()));
	if (!overlap)
		for (const auto &[trafo, _] : chain)
			if (trafo.getNumUnkn() > 0 && hasOverlap(trafo.getFirstUidx(), trafo.getNumUnkn()))
			{
				overlap = true;
				break;
			}
	if (!overlap)
		return TDenseMatrix::Zero(3, dimBlock);

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
		if (colToObject[it.col()] >= 0)
			result.insert(colToObject[it.col()]);
	return result;
}

std::set<std::set<int>> TLSConsCheck::identifyConnectedNullspaceGroups()
{
	// objects contributing to the nullspace: those whose unknown rows have any nonzero entry
	std::set<int> nullspaceObjects;
	for (int row = 0; row < nullspace.rows(); row++)
		if (colToObject[row] >= 0 && !nullspace.row(row).isZero())
			nullspaceObjects.insert(colToObject[row]);

	// union-find with path halving: objects contributing to the nullspace that share a measurement or constraint row are connected
	std::vector<int> parent(objectNames.size());
	std::iota(parent.begin(), parent.end(), 0);
	auto find = [&](int x) {
		while (parent[x] != x)
			x = parent[x] = parent[parent[x]];
		return x;
	};
	for (int row = 0; row < firstDgnMatrix.rows(); row++)
	{
		int first = -1;
		for (int obj : objectsOnRow(row))
		{
			if (nullspaceObjects.count(obj) == 0)
				continue;
			if (first < 0)
				first = obj;
			else
				parent[find(obj)] = find(first);
		}
	}

	std::map<int, std::set<int>> groupMap;
	for (int obj : nullspaceObjects)
		groupMap[find(obj)].insert(obj);

	std::set<std::set<int>> result;
	for (auto &[_, group] : groupMap)
		result.insert(std::move(group));
	return result;
}

void TLSConsCheck::initialize()
{
	for (auto &object : projData.getPoints())
		addObject(object, "Point");
	for (auto &object : projData.getLines())
		addObject(object, "Line");
	for (auto &object : projData.getAngles())
		addObject(object, "Angle");
	for (auto &object : projData.getPlanes())
		addObject(object, "Plane");
	for (auto &object : projData.getLength())
		addObject(object, "Length");
	for (auto &object : projData.getSags())
		addObject(object, "Sag");
	for (auto it = projData.getTree().begin(); it != projData.getTree().end(); ++it)
	{
		TAdjustableHelmertTransformation trafo(it.node->data.get()->frame);
		addObject(trafo, "Transformation");
	}

	// column→object lookup
	colToObject.assign(firstDgnMatrix.cols(), -1);
	for (int obj = 0; obj < (int)objectIndices.size(); obj++)
		for (int col : objectIndices[obj])
			colToObject[col] = obj;

	nullspace = computeNullspace();
}

void TLSConsCheck::addObject(TVAdjustableObject &object, const std::string &objectType)
{
	objectNames.push_back(object.getName());
	objectTypes.push_back(objectType);
	std::vector<int> aux;
	aux.reserve(object.getNumUnkn());
	for (int i = 0; i < object.getNumUnkn(); i++)
		aux.push_back(object.getFirstUidx() + i);
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
	// Exploits the assumption that the nullspace dimension is small (typically 1-7 for a datum defect).
	//
	// The eigenvalues of A^T A are the squared singular values of A.
	// Applying (A^T A + epsilon I)^{-1} multiplies nullspace components by 1/epsilon
	// and the other eigenvectors by 1/(lambda + epsilon), for the associated eigenvalue lambda.
	// A direction is considered null if its singular value is at most pivotThreshold, relative to sigmaMax as for a rank revealing
	// decomposition and absolute as for the residual test A v = 0 of a unit vector v; the shift epsilon is chosen relative to lambdaMax = sigmaMax^2.

	const int nCols = (int)firstDgnMatrix.cols();
	if (nCols == 0)
		return TDenseMatrix::Zero(0, 0);

	Eigen::SparseMatrix<double> A = firstDgnMatrix;
	Eigen::SparseMatrix<double> AtA = A.transpose() * A;

	// To find the largest eigenvalue of A^T A by power iteration
	std::mt19937 rng(42); // fixed seed for reproducibility
	std::normal_distribution<double> dist(0.0, 1.0);
	Eigen::VectorXd x(nCols);
	for (int i = 0; i < nCols; i++)
		x(i) = dist(rng);
	double lambdaMax = 0;
	for (int iter = 0; iter < 30; iter++)
	{
		x.normalize();
		Eigen::VectorXd y = AtA * x;
		lambdaMax = x.dot(y);
		x = y;
	}
	// a zero matrix: every direction is null
	if (lambdaMax <= 0)
		lambdaMax = 1;
	// largest singular value of A
	const double sigmaMax = std::sqrt(lambdaMax);
	const double sigmaCutoff = pivotThreshold * std::min(1.0, sigmaMax);

	// shift: well below the null cutoff pivotThreshold^2 * lambdaMax and above the rounding level of A^T A
	const double epsilon = std::max(1e-2 * pivotThreshold * pivotThreshold, 1e-14) * lambdaMax;
	for (int i = 0; i < nCols; i++)
		AtA.coeffRef(i, i) += epsilon;

	Eigen::MatrixXd Vnull;

	// sparse Cholesky factorization (computed once), in theory the AtA matrix should be positive definite
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
	solver.compute(AtA);
	if (solver.info() != Eigen::Success)
	{
		logWarning() << "Sparse Cholesky factorization failed in nullspace computation, falling back to dense method.";
		// fallback: dense FullPivLU
		Eigen::FullPivLU<Eigen::MatrixXd> lu{Eigen::MatrixXd(A)};
		lu.setThreshold(pivotThreshold);
		Vnull = (lu.dimensionOfKernel() > 0) ? Eigen::MatrixXd(lu.kernel()) : Eigen::MatrixXd(nCols, 0);
	}
	else
	{
		// block of random start vectors; it is doubled as long as all of its vectors turn out to be null
		int blockSize = std::min(15, nCols);
		while (true)
		{
			Eigen::MatrixXd V(nCols, blockSize);
			for (int i = 0; i < nCols; i++)
				for (int j = 0; j < blockSize; j++)
					V(i, j) = dist(rng);

			// inverse iteration: per step, components with lambda >= pivotThreshold^2 * lambdaMax
			// shrink by at least a factor 100 relative to the nullspace components
			for (int iter = 0; iter < 5; iter++)
			{
				// solve (A^T A + epsilon I) V_new = V using the precomputed Cholesky factor
				V = solver.solve(V);

				// re-orthogonalize via thin QR to keep numerical stability
				Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qr(V);
				qr.setThreshold(pivotThreshold);
				V = qr.householderQ() * Eigen::MatrixXd::Identity(nCols, qr.rank());
			}

			// Rayleigh-Ritz: the SVD of A V gives orthonormal directions v in span(V) with singular values ||A v||;
			// those below sigmaCutoff are considered the nullspace
			Eigen::JacobiSVD<Eigen::MatrixXd> svd(A * V, Eigen::ComputeFullV);
			int nNonNull = (int)(svd.singularValues().array() > sigmaCutoff).count();
			int dim = (int)V.cols() - nNonNull;
			Vnull = V * svd.matrixV().rightCols(dim);

			if (dim < blockSize || blockSize == nCols)
				break;
			blockSize = std::min(2 * blockSize, nCols);
		}
	}

	// Sparsify: transform to reduced row echelon form so that each basis vector
	// has a 1 in exactly one pivot row and 0 in all other pivot rows.
	int k = (int)Vnull.cols();
	if (k == 0)
		return TDenseMatrix::Zero(nCols, 0);

	// find k linearly independent rows via column-pivoted QR on the transpose
	Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qrBasis(Vnull.transpose());
	qrBasis.setThreshold(pivotThreshold);
	Eigen::VectorXi pivots = qrBasis.colsPermutation().indices().head(k);
	// transform: V_new = V * V_pivot^{-1} gives identity at pivot rows
	Eigen::MatrixXd Vpivot = Vnull(pivots, Eigen::indexing::all);
	Vnull = Vnull * Vpivot.fullPivLu().solve(Eigen::MatrixXd::Identity(k, k));
	// clean rounding noise, which is of order 1e-12 while genuine entries are of order 1e-8 and above;
	// an object takes part in the nullspace if one of its entries remains
	Vnull = (Vnull.array().abs() < 1e-10).select(0.0, Vnull);

	return Vnull;
}
TDenseMatrix TLSConsCheck::computeKernelWrtObjectSet(const std::set<int> &allowedObjects)
{
	// each column is a nullspace basis vector restricted to the allowed objects (zeros elsewhere)
	std::vector<int> allowedIndices;
	for (int obj : allowedObjects)
		for (int idx : objectIndices[obj])
			allowedIndices.push_back(idx);

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
		std::vector<int> external;
		std::set_difference(contributing.begin(), contributing.end(), group.begin(), group.end(), std::back_inserter(external));
		if (!external.empty() && external.size() < contributing.size())
		{
			nConnections++;
			externalConnectedObjects.insert(external.begin(), external.end());
		}
	}
	return {externalConnectedObjects, nConnections};
}
