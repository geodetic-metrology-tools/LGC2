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
	A.topRows(projData.fUEOIndices.EIndex) = *inputMtr.getFirstDgnMtrx();
	A.bottomRows(projData.fUEOIndices.CIndex) = *inputMtr.getCnstrFirstDgnMtrx();

	firstDgnMatrix = A;

	initialize();

	// identify connected groups of kernel based on the nullspace
	connectedNullspaceGroups = identifyConnectedNullspaceGroups();
	if (connectedNullspaceGroups.size() == 0)
	{
		resultStatus = true;
	}
	if (resultStatus == false)
	{
		generateErrorMessage();
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
		vector<TDenseMatrix> kernGroupBaseVectors = computeKernelWrtObjectSet(group);
		// generate warning log for this group
		generateGroupWarning(group, kernGroupBaseVectors, nGroup);
		nGroup++;
	}
}
void TLSConsCheck::generateGroupWarning(const set<int> group, const vector<TDenseMatrix> kernGroupBaseVectors, const int groupNumber)
{
	// Generate warning message listing the objects and the directions
	// corresponding to the degrees of freedom
	// To improve readability of the output ignore components below threshold.

	// plotting the ambiguous directions
	double plottingThreshold = 1e-12;
	logWarning() << string(100, '=');
	int dimKernGroup = kernGroupBaseVectors.size();
	set<int> pointsInGroup = getPoints(group);
	logWarning() << "Group " << groupNumber << " with" << group.size() << "object(s) (including" << pointsInGroup.size() << "point(s)) and " << dimKernGroup << " Degree(s) of Freedom : ";
	stringstream directionHeader;
	directionHeader << setw(22) << " Type "
					<< "|" << setw(29) << "Object Name"
					<< " | ";
	for (int k = 0; k < dimKernGroup; k++)
	{
		directionHeader << setw(17) << "Direction "
						<< "d_" << k << " | ";
	}
	int lineWidth = directionHeader.tellp();
	logWarning() << directionHeader.str();
	logWarning() << string(lineWidth, '-');
	// loop over objects in group
	for (auto obj : group)
	{
		stringstream msg;
		msg.precision(6);
		msg << scientific;
		// loop over dimensions of object
		for (int k = 0; k < objectIndices[obj].size(); k++)
		{
			msg << setw(1) << objectIndices[obj].size() << "-dim." << setw(15) << objectTypes[obj] << " | " << setw(28) << objectNames[obj] << " | ";
			// loop over degrees of freedom of group
			for (int j = 0; j < dimKernGroup; j++)
			{
				double d = kernGroupBaseVectors.at(j)(objectIndices[obj][k]);
				msg << setw(20) << d * (fabs(d) > plottingThreshold) << " | ";
			}
			logWarning() << msg.str();
			msg.str("");
		}
		msg << string(lineWidth, '-');
		logWarning() << msg.str();
	}

	// check if the movement of the points in this group can be explained by a helmert transformation, only makes sense if group has at least 2 points, maybe 3 points
	if (pointsInGroup.size() > 1)
	{
		logWarning() << "In case these directions can be explained as linear combinations of Helmert transformations acting on these points (in Root coordinates), they "
						"are listed below";

		// this method checks if directions can be interpreted as helmert movements
		vector<vector<string>> correspondingTransformations = interpreteGroupDirectionsAsHelmertMovements(pointsInGroup, kernGroupBaseVectors);
		// this method generates the error message
		plotTransformationMessage(correspondingTransformations);
	}

	// get external connections
	pair<set<int>, int> extCon = externalConnections(group);
	set<int> external = extCon.first;
	int nConnections = extCon.second;
	if (external.size() > 0)
	{
		logWarning() << "This group is connected via" << nConnections << "measurements/constraints to the following" << external.size() << "objects";
		for (auto object : external)
		{
			logWarning() << setw(12) << objectTypes[object] << setw(25) << objectNames[object];
		}
	}
	else
	{
		logWarning() << "This group is isolated from the rest of the (adjustable) objects.";
	}
}

TDenseMatrix TLSConsCheck::getMasterDirections(string pointName)
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

TDenseMatrix TLSConsCheck::getAmbiguousDirectionsInRoot(string pointName, TDenseMatrix nullspaceBlock)
{
	// compute the direction of movement of a point transformed to root defined by the ambiguous direction associated to the nullspace vector.

	LGCAdjustablePoint point = projData.getPoints().getObject(pointName);
	TPositionVector positionInSubframe = point.getEstimatedValue();
	// get the transformation to root
	int dimBlock = nullspaceBlock.cols();
	TLOR2LOR sub2Root(point.getFrameTreePosition(), projData.getTree().begin(), "sub2Root");
	TDenseMatrix ptJacobian(3, dimBlock);
	ptJacobian.setZero();
	// the derivative corresponding to the point itself
	TDenseMatrix ptInSubframeMovement(3, dimBlock);
	ptInSubframeMovement.setZero();
	if (point.getNumUnkn() > 0)
	{
		ptInSubframeMovement(point.getRelativeUnknIndices(), Eigen::indexing::all) = nullspaceBlock.middleRows(point.getFirstUidx(), point.getNumUnkn());
		ptJacobian = (sub2Root.getPartialDerivativeWrtPosition()) * ptInSubframeMovement;
	}
	// the contributions from the chain of transformations
	vector<pair<TAdjustableHelmertTransformation, TDenseMatrix>> chainSensitivities = sub2Root.getPartialDerivativesWrtHelmertParameters(positionInSubframe);
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

vector<int> TLSConsCheck::getIndicesOfRow(const Eigen::SparseMatrix<double, Eigen::RowMajor> &M, int rowNumber)
{
	vector<int> colIndices;
	// use the sparsity pattern
	for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator it(M, rowNumber); it; ++it)
	{
		colIndices.emplace_back(it.col());
	}

	return colIndices;
}

vector<string> TLSConsCheck::involvedHelmertComponents(TVector linComb)
{
	vector<string> components;
	if (linComb.rows() != 7)
	{
		std::logic_error("Linear combination of Helmert derivatives has to have 7 components.)");
	}
	vector<string> trafoComponents({"TX", "TY", "TZ", "RX", "RY", "RZ", "SCL"});
	for (int trafoNum = 0; trafoNum < 7; trafoNum++)
	{
		if (fabs(linComb(trafoNum)) > pivotThreshold)
		{
			components.push_back(trafoComponents.at(trafoNum));
		}
	}
	return components;
}

vector<vector<std::string>> TLSConsCheck::interpreteGroupDirectionsAsHelmertMovements(set<int> pointsInGroup, vector<TDenseMatrix> kernGroupBaseVectors)
{
	vector<vector<std::string>> result;
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
		vector<std::string> directionResult;
		TVector pointNullspaceVector = ambiguousGroupDirectionsInRoot.col(j);
		// can it be expressed by the span of the helmertMovements?
		Eigen::VectorXd linComb = helmertMovementsInRoot.fullPivHouseholderQr().solve(pointNullspaceVector);
		string helmertString;
		bool isInSpan = ((helmertMovementsInRoot * linComb - pointNullspaceVector).norm() < pivotThreshold);
		bool isTrivial = pointNullspaceVector.isZero();
		if (isTrivial)
		{
			// in this case the non-uniqueness direction transformed to Root is zero which means the root coordinates of the point are actually determinable but the subframe coordinates and some frame freedoms cancel each other out creating a inconsistency
			directionResult.push_back("stationary");
		}
		else if (isInSpan)
		{
			vector<string> involvedComps = involvedHelmertComponents(linComb);
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

void TLSConsCheck::plotTransformationMessage(vector<vector<string>> input)
{
	stringstream msg;
	msg.precision(6);
	msg << scientific;
	msg << setw(54) << "involved Transformations |";

	// check each column of the group-nullspace
	for (int j = 0; j < input.size(); j++)
	{
		string helmertString;
		for (auto comp : input.at(j))
		{
			helmertString.append(comp);
			helmertString.append(" ");
		}
		msg << setw(22) << helmertString << "|";
	}
	logWarning() << msg.str();
}

bool TLSConsCheck::getResultStatus()
{
	return resultStatus;
}

set<int> TLSConsCheck::getNullspaceNeighbors(int object)
{
	return nullspaceNeighbors.at(object);
}

set<set<int>> TLSConsCheck::identifyConnectedNullspaceGroups()
{
	set<set<int>> connectedNullspaceGroups;
	for (auto obj : nullspaceObjects)
	{
		// only search for nontrivial objects
		set<int> group = getConnectedNullspaceGroup(obj);
		connectedNullspaceGroups.insert(group);
	}
	return connectedNullspaceGroups;
}

set<int> TLSConsCheck::getConnectedNullspaceGroup(int i)
{
	// get the connected group of object i
	set<int> group({i});
	set<int> newGroup;
	set<int> added({i});
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

void TLSConsCheck::initialize()
{
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
	// now the unknowns associated to transformations.. (as in TLSResultsMatricesExtractor::extractTransformationParams)
	// as there is no "adjustable transformation collection", we have to iterate over the tree and get them on our own.
	for (auto it(projData.getTree().begin()); it != projData.getTree().end(); ++it)
	{
		TAdjustableHelmertTransformation trafo(it.node->data.get()->frame);
		addObject(trafo, "Transformation");
	}

	// compute neighbors
	// initialize neighbors
	for (auto object : objectNames)
	{
		set<int> empty;
		neighbors.push_back(empty);
		nullspaceNeighbors.push_back(empty);
	}
	for (int i = 0; i < firstDgnMatrix.rows(); i++)
	{
		vector<int> colIndices = getIndicesOfRow(firstDgnMatrix, i);
		set<int> contributingToRow = objectsFromIndices(colIndices);
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
void TLSConsCheck::addObject(TVAdjustableObject &object, string objectType)
{
	// add name, type and indices
	objectNames.push_back(object.getName());
	objectTypes.push_back(objectType);
	vector<int> aux(0);
	for (int i = 0; i < object.getNumUnkn(); i++)
	{
		aux.push_back(object.getFirstUidx() + i);
	}
	objectIndices.push_back(aux);
}

set<int> TLSConsCheck::getPoints(set<int> group)
{
	set<int> points;
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
	// get only the columns of the first design matrix that correspond to the test_set of objects
	TDenseMatrix firstDense = firstDgnMatrix;
	// compute kernel representation of this matrix
	// with pullpivlu
	Eigen::FullPivLU<TDenseMatrix> lu(firstDense);
	lu.setThreshold(pivotThreshold);
	TDenseMatrix nullspace = lu.kernel();
	if (nullspace.isZero())
	{
		// if only the zero matrix is returned by eigen, the nullspace has dimension 0
		nullspace.conservativeResize(firstDense.cols(), 0);
	}

	vector<int> confirmedNullspaceVectors;
	// check if the proposed vectors are really in the nullspace
	for (int j = 0; j < nullspace.cols(); j++)
	{
		double testZero = (firstDgnMatrix * nullspace.col(j)).norm();
		if (testZero < pivotThreshold)
		{
			confirmedNullspaceVectors.push_back(j);
		}
	}
	TDenseMatrix confirmedNullspace(nullspace.cols(), confirmedNullspaceVectors.size());
	confirmedNullspace = nullspace(Eigen::indexing::all, confirmedNullspaceVectors);

	return confirmedNullspace;
}
vector<TDenseMatrix> TLSConsCheck::computeKernelWrtObjectSet(set<int> allowedObjects)
{
	// based on the already computed nullspace, compute a base of the projection onto the subspace defined by the indices of a set of allowed objects.
	// If the set of allowed objects corresponds to a connected nullspace group, there is at least one vector in the nullspace that has nonzero entries only at indices
	// corresponding to objects in this group so in this case this method will return at least one nullspace vector
	vector<int> allowedIndices = indicesFromSet(allowedObjects);
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
	vector<TDenseMatrix> kernGroupBaseVectors;
	for (int j = 0; j < result.cols(); j++)
	{
		kernGroupBaseVectors.push_back(result.col(j));
	}

	return kernGroupBaseVectors;
}
vector<int> TLSConsCheck::indicesFromSet(set<int> objectSet)
{
	// construct indices vector from object set
	vector<int> indicesVector;
	for (int object : objectSet)
	{
		for (int index : objectIndices[object])
		{
			indicesVector.push_back(index);
		}
	}
	return indicesVector;
}

set<int> TLSConsCheck::objectsFromIndices(vector<int> x)
{
	// create eigen vector with 1 at the corresponding places
	Eigen::VectorXd aux(firstDgnMatrix.cols());
	aux.setZero();
	aux(x) = Eigen::VectorXd::Ones(x.size());
	return contributingObjects(aux);
}

set<int> TLSConsCheck::contributingObjects(TDenseMatrix M)
{
	// get objects such that M has at least one column that is nonzero on that object-row
	set<int> contributing;
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

pair<set<int>, int> TLSConsCheck::externalConnections(set<int> group)
{
	// for a given group of objects compute the objects in the complement that are connected to this group
	set<int> externalConnectedObjects;
	int nConnections = 0;

	set<int> complementOfGroup;
	for (int i = 0; i < objectNames.size(); i++)
	{
		if (group.count(i) == 0)
		{
			// i is not in group
			complementOfGroup.insert(i);
		}
	}
	vector<int> internalIndices = indicesFromSet(group);
	vector<int> externalIndices = indicesFromSet(complementOfGroup);

	for (int row = 0; row < firstDgnMatrix.rows(); row++)
	{
		vector<int> colIndices;
		// use sparsity pattern to compute connections
		colIndices = getIndicesOfRow(firstDgnMatrix, row);
		set<int> contributing = objectsFromIndices(colIndices);
		TDenseMatrix indicesMarker(firstDgnMatrix.rows(), 1);
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
