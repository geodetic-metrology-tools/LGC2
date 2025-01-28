// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <Eigen/LU>

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

	//std::cout << "A1;A2" << std::endl << A.toDense() << std::endl;
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

TDenseMatrix TLSConsCheck::getMasterDirections(std::string pointName)
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

TDenseMatrix TLSConsCheck::getAmbiguousDirectionsInRoot(std::string pointName, TDenseMatrix nullspaceBlock)
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
		bool isInSpan = ((helmertMovementsInRoot * linComb - pointNullspaceVector).norm() < pivotThreshold);
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

	// loop over ALL points to find the affected points
	for (auto &point : projData.getPoints())
	{
		std::string pointName = point.getName();
		Eigen::Vector3d rootDirection;
		rootDirection = getAmbiguousDirectionsInRoot(pointName, nullspaceVector);
		if (rootDirection.norm() > 1e-12)
		{
			affectedPoints.insert(pointName);
			points2Movements[pointName] = rootDirection;
			// also compute root positions
			LGCAdjustablePoint point = projData.getPoints().getObject(pointName);
			// transform point to root coordinates
			TLOR2LOR sub2Root(point.getFrameTreePosition(), projData.getTree().begin(), "sub2Root");
			TPositionVector positionInRoot = point.getEstimatedValue();
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

	// 1. interprete all nullspace dirs as helmert directions, first intersect with helmert dirs and then solve
	Eigen::MatrixXd nullspaceAndHelmert = intersect(nullspaceDirections, helmertMovements);
	Eigen::MatrixXd nullspaceAsHelmert = helmertMovements.fullPivHouseholderQr().solve(nullspaceAndHelmert);
	// 2. check dimension
	int dimHelmert = nullspaceAsHelmert.fullPivHouseholderQr().rank();
	if (dimHelmert < dimNullspace)
	{
		logWarning() << "not all ambiguous directions can be represented as helmert movements.";
		return false;
	}
	// 3. find pure translations and pure rot+scale
	Eigen::MatrixXd transBase(7, 3);
	transBase.setZero();
	transBase.topRows(3) = Eigen::MatrixXd::Identity(3, 3);
	Eigen::MatrixXd pureTrans = intersect(nullspaceAsHelmert, transBase);
	Eigen::MatrixXd rotAndScaleBase(7, 4);
	rotAndScaleBase.setZero();
	rotAndScaleBase.bottomRows(4) = Eigen::MatrixXd::Identity(4, 4);
	Eigen::MatrixXd pureRotAndScale = intersect(nullspaceAsHelmert, rotAndScaleBase);
	int dimPureTrans = pureTrans.fullPivHouseholderQr().rank();
	int dimPureRotAndScale = pureRotAndScale.fullPivHouseholderQr().rank();
	// 4. check if pure decomposition already covers all
	Eigen::MatrixXd rest;
	if (dimPureTrans + dimPureRotAndScale == dimNullspace)
	{
		rest = pureRotAndScale;
	}
	else
	{
		rest = orthogonalComplement(pureTrans, nullspaceAsHelmert);
		// make sure that here no translation is blocked anymore
		rest.topRows(3).setZero();
	}
	// 5. block pure translations and rest
	// decide which translations to block
	std::array<bool, 7> translationConstraints = whatToBlock(pureTrans);
	// initialize the rest
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
	std::set<std::set<int>> connectedNullspaceGroups;
	for (auto obj : nullspaceObjects)
	{
		// only search for nontrivial objects
		std::set<int> group = getConnectedNullspaceGroup(obj);
		connectedNullspaceGroups.insert(group);
	}
	return connectedNullspaceGroups;
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
		// find biggest row
		int biggestRowIdx;
		aux.rowwise().norm().maxCoeff(&biggestRowIdx);
		result[biggestRowIdx] = true;
		// compute intersecion of aux with vectors that are zero at the index
		Eigen::MatrixXd projImage(7, 7);
		projImage.setIdentity();
		projImage.row(biggestRowIdx).setZero();
		aux = intersect(aux, projImage);
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
	// imA n imB = P_1 ker(C) with C=[A,-B]
	int nRows = A.rows();
	if (nRows != B.rows())
	{
		throw std::runtime_error("To compute the intersection of the images of matrix A and matrix B they must have the same row-dimension.");
	}
	Eigen::MatrixXd C = Eigen::MatrixXd::Zero(nRows, A.cols() + B.cols());
	C.leftCols(A.cols()) = A;
	C.rightCols(B.cols()) = -B;

	Eigen::FullPivLU<Eigen::MatrixXd> lu_decomp(C);
	lu_decomp.setThreshold(pivotThreshold);
	Eigen::MatrixXd kernel = lu_decomp.kernel();
	Eigen::MatrixXd intersection = A * kernel.topRows(A.cols());
	Eigen::MatrixXd nontrivialIntersection = removeNearZeroNormColumns(intersection, 1e-9);
	return nontrivialIntersection;
}

Eigen::MatrixXd TLSConsCheck::orthogonalComplement(const Eigen::MatrixXd &U, const Eigen::MatrixXd &V)
{
	// compute orthogonal complement of U in V.
	// check dimensions
	if (U.rows() != V.rows())
	{
		throw std::runtime_error("Inconsistent dimensions.");
	}
	int colsU = U.cols();
	int dimU = U.fullPivHouseholderQr().rank();
	int colsV = V.cols();
	int dimV = V.fullPivHouseholderQr().rank();

	if ((U.cols() == 1 && U.norm() <= 1e-9) || U.cols() == 0)
	{
		// complement of empty space is the whole space
		return V;
	}

	// combine both matrices
	Eigen::MatrixXd combinedMat(U.rows(), colsV + colsU);
	combinedMat << U, V;
	// check dimension to see if U really is a subspace of V
	if (combinedMat.fullPivHouseholderQr().rank() > dimV)
	{
		throw std::runtime_error("U is not a subspace of V.");
	}

	// the first dimU columns of Q span U, the next dimV-dimU columns span its orthogonal complement in V
	Eigen::HouseholderQR<Eigen::MatrixXd> qr(combinedMat);
	Eigen::MatrixXd Q = qr.householderQ();

	return Q.middleCols(dimU, dimV - dimU);
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
	for (auto &object : projData.getSags())
	{
		addObject(object, "Sag");
	}
	for (auto it(projData.getTree().begin()); it != projData.getTree().end(); ++it)
	{
		TAdjustableHelmertTransformation trafo(it.node->data.get()->frame);
		addObject(trafo, "Transformation");
	}

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
void TLSConsCheck::addObject(TVAdjustableObject &object, std::string objectType)
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

std::set<int> TLSConsCheck::getPoints(std::set<int> group)
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
	// including this flag to make fullPivLu available for debugging
	bool useFullPivLu = true;
	Eigen::MatrixXd potentialNullspace;
	if (useFullPivLu)
	{
		// use fullpivlu decomposition which has a direct kernel method but needs a dense matrix
		TDenseMatrix firstDense = firstDgnMatrix;
		// compute kernel representation of this matrix
		// with pullpivlu
		Eigen::FullPivLU<TDenseMatrix> lu(firstDense);
		lu.setThreshold(pivotThreshold);
		potentialNullspace = lu.kernel();
	}
	else
	{
		// compute the nullspace of A using qr decomposition of A^T. If the nullspace is nonzero, it corresponds to the rightmost columns of Q
		// QR method is faster then fullPivLU for big matrices
		Eigen::SparseMatrix<double> A = firstDgnMatrix;
		int nVar = A.cols();

		// do a QR decomposition
		Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> qr;
		// qr pivot threshold value tested on database of test problems, 1e-4 giving more robust results compared to Eigens automatic dynamic thershold
		qr.setPivotThreshold(1e-4);
		qr.compute(A.transpose());
		if (qr.info() != Eigen::Success)
		{
			logWarning() << "QR decomposition for nullspace computation failed";
		}

		TDenseMatrix Q = qr.matrixQ();
		potentialNullspace = Q;
	}

	// no need to explicitely compute the rank as we test the nullspace vectors anyway. We assume the nullspace has not more then maxDim dimensions
	std::vector<int> confirmedNullspaceVectors;
	// check if the proposed vectors are really in the nullspace
	int potentialDim = potentialNullspace.cols();
	// start with the rightmost columns as they are the nullspace candidates
	for (int j = potentialDim - 1; j >= 0; j--)
	{
		double testZero = (firstDgnMatrix * potentialNullspace.col(j)).norm();
		if (testZero < pivotThreshold)
		{
			confirmedNullspaceVectors.push_back(j);
		}
		else if (testZero > 1)
		{
			// as the vector is far from being in the nullspace we do not test any further candidates
			break;
		}
	}
	TDenseMatrix confirmedNullspace(firstDgnMatrix.rows(), confirmedNullspaceVectors.size());
	confirmedNullspace = potentialNullspace(Eigen::indexing::all, confirmedNullspaceVectors);

	return confirmedNullspace;
}
std::vector<TDenseMatrix> TLSConsCheck::computeKernelWrtObjectSet(std::set<int> allowedObjects)
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
std::vector<int> TLSConsCheck::indicesFromSet(std::set<int> objectSet)
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

std::set<int> TLSConsCheck::objectsFromIndices(std::vector<int> x)
{
	// create eigen vector with 1 at the corresponding places
	Eigen::VectorXd aux(firstDgnMatrix.cols());
	aux.setZero();
	aux(x) = Eigen::VectorXd::Ones(x.size());
	return contributingObjects(aux);
}

std::set<int> TLSConsCheck::contributingObjects(TDenseMatrix M)
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

std::pair<std::set<int>, int> TLSConsCheck::externalConnections(std::set<int> group)
{
	// for a given group of objects compute the objects in the complement that are connected to this group
	std::set<int> externalConnectedObjects;
	int nConnections = 0;

	std::set<int> complementOfGroup;
	for (int i = 0; i < objectNames.size(); i++)
	{
		if (group.count(i) == 0)
		{
			// i is not in group
			complementOfGroup.insert(i);
		}
	}
	std::vector<int> internalIndices = indicesFromSet(group);
	std::vector<int> externalIndices = indicesFromSet(complementOfGroup);

	for (int row = 0; row < firstDgnMatrix.rows(); row++)
	{
		std::vector<int> colIndices;
		// use sparsity pattern to compute connections
		colIndices = getIndicesOfRow(firstDgnMatrix, row);
		std::set<int> contributing = objectsFromIndices(colIndices);
		// we go through the row, indicesmarker has to have size .cols()
		TDenseMatrix indicesMarker(firstDgnMatrix.cols(), 1);
		indicesMarker.setZero();
		for (auto j : colIndices)
		{
			indicesMarker(j) = 1;
		}
		bool dependsOnGroup = !(indicesMarker(internalIndices, 0).isZero());
		bool dependsOnComplement = !(indicesMarker(externalIndices, 0).isZero());

		if (dependsOnGroup && dependsOnComplement)
		{
			// the measurement involves internal and external objects
			nConnections++;
			for (auto object : contributing)
			{
				if (group.count(object) == 0)
				{
					externalConnectedObjects.insert(object);
				}
			}
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
