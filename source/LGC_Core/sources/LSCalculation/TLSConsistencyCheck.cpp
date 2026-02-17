// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <Eigen/LU>
#include <Eigen/QR>
#include <Eigen/SVD>
#include <Eigen/SparseCholesky>

#include <random>

#include <functional>
#include <map>
#include <unordered_map>

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
	for (auto group : connectedNullspaceGroups)
	{
		// compute nullspace associated to this group
		std::vector<TDenseMatrix> kernGroupBaseVectors = computeKernelWrtObjectSet(group);
		// generate warning log for this group
		generateGroupWarning(group, kernGroupBaseVectors, nGroup);
		nGroup++;
	}
}
void TLSConsCheck::generateGroupWarning(const std::set<int> &group, const std::vector<TDenseMatrix> &kernGroupBaseVectors, const int groupNumber)
{
	// Generate warning message listing the objects and the directions
	// corresponding to the degrees of freedom
	// To improve readability of the output ignore components below threshold.

	// plotting the ambiguous directions
	double plottingThreshold = 1e-12;
	logWarning() << std::string(100, '=');
	int dimKernGroup = kernGroupBaseVectors.size();
	std::set<int> pointsInGroup = getPoints(group);
	logWarning() << "Group " << groupNumber << " with" << group.size() << "object(s) (including" << pointsInGroup.size() << "point(s)) and " << dimKernGroup << " Degree(s) of Freedom : ";
	std::stringstream directionHeader;
	directionHeader << std::setw(22) << " Type "
					<< "|" << std::setw(29) << "Object Name"
					<< " | ";
	for (int k = 0; k < dimKernGroup; k++)
	{
		directionHeader << std::setw(17) << "Direction "
						<< "d_" << k << " | ";
	}
	int lineWidth = directionHeader.tellp();
	logWarning() << directionHeader.str();
	logWarning() << std::string(lineWidth, '-');
	// loop over objects in group
	for (auto obj : group)
	{
		std::stringstream msg;
		msg.precision(6);
		msg << std::scientific;
		// loop over dimensions of object
		for (int k = 0; k < objectIndices[obj].size(); k++)
		{
			msg << std::setw(1) << objectIndices[obj].size() << "-dim." << std::setw(15) << objectTypes[obj] << " | " << std::setw(28) << objectNames[obj] << " | ";
			// loop over degrees of freedom of group
			for (int j = 0; j < dimKernGroup; j++)
			{
				double d = kernGroupBaseVectors.at(j)(objectIndices[obj][k]);
				msg << std::setw(20) << d * (fabs(d) > plottingThreshold) << " | ";
			}
			logWarning() << msg.str();
			msg.str("");
		}
		msg << std::string(lineWidth, '-');
		logWarning() << msg.str();
	}

	// check if the movement of the points in this group can be explained by a helmert transformation, only makes sense if group has at least 2 points, maybe 3 points
	if (pointsInGroup.size() > 1)
	{
		logWarning() << "In case these directions can be explained as linear combinations of Helmert transformations acting on these points (in Root coordinates), they "
						"are listed below";

		// this method checks if directions can be interpreted as helmert movements
		std::vector<std::vector<std::string>> correspondingTransformations = interpreteGroupDirectionsAsHelmertMovements(pointsInGroup, kernGroupBaseVectors);
		// this method generates the error message
		plotTransformationMessage(correspondingTransformations);
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
	// compute the direction of movement of a point transformed to root defined by the ambiguous direction associated to the nullspace vector.

	// the point coordinates in root p_root are a function of the point coordinates in the subframe (p_sub) + the helmert parameters (p1,..,pn) defining the chain from
	// sub to root p_root(p_sub,p1,..,p_n) so the derivative of the point coordinates in root can be computed via the chain rule (let d p_sub, d p_i be the directions
	// indicated in the nullspace) d p_root = d p_root(p_sub,p1,..,p_n)/d p_sub * d p_sub + sum i=1..n d p_root(p_sub,p1,..,p_n)/d p_i * d p_i

	LGCAdjustablePoint point = projData.getPoints().getObject(pointName);
	TPositionVector positionInSubframe = point.getEstimatedValue();
	// get the transformation to root
	int dimBlock = nullspaceBlock.cols();
	TLOR2LOR sub2Root(point.getFrameTreePosition(), projData.getTree().begin(), "sub2Root");
	TDenseMatrix ptJacobian(3, dimBlock);
	ptJacobian.setZero();
	// the derivative corresponding to the point variable itself
	TDenseMatrix ptInSubframeMovement(3, dimBlock);
	ptInSubframeMovement.setZero();
	if (point.getNumUnkn() > 0)
	{
		ptInSubframeMovement(point.getRelativeUnknIndices(), Eigen::indexing::all) = nullspaceBlock.middleRows(point.getFirstUidx(), point.getNumUnkn());
		ptJacobian = (sub2Root.getPartialDerivativeWrtPosition()) * ptInSubframeMovement;
	}
	// the contributions from the chain of transformations
	std::vector<std::pair<TAdjustableHelmertTransformation, TDenseMatrix>> chainSensitivities = sub2Root.getPartialDerivativesWrtHelmertParameters(positionInSubframe);
	for (const auto &[trafo, partDerivWrtParameters] : chainSensitivities)
	{
		if (trafo.getNumUnkn() > 0)
		{
			TDenseMatrix Hp = partDerivWrtParameters(Eigen::indexing::all, trafo.getRelativeUnknIndices());
			TDenseMatrix insensitiveHelmertDirections = nullspaceBlock.middleRows(trafo.getFirstUidx(), trafo.getNumUnkn());
			ptJacobian += Hp * insensitiveHelmertDirections;
		}
	}

	return ptJacobian;
}

std::vector<int> TLSConsCheck::getIndicesOfRow(const Eigen::SparseMatrix<double, Eigen::RowMajor> &M, int rowNumber)
{
	std::vector<int> colIndices;
	// use the sparsity pattern
	for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator it(M, rowNumber); it; ++it)
	{
		colIndices.emplace_back(it.col());
	}

	return colIndices;
}

std::vector<std::string> TLSConsCheck::involvedHelmertComponents(TVector linComb)
{
	std::vector<std::string> components;
	if (linComb.rows() != 7)
	{
		throw std::logic_error("Linear combination of Helmert derivatives has to have 7 components.)");
	}
	std::vector<std::string> trafoComponents({"TX", "TY", "TZ", "RX", "RY", "RZ", "SCL"});
	for (int trafoNum = 0; trafoNum < 7; trafoNum++)
	{
		if (fabs(linComb(trafoNum)) > pivotThreshold)
		{
			components.push_back(trafoComponents.at(trafoNum));
		}
	}
	return components;
}

std::vector<std::vector<std::string>> TLSConsCheck::interpreteGroupDirectionsAsHelmertMovements(const std::set<int> &pointsInGroup, const std::vector<TDenseMatrix> &kernGroupBaseVectors)
{
	std::vector<std::vector<std::string>> result;
	int nPoints = pointsInGroup.size();

	int dimKernGroup = kernGroupBaseVectors.size();

	TDenseMatrix nullspaceOfGroup(projData.fUEOIndices.UIndex, dimKernGroup);
	for (int i = 0; i < dimKernGroup; i++)
	{
		nullspaceOfGroup.col(i) = kernGroupBaseVectors.at(i);
	}

	TDenseMatrix helmertMovementsInRoot(3 * nPoints, 7);
	TDenseMatrix ambiguousGroupDirectionsInRoot(3 * nPoints, dimKernGroup);
	int j = 0;
	for (int k : pointsInGroup)
	{
		helmertMovementsInRoot.middleRows(3 * j, 3) = getMasterDirections(objectNames.at(k));
		ambiguousGroupDirectionsInRoot.middleRows(3 * j, 3) = getAmbiguousDirectionsInRoot(objectNames.at(k), nullspaceOfGroup);
		j++;
	}

	// check each column of the group-nullspace
	for (int j = 0; j < dimKernGroup; j++)
	{
		std::vector<std::string> directionResult;
		TVector pointNullspaceVector = ambiguousGroupDirectionsInRoot.col(j);
		// can it be expressed by the span of the helmertMovements?
		Eigen::VectorXd linComb = helmertMovementsInRoot.fullPivHouseholderQr().solve(pointNullspaceVector);
		std::string helmertString;
		// use relative threshold for scale-independence
		double residualNorm = (helmertMovementsInRoot * linComb - pointNullspaceVector).norm();
		double dirNorm = pointNullspaceVector.norm();
		double relativeResidual = (dirNorm > pivotThreshold) ? (residualNorm / dirNorm) : residualNorm;
		bool isInSpan = (relativeResidual < pivotThreshold);
		bool isTrivial = pointNullspaceVector.isZero();
		if (isTrivial)
		{
			// in this case the non-uniqueness direction transformed to Root is zero which means the root coordinates of the point are actually determinable but the subframe coordinates and some frame freedoms cancel each other out creating a inconsistency
			directionResult.push_back("stationary");
		}
		else if (isInSpan)
		{
			std::vector<std::string> involvedComps = involvedHelmertComponents(linComb);
			directionResult = involvedComps;
		}
		else
		{
			// direction is no linear combination of Helmert trafos
			directionResult.push_back("notInterpretable");
		}
		result.push_back(directionResult);
	}

	return result;
}

std::tuple<std::set<std::string>, Eigen::VectorXd, Eigen::VectorXd> TLSConsCheck::getAffectedPointsAndRootMovements(const std::set<int> &group, const Eigen::VectorXd &nullspaceVector)
{
	std::set<std::string> affectedPoints;
	std::unordered_map<std::string, Eigen::Vector3d> points2Movements;
	std::unordered_map<std::string, Eigen::Vector3d> points2Positions;

	// initialize the points with the points that are in the associated group
	std::set<int> pointNumbers = getPoints(group);
	for (auto number : pointNumbers)
	{
		if (objectTypes[number] != "Point")
		{
			throw std::runtime_error("Object " + objectNames[number] + " is not of type POINT.");
		}
		std::string pointName = objectNames[number];
		affectedPoints.insert(pointName);
		Eigen::Vector3d rootDirection;
		rootDirection = getAmbiguousDirectionsInRoot(pointName, nullspaceVector);
		points2Movements[pointName] = rootDirection;
		// also compute root positions
		LGCAdjustablePoint point = projData.getPoints().getObject(pointName);
		// transform point to root coordinates
		TLOR2LOR sub2Root(point.getFrameTreePosition(), projData.getTree().begin(), "sub2Root");
		TPositionVector positionInRoot = point.getEstimatedValue();
		sub2Root.transform(positionInRoot);
		points2Positions[pointName] = positionInRoot.toRealVector();
	}

	// check remaining points: only compute expensive root directions for points
	// whose own unknowns or frame chain unknowns have nonzero nullspace entries
	for (auto &point : projData.getPoints())
	{
		std::string pointName = point.getName();
		if (affectedPoints.count(pointName))
			continue; // already processed from group

		// quick check: does this point have unknowns that overlap with nonzero nullspace entries?
		bool mayBeAffected = false;
		if (point.getNumUnkn() > 0)
			for (int i = 0; i < point.getNumUnkn(); i++)
				if (std::abs(nullspaceVector(point.getFirstUidx() + i)) > 1e-12)
				{
					mayBeAffected = true;
					break;
				}

		// also check frame chain parameters
		if (!mayBeAffected)
		{
			TLOR2LOR sub2Root(point.getFrameTreePosition(), projData.getTree().begin(), "sub2Root");
			for (const auto &[trafo, partDeriv] : sub2Root.getPartialDerivativesWrtHelmertParameters(point.getEstimatedValue()))
				if (trafo.getNumUnkn() > 0)
				{
					for (int i = 0; i < trafo.getNumUnkn(); i++)
						if (std::abs(nullspaceVector(trafo.getFirstUidx() + i)) > 1e-12)
						{
							mayBeAffected = true;
							break;
						}
					if (mayBeAffected)
						break;
				}
		}

		if (!mayBeAffected)
			continue;

		Eigen::Vector3d rootDirection = getAmbiguousDirectionsInRoot(pointName, nullspaceVector);
		if (rootDirection.norm() > 1e-12)
		{
			affectedPoints.insert(pointName);
			points2Movements[pointName] = rootDirection;
			LGCAdjustablePoint pt = projData.getPoints().getObject(pointName);
			TLOR2LOR sub2Root(pt.getFrameTreePosition(), projData.getTree().begin(), "sub2Root");
			TPositionVector positionInRoot = pt.getEstimatedValue();
			sub2Root.transform(positionInRoot);
			points2Positions[pointName] = positionInRoot.toRealVector();
		}
	}
	// create a vector with all directions of affected points concatenated. The order is the lexicographic order of the point names.
	int numberOfAffectedPoints = affectedPoints.size();
	Eigen::VectorXd names2Movements(3 * numberOfAffectedPoints);
	Eigen::VectorXd names2Positions(3 * numberOfAffectedPoints);
	names2Movements.setZero();
	names2Positions.setZero();
	int j = 0;
	for (auto pointName : affectedPoints)
	{
		names2Movements.segment(3 * j, 3) = points2Movements[pointName];
		names2Positions.segment(3 * j, 3) = points2Positions[pointName];
		j++;
	}

	return {affectedPoints, names2Movements, names2Positions};
}

void TLSConsCheck::plotTransformationMessage(std::vector<std::vector<std::string>> input)
{
	std::stringstream msg;
	msg.precision(6);
	msg << std::scientific;
	msg << std::setw(54) << "involved Transformations |";

	// check each column of the group-nullspace
	for (int j = 0; j < input.size(); j++)
	{
		std::string helmertString;
		for (auto comp : input.at(j))
		{
			helmertString.append(comp);
			helmertString.append(" ");
		}
		msg << std::setw(22) << helmertString << "|";
	}
	logWarning() << msg.str();
}

bool TLSConsCheck::findRotationAxis(const Eigen::VectorXd &positions, const Eigen::MatrixXd &directions)
{
	// default: no rotation center/axis found
	bool identifiedAsRotation = false;
	if (positions.rows() != directions.rows())
	{
		throw std::runtime_error("Dimensions inconsistent.");
	}
	if (positions.rows() % 3 != 0)
	{
		throw std::runtime_error("Dimension of vector representing positions has to be a multiple of 3.");
	}
	int nPoints = positions.rows() / 3;
	int nDirections = directions.cols();

	// find the rotation center
	// if there is a rotation center and the directions represent a rotation around this point, the directions need to be orthogonal to the vector from the center to the point
	// so <(p_i-c),d_i>=0 needs to hold for all points i. This gives nPoint equations linear in c \in R^3 so we can try to solve it.
	// If theres a solution it will be the rotation center. If its not solvable the directions dont represent a rotation.

	// compute matrix representation of c-><c,d_i>_i=1..nPoint
	Eigen::MatrixXd matRep(nPoints * nDirections, 3);
	// compute b=<p_i,d_i>_i=1..nPoint
	Eigen::VectorXd b(nPoints * nDirections);
	for (int j = 0; j < nPoints; j++)
	{
		for (int k = 0; k < nDirections; k++)
		{
			matRep.row(j * nDirections + k) = directions.col(k).segment(j * 3, 3);
			b(j * nDirections + k) = positions.segment(j * 3, 3).dot(directions.col(k).segment(j * 3, 3));
		}
	}
	Eigen::MatrixXd axisDirection = matRep.fullPivLu().kernel();
	int axisDimension = matRep.fullPivLu().dimensionOfKernel();
	// try to solve the linear equation matRep*c=b
	Eigen::Vector3d solution = matRep.fullPivHouseholderQr().solve(b);
	bool solutionExists = (matRep * solution - b).norm() < pivotThreshold;
	if (solutionExists)
	{
		logWarning() << "succesfully identified a rotation axis: " << std::endl << solution << " + t* " << axisDirection;
		identifiedAsRotation = true;
		// if the dimension of the kernel is zero and the solution is still valid, then this means that there are several ambiguous rotations and their axis intersect in
		// the solution point check if any of the existing points is sitting on this axis
		for (auto point : projData.getPoints())
		{
			std::string pointName = point.getName();
			// transform point to root coordinates
			TLOR2LOR sub2Root(point.getFrameTreePosition(), projData.getTree().begin(), "sub2Root");
			TPositionVector positionInRoot = point.getEstimatedValue();
			sub2Root.transform(positionInRoot);
			Eigen::Vector3d pos = positionInRoot.toRealVector();
			// test if the point is on the rotation axis
			Eigen::VectorXd t = axisDirection.fullPivHouseholderQr().solve(pos - solution);
			bool isOnAxis = (axisDirection * t).isApprox(pos - solution, pivotThreshold);
			if (isOnAxis)
			{
				logWarning() << "Point \" " << pointName << "\" is on this axis in Root. Are the other points rotating around this point?";
			}
		}
	}
	else
	{
		logWarning() << "no rotation center found for nullspace direction" << std::endl;
	}
	return identifiedAsRotation;
}

bool TLSConsCheck::findDirectionsToBlock(std::array<bool, 7> &chosenConstraints, const Eigen::MatrixXd &helmertMovements, const Eigen::VectorXd &pointPositions, const Eigen::MatrixXd &nullspaceDirections)
{
	bool foundConstraints = false;

	// do some initial checks
	if (fmod(pointPositions.rows(), 3) != 0)
	{
		throw std::runtime_error("Dimension of Vector of positions has to be multiple of 3");
	}
	int nAffectedPoints = pointPositions.rows() / 3;

	if (helmertMovements.rows() != nullspaceDirections.rows())
	{
		throw std::runtime_error("Helmert directions and nullspace directions must span subspaces of the same vector space.");
	}

	// if there is only one point affected, it is impossible to tell if the ambiguity corresponds to a rotation or translation
	// the helmert movements can not be cleanly separated
	if (nAffectedPoints == 1)
	{
		logWarning() << "Only one point affected, impossible to interprete direction in a unique way as linear combination of translations and rotations.";
		return false;
	}

	// if the dimension of the nullspace directions is greater then 7 it is impossible to interprete all as linear combination of the 7 Helmert directions
	// the dimension of the nullspace directions is the number of columns. The nullspaceDirections matrix is constructed to have linear independent columns
	int dimNullspace = nullspaceDirections.cols();
	if (dimNullspace > 7)
	{
		logWarning() << "Dimension of the group nullspace is greater then 7 and it is impossible to explain all these directions as linear combination of "
						"translations, rotations and scale.";
		return false;
	}

	// 1. project nullspace directions onto Helmert span via least-squares
	Eigen::MatrixXd nullspaceAsHelmert = helmertMovements.fullPivHouseholderQr().solve(nullspaceDirections);

	// verify that each direction is actually representable as a Helmert movement
	// use relative threshold: residual/direction_norm < threshold
	Eigen::MatrixXd residual = helmertMovements * nullspaceAsHelmert - nullspaceDirections;
	for (int j = 0; j < dimNullspace; j++)
	{
		double resNorm = residual.col(j).norm();
		double dirNorm = nullspaceDirections.col(j).norm();
		double relativeResidual = (dirNorm > pivotThreshold) ? (resNorm / dirNorm) : resNorm;
		if (relativeResidual > pivotThreshold)
		{
			logWarning() << "not all ambiguous directions can be represented as helmert movements.";
			return false;
		}
	}

	// 2. check dimension
	int dimHelmert = nullspaceAsHelmert.fullPivHouseholderQr().rank();
	if (dimHelmert < dimNullspace)
	{
		logWarning() << "not all ambiguous directions can be represented as helmert movements.";
		return false;
	}
	// 3. separate into pure translations and pure rotations/scale
	// Pure translations: directions with zero rotation/scale components (rows 3-6)
	// Clean up numerical noise before LU decomposition (use absolute threshold)
	Eigen::MatrixXd cleanedNullspaceAsHelmert = nullspaceAsHelmert;
	cleanedNullspaceAsHelmert = (cleanedNullspaceAsHelmert.array().abs() < pivotThreshold).select(0.0, cleanedNullspaceAsHelmert);

	Eigen::FullPivLU<Eigen::MatrixXd> luRotScale(cleanedNullspaceAsHelmert.bottomRows(4));
	luRotScale.setThreshold(pivotThreshold);
	Eigen::MatrixXd pureTransCoeffs = luRotScale.kernel();
	Eigen::MatrixXd pureTrans;
	if (pureTransCoeffs.cols() > 0)
		pureTrans = cleanedNullspaceAsHelmert * pureTransCoeffs;
	else
		pureTrans = Eigen::MatrixXd::Zero(7, 0);
	int dimPureTrans = (pureTrans.cols() > 0) ? pureTrans.fullPivHouseholderQr().rank() : 0;

	// Pure rotations/scale: directions with zero translation components (rows 0-2)
	Eigen::FullPivLU<Eigen::MatrixXd> luTrans(cleanedNullspaceAsHelmert.topRows(3));
	luTrans.setThreshold(pivotThreshold);
	Eigen::MatrixXd pureRotScaleCoeffs = luTrans.kernel();
	Eigen::MatrixXd pureRotAndScale;
	if (pureRotScaleCoeffs.cols() > 0)
		pureRotAndScale = cleanedNullspaceAsHelmert * pureRotScaleCoeffs;
	else
		pureRotAndScale = Eigen::MatrixXd::Zero(7, 0);
	int dimPureRotAndScale = (pureRotAndScale.cols() > 0) ? pureRotAndScale.fullPivHouseholderQr().rank() : 0;

	// 4. determine what to block for the rotation/scale part
	Eigen::MatrixXd rest;
	if (dimPureTrans + dimPureRotAndScale == dimNullspace)
	{
		rest = pureRotAndScale;
	}
	else
	{
		// complement of pureTrans in cleanedNullspaceAsHelmert, with translation rows zeroed
		if (pureTransCoeffs.cols() > 0 && dimPureTrans > 0)
		{
			Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qrPT(pureTransCoeffs);
			qrPT.setThreshold(pivotThreshold);
			Eigen::MatrixXd Q = qrPT.householderQ() * Eigen::MatrixXd::Identity(dimNullspace, dimNullspace);
			rest = cleanedNullspaceAsHelmert * Q.rightCols(dimNullspace - dimPureTrans);
		}
		else
		{
			rest = cleanedNullspaceAsHelmert;
		}
		// zero translation rows so the greedy only picks rotation/scale parameters
		rest.topRows(3).setZero();
	}

	// 5. block pure translations and rest separately
	std::array<bool, 7> translationConstraints = whatToBlock(pureTrans);
	std::array<bool, 7> remainingConstraints = whatToBlock(rest);

	// combine both
	for (int j = 0; j < 7; j++)
	{
		chosenConstraints[j] = translationConstraints[j] || remainingConstraints[j];
	}
	int numberOfConstraintsFound = std::count(std::begin(chosenConstraints), std::end(chosenConstraints), true);
	if (numberOfConstraintsFound == dimNullspace)
	{
		foundConstraints = true;
	}
	else
	{
		logWarning() << "Could not find enough constraints for problematic group.";
		return false;
	}
	return foundConstraints;
}

bool TLSConsCheck::getResultStatus()
{
	return resultStatus;
}

std::set<int> TLSConsCheck::getNullspaceNeighbors(int object)
{
	return nullspaceNeighbors.at(object);
}

std::set<std::set<int>> TLSConsCheck::identifyConnectedNullspaceGroups()
{
	std::unordered_map<int, int> parent;
	for (int obj : nullspaceObjects)
		parent[obj] = obj;

	// find with path compression
	std::function<int(int)> find = [&](int x) {
		return parent[x] == x ? x : parent[x] = find(parent[x]);
	};

	// union by nullspace neighbors
	for (int obj : nullspaceObjects)
		for (int neighbor : nullspaceNeighbors[obj])
			if (nullspaceObjects.count(neighbor))
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

std::set<int> TLSConsCheck::getConnectedNullspaceGroup(int i)
{
	// get the connected group of object i
	std::set<int> group({i});
	std::set<int> newGroup;
	std::set<int> added({i});
	while (added.size() > 0)
	{
		newGroup = group;
		for (auto object : added)
		{
			// add the neighbors of objects that were added in the previous round that are in the kernel
			for (auto toBeInserted : getNullspaceNeighbors(object))
			{
				newGroup.insert(toBeInserted);
			}
		}
		// check which points have been added
		added.clear();
		set_difference(newGroup.begin(), newGroup.end(), group.begin(), group.end(), inserter(added, added.begin()));
		group = newGroup;
	}
	return group;
}

bool TLSConsCheck::computeNecessaryLIBRConstraints(std::list<TLGCPointConstraintGroup> &proposedPointGroupConstraints)
{
	// go over each group  and transform each ambiguous direction int point movements in root.
	std::vector<std::tuple<std::set<std::string>, Eigen::VectorXd, Eigen::VectorXd>> groupsOfAffectedPoints;
	for (auto group : connectedNullspaceGroups)
	{
		// compute nullspace associated to this group
		std::vector<TDenseMatrix> kernGroupBaseVectors = computeKernelWrtObjectSet(group);
		for (auto nullspaceVector : kernGroupBaseVectors)
		{
			groupsOfAffectedPoints.push_back(getAffectedPointsAndRootMovements(group, nullspaceVector));
		}
	}

	// collect all directions affecting the same sets of points
	// need map here because std::set has no default hash function
	std::map<std::set<std::string>, Eigen::MatrixXd> groups2AmbiguousDirections;
	std::map<std::set<std::string>, Eigen::MatrixXd> groups2HelmertMovements;
	std::map<std::set<std::string>, Eigen::VectorXd> groups2Positions;

	for (auto affectedPointGroup : groupsOfAffectedPoints)
	{
		int nRows = 3 * std::get<0>(affectedPointGroup).size();
		std::set<std::string> pointNames = std::get<0>(affectedPointGroup);
		Eigen::VectorXd rootDirections = std::get<1>(affectedPointGroup);
		Eigen::VectorXd rootPositions = std::get<2>(affectedPointGroup);
		Eigen::MatrixXd newDir = groups2AmbiguousDirections[pointNames];
		// prepare the concatentaion of a new column with the corresponding root direction
		newDir.conservativeResize(nRows, newDir.cols() + 1);
		newDir.col(newDir.cols() - 1) = rootDirections;
		groups2AmbiguousDirections[pointNames] = newDir;
		groups2Positions[pointNames] = rootPositions;

		// now for this group compute the hypothetical action af a helmert transformation (in root coordinates)
		Eigen::MatrixXd helmertMovements = Eigen::MatrixXd::Zero(nRows, 7);
		int i = 0;
		for (auto point : pointNames)
		{
			helmertMovements.middleRows(3 * i, 3) = getMasterDirections(point);
			i++;
		}
		groups2HelmertMovements[pointNames] = helmertMovements;
	}

	// go over each group and find blocking constraints
	int numberConstraintsAdded = 0;
	for (auto group : groups2HelmertMovements)
	{
		// 1. do initial checks:
		// how much ambiguous dimensions (>7??)
		// ambiguous directions subspace of helmert?
		// how many points affected? should be at least 2, better 3 otherwise no clean separation into Helmert directions possible
		// 2. try to decompose into pure translations and pure rest, if not possible decompose into pure translations and orthogonal complement
		// 3. find blocking constraints for the found ambiiguous directions
		// 4. count how much constraints were found and compare to dimension of nullspace, success only if both are equal

		std::set<std::string> pointNames = group.first;
		Eigen::MatrixXd helmertMovements = groups2HelmertMovements[pointNames];
		Eigen::VectorXd pointPositions = groups2Positions[pointNames];
		Eigen::MatrixXd nullspaceDirections = groups2AmbiguousDirections[pointNames];

		std::array<bool, 7> chosenConstraints{};
		bool constraintsFound = findDirectionsToBlock(chosenConstraints, helmertMovements, pointPositions, nullspaceDirections);
		if (!constraintsFound)
		{
			logWarning() << "Could not find blocking constraints for a problematic group.";
			return false;
		}

		// use the found constraints and define a point constraint group
		TLGCPointConstraintGroup newConstraintGroup(projData, chosenConstraints, group.first);

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
		// make sure already selected indices remain strictly zero
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

Eigen::MatrixXd TLSConsCheck::intersect(const Eigen::MatrixXd &A, const Eigen::MatrixXd &B)
{
	// compute im(A) ∩ im(B) via principal angles (SVD of QA^T * QB)
	int nRows = A.rows();
	if (nRows != B.rows())
	{
		throw std::runtime_error("To compute the intersection of the images of matrix A and matrix B they must have the same row-dimension.");
	}

	// orthonormal bases via column-pivoted QR
	Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qrA(A), qrB(B);
	qrA.setThreshold(pivotThreshold);
	qrB.setThreshold(pivotThreshold);
	int rankA = qrA.rank();
	int rankB = qrB.rank();
	if (rankA == 0 || rankB == 0)
		return Eigen::MatrixXd::Zero(nRows, 0);

	Eigen::MatrixXd QA = qrA.householderQ() * Eigen::MatrixXd::Identity(nRows, rankA);
	Eigen::MatrixXd QB = qrB.householderQ() * Eigen::MatrixXd::Identity(nRows, rankB);

	// SVD of QA^T * QB: singular values are cosines of principal angles
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(QA.transpose() * QB, Eigen::ComputeThinU);

	// singular values near 1.0 indicate shared directions
	int dimIntersection = 0;
	for (int i = 0; i < svd.singularValues().size(); i++)
		if (svd.singularValues()(i) > 1.0 - pivotThreshold)
			dimIntersection++;

	if (dimIntersection == 0)
		return Eigen::MatrixXd::Zero(nRows, 0);

	return QA * svd.matrixU().leftCols(dimIntersection);
}

Eigen::MatrixXd TLSConsCheck::orthogonalComplement(const Eigen::MatrixXd &U, const Eigen::MatrixXd &V)
{
	// compute orthogonal complement of U in V via SVD
	if (U.rows() != V.rows())
		throw std::runtime_error("Inconsistent dimensions.");

	if (U.cols() == 0 || (U.cols() == 1 && U.norm() <= pivotThreshold))
		return V;

	// orthonormal basis for V
	Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qrV(V);
	qrV.setThreshold(pivotThreshold);
	int dimV = qrV.rank();
	if (dimV == 0)
		return Eigen::MatrixXd::Zero(V.rows(), 0);

	Eigen::MatrixXd QV = qrV.householderQ() * Eigen::MatrixXd::Identity(V.rows(), dimV);

	// project U into V-coordinates, SVD reveals which directions U occupies
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(QV.transpose() * U, Eigen::ComputeFullU);

	int dimU = 0;
	for (int i = 0; i < svd.singularValues().size(); i++)
		if (svd.singularValues()(i) > pivotThreshold)
			dimU++;

	int dimComplement = dimV - dimU;
	if (dimComplement <= 0)
		return Eigen::MatrixXd::Zero(V.rows(), 0);

	// last dimComplement columns of full U are orthogonal to U within V
	return QV * svd.matrixU().rightCols(dimComplement);
}

void TLSConsCheck::initialize()
{
	// parameters
	for (auto &object : projData.getPoints())
	{
		addObject(object, "Point");
	}
	for (auto &object : projData.getLines())
	{
		addObject(object, "Line");
	}
	for (auto &object : projData.getAngles())
	{
		addObject(object, "Angle");
	}
	for (auto &object : projData.getPlanes())
	{
		addObject(object, "Plane");
	}
	for (auto &object : projData.getLength())
	{
		addObject(object, "Length");
	}
	for (auto it(projData.getTree().begin()); it != projData.getTree().end(); ++it)
	{
		TAdjustableHelmertTransformation trafo(it.node->data.get()->frame);
		addObject(trafo, "Transformation");
	}

	// build column→object lookup
	colToObject.resize(firstDgnMatrix.cols(), -1);
	for (int obj = 0; obj < static_cast<int>(objectIndices.size()); obj++)
		for (int col : objectIndices[obj])
			colToObject[col] = obj;

	// compute neighbors
	// initialize neighbors
	for (auto object : objectNames)
	{
		std::set<int> empty;
		neighbors.push_back(empty);
		nullspaceNeighbors.push_back(empty);
	}
	for (int i = 0; i < firstDgnMatrix.rows(); i++)
	{
		std::vector<int> colIndices = getIndicesOfRow(firstDgnMatrix, i);
		std::set<int> contributingToRow = objectsFromIndices(colIndices);
		for (auto object : contributingToRow)
		{
			for (auto objectToInsert : contributingToRow)
			{
				neighbors[object].insert(objectToInsert);
			}
		}
	}
	nullspace = computeNullspace();

	// compute nullspaceNeighbors (only allow connections to objects contributing
	// to kernel)
	nullspaceObjects = contributingObjects(nullspace);
	for (int i = 0; i < objectNames.size(); i++)
	{
		// nullspaceNeighbors are neighbors and contribute to kernel
		for (auto object : neighbors[i])
		{
			if (nullspaceObjects.count(object) > 0)
			{
				nullspaceNeighbors[i].insert(object);
			}
		}
	}
}
void TLSConsCheck::addObject(TVAdjustableObject &object, const std::string &objectType)
{
	// add name, type and indices
	objectNames.push_back(object.getName());
	objectTypes.push_back(objectType);
	std::vector<int> aux(0);
	for (int i = 0; i < object.getNumUnkn(); i++)
	{
		aux.push_back(object.getFirstUidx() + i);
	}
	objectIndices.push_back(aux);
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

	int nullspaceDim = static_cast<int>(confirmedColumns.size());
	if (nullspaceDim == initialBlockSize)
		logWarning() << "Detected nullspace dimension (" << nullspaceDim
					 << ") equals the initial block size. The true nullspace may be larger.";

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
std::vector<TDenseMatrix> TLSConsCheck::computeKernelWrtObjectSet(const std::set<int> &allowedObjects)
{
	// based on the already computed nullspace, compute a base of the projection onto the subspace defined by the indices of a set of allowed objects.
	// If the set of allowed objects corresponds to a connected nullspace group, there is at least one vector in the nullspace that has nonzero entries only at indices
	// corresponding to objects in this group so in this case this method will return at least one nullspace vector
	std::vector<int> allowedIndices = indicesFromSet(allowedObjects);
	TDenseMatrix projection(allowedIndices.size(), nullspace.cols());
	projection = nullspace(allowedIndices, Eigen::indexing::all);
	// get a base of the projection
	Eigen::FullPivLU<Eigen::MatrixXd> lu(projection);
	int dimOfProjection = lu.rank();
	TDenseMatrix projectionBase(allowedIndices.size(), dimOfProjection);
	TDenseMatrix test = lu.image(projection);
	projectionBase = test.leftCols(dimOfProjection);
	// embed projected image in original space (putting zeros at all other indices)
	TDenseMatrix result(nullspace.rows(), dimOfProjection);
	result.setZero();
	result(allowedIndices, Eigen::indexing::all) = test;
	std::vector<TDenseMatrix> kernGroupBaseVectors;
	for (int j = 0; j < result.cols(); j++)
	{
		kernGroupBaseVectors.push_back(result.col(j));
	}

	return kernGroupBaseVectors;
}
std::vector<int> TLSConsCheck::indicesFromSet(const std::set<int> &objectSet)
{
	// construct indices vector from object set
	std::vector<int> indicesVector;
	for (int object : objectSet)
	{
		for (int index : objectIndices[object])
		{
			indicesVector.push_back(index);
		}
	}
	return indicesVector;
}

std::set<int> TLSConsCheck::objectsFromIndices(const std::vector<int> &x)
{
	std::set<int> result;
	for (int col : x)
		result.insert(colToObject[col]);
	return result;
}

std::set<int> TLSConsCheck::contributingObjects(const TDenseMatrix &M)
{
	// get objects such that M has at least one column that is nonzero on that object-row
	std::set<int> contributing;
	for (int i = 0; i < objectNames.size(); i++)
	{
		TDenseMatrix aux;
		aux = M(objectIndices[i], Eigen::indexing::all);
		if (!aux.isZero())
		{
			contributing.insert(i);
		}
	}
	return contributing;
}

std::pair<std::set<int>, int> TLSConsCheck::externalConnections(const std::set<int> &group)
{
	// for a given group of objects compute the objects in the complement that are connected to this group
	std::set<int> externalConnectedObjects;
	int nConnections = 0;

	for (int row = 0; row < firstDgnMatrix.rows(); row++)
	{
		std::vector<int> colIndices = getIndicesOfRow(firstDgnMatrix, row);
		std::set<int> contributing = objectsFromIndices(colIndices);

		bool hasInternal = false, hasExternal = false;
		for (int obj : contributing)
		{
			if (group.count(obj))
				hasInternal = true;
			else
				hasExternal = true;
		}

		if (hasInternal && hasExternal)
		{
			nConnections++;
			for (int obj : contributing)
				if (!group.count(obj))
					externalConnectedObjects.insert(obj);
		}
	}

	return {externalConnectedObjects, nConnections};
}

Eigen::MatrixXd TLSConsCheck::removeNearZeroNormColumns(const Eigen::MatrixXd &matrix, double threshold)
{
	std::vector<int> colsToKeep;

	for (int i = 0; i < matrix.cols(); ++i)
	{
		if (matrix.col(i).norm() > threshold)
		{
			colsToKeep.push_back(i);
		}
	}

	Eigen::MatrixXd result(matrix.rows(), colsToKeep.size());
	for (size_t i = 0; i < colsToKeep.size(); ++i)
	{
		result.col(i) = matrix.col(colsToKeep[i]);
	}

	return result;
}
