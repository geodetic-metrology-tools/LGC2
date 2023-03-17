#include <TSparseMatrix.h>
#include <TLSAlgorithm.h>
#include <Logger.hpp>
#include <TVAdjustableObject.h>
#include <Eigen/LU>
#include <TLSConsistencyCheck.h>
#include <TLOR2LOR.h>
#include <TDirectTransformation.h>

using namespace std;

// constructor
TLSConsCheck::TLSConsCheck(TLGCData& data, const TLSInputMatrices& inputMtr)
{
	Eigen::SparseMatrix<double, Eigen::RowMajor> A(data.fUEOIndices.EIndex + data.fUEOIndices.CIndex, data.fUEOIndices.UIndex);
	A.topRows(data.fUEOIndices.EIndex) = *inputMtr.getFirstDgnMtrx();
	A.bottomRows(data.fUEOIndices.CIndex) = *inputMtr.getCnstrFirstDgnMtrx();
	
    firstDgnMatrix = A;
    initialize(data);
    computeMasterJacobianAndInsensitiveRootDirections(data);

    // test if some Nullspace direction can be explained by a Helmert transformation
	if (nullspace.cols() > 0)
	{
        // test if nullspace directions can be explained with master movements
		for (int i = 0; i < insensitiveRootDirections.cols(); i++)
		{
			// test if i-th column of nullspace directions is in span of master directions
			Eigen::VectorXd b = insensitiveRootDirections.col(i);
			if (b.norm() < 1e-10)
			{
				logWarning() << "The " << i << "-th column of the Nullspace corresponds to stationary root coordinates. This can happen if the coordinates viewd from root are computable but the subframe coordinates cannot be computed because they are free and the subframe is also free."
							 << std::endl;
			}
			else
			{
                // now we test if the movement can be explained by a helmert transformation of all points
				Eigen::VectorXd test = masterJacobian.fullPivHouseholderQr().solve(b);
				logWarning() << "Helmert trafo-test (zero values indicate that a movement can be explained by a helmert transformation )=" << (masterJacobian * test - b).norm()
							 << std::endl;
				logWarning() << test.transpose() << std::endl;
				if ((masterJacobian * test - b).norm() < 1e-8)
				{
					logWarning() << i << "-th column of Nullspace Movements can be explained as" << std::endl;
					// print the helmert trafos that explain the movement
					whichConstraintsDoWeNeed(test);
				}
			}
		}
	}

    // identify connected groups of kernel
    set<set<int>> connectedGroups = identifyConnectedNullspaceGroups();
    int dimKern;
    if (nullspace.isZero()) {
        dimKern = 0;
    }
    else {
        dimKern = nullspace.cols();
    }

    auto& outputMessages(data.getFileLogger());
    outputMessages.writeReportHeader("Geometry consistency check:");
    if (dimKern > 0) {
        outputMessages << TFileLogger::e_logType::LOG_ERROR << "Geometric inconsistency detected, see log2 file.";
        logCritical() << "The Nullspace of the first design matrix is nonzero. There are groups of unidentifiable objects and the estimation problem has no unique solution.";
        logWarning() << "There are " << connectedGroups.size() << " connected Groups of unidentifiable objects.";
    }
    else
    {
        outputMessages << TFileLogger::e_logType::LOG_INFO << "No geometric inconsistency detected.";
        resultStatus = true;
        logWarning() << "No identifiability-problems detected.";
    }

    int nGroup = 0;
    for (auto group : connectedGroups) {
        //TDenseMatrix kernGroup = computeKernelWrtObjectSet(group);
		vector<int> groupIndices = indicesFromSet(group);
		TDenseMatrix kernGroup_aux = nullspace(groupIndices, Eigen::indexing::all);
		TDenseMatrix kernGroup(nullspace.rows(), nullspace.cols());
		kernGroup.setZero();
        // zero columns have to be deleted otherwise every group will be shown as it has the same degrees of freedom as the whole configuration
		kernGroup(groupIndices, Eigen::indexing::all) = kernGroup_aux;
		vector<TDenseMatrix> kernGroupBaseVectors;
		for (int i = 0; i < kernGroup.cols();i++)
		{
			if (kernGroup.col(i).norm() > 1e-8)
			{
				kernGroupBaseVectors.push_back(kernGroup.col(i));
			}
		}
        //TDenseMatrix kernGroup = computeKernelWrtObjectSet(group);
        // generate warning log for this group
		generateGroupWarning(group, kernGroupBaseVectors, nGroup);
        nGroup++;
    }
}

void TLSConsCheck::generateGroupWarning(const set<int> group, const vector<TDenseMatrix> kernGroupBaseVectors, const int groupNumber) {
    // Generate warning message listing the objects and the directions
    // corrsponding to the degrees of freedom
    // To improve readability of the output ignore components below threshold.
    double plottingThreshold = 1e-12;
    stringstream groupHeader;
    groupHeader << string(100, '=');
    logWarning() << groupHeader.str();
    int dimKernGroup = kernGroupBaseVectors.size();
    logWarning() << "Group " << groupNumber << " with" << group.size() << "objects and" << dimKernGroup << "Degrees of Freedom:";
    stringstream directionHeader;
    directionHeader << setw(14) << " Type" << setw(31) << "Object Name" << " | ";
    for (int k = 0; k < dimKernGroup; k++) {
        directionHeader << setw(10) << "Direction " << "d_" << k << " | ";
    }
    int lineWidth = directionHeader.tellp();
    logWarning() << directionHeader.str();
    logWarning() << string(lineWidth, '-');
    // loop over objects in group
    for (auto obj : group) {
        stringstream msg;
        msg.precision(6);
        msg << scientific;
        // loop over dimensions of object
        for (int k = 0; k < objectIndices[obj].size(); k++) {
            msg << setw(1) << objectIndices[obj].size() << "-dim." << setw(8) << objectTypes[obj] << " " << setw(30) << objectNames[obj] << " | ";
            // loop over degrees of freedom of group
            for (int j = 0; j < dimKernGroup; j++) {
                double d = kernGroupBaseVectors.at(j)(objectIndices[obj][k]);
                msg << setw(13) << d * (fabs(d) > plottingThreshold) << " | ";
            }
            logWarning() << msg.str();
            msg.str("");
        }
        msg << string(lineWidth, '-');
        logWarning() << msg.str();
    }
    //get external connections
    pair<set<int>, int> extCon = externalConnections(group);
    set<int> external = extCon.first;
    int nConnections = extCon.second;
    if (external.size() > 0) {
        logWarning() << "This group is connected via" << nConnections << "measurements/constraints to the following" << external.size() << "objects";
        for (auto object : external) {
            logWarning() << setw(12) << objectTypes[object] << setw(25) << objectNames[object];
        }
    }
    else {
        logWarning() << "This group is isolated from the rest of the objects.";
    }

}

TDenseMatrix TLSConsCheck::getMasterJacobian(const TLGCData &data)
{
	// create the "Master" Helmert transformation object acting on the Root frame
	// equal to the identity transformation
	// for our purpose we use the default constructor of TDirectTransformation, which gives an identity transformation
	TDirectTransformation masterTrafo;
	// total number of points, independant of degrees of freedom of point
	int numberOfPoints = data.getPoints().numObjects();
	TDenseMatrix masterJacobian(3 * numberOfPoints, 7);
	TDataTree tree = data.getTree();
	// iterate over all adjustable points
	int pointCounter = 0;
	for (const auto &point : data.getPoints())
	{
		// transform point to root coordinates
		TLOR2LOR sub2Root(point.getFrameTreePosition(), tree.begin(), "pointTrafo");
		TPositionVector positionInRoot = point.getEstimatedValue();
		sub2Root.transform(positionInRoot);
		TDenseMatrix ptJacobian(3, 7);
		ptJacobian = masterTrafo.getPartialDerivativeWrtHelmertParameters(positionInRoot);
		masterJacobian.block(3 * pointCounter, 0, 3, 7) = ptJacobian;
		pointCounter++;
	}
	return masterJacobian;
}

TDenseMatrix TLSConsCheck::getInsensitiveDirectionsInRoot(const TLGCData &data)
{
    // translate the Nullspace of the first design matrix to insensitive directions in the Root frame
    // for each point, the chain of transformations to root (inlcuding their sensitivities) have to be considered
	// total number of points, independant of degrees of freedom of point
	int numberOfPoints = data.getPoints().numObjects();
	int dimNullspace = nullspace.cols();
	TDenseMatrix insensitiveDirections(3 * numberOfPoints, dimNullspace);
	TDataTree tree = data.getTree();
	int pointCounter = 0;
	for (const auto &point : data.getPoints())
	{
		TPositionVector positionInSubframe = point.getEstimatedValue();
		// get the transformation to root
		TLOR2LOR sub2Root(point.getFrameTreePosition(), tree.begin(), "pointTrafo");
		TDenseMatrix ptJacobian(3, dimNullspace);
		ptJacobian.setZero();
		// the derivative corresponding to the point itself
		TDenseMatrix ptInSubframeMovement(3, dimNullspace);
		ptInSubframeMovement.setZero();
		if (point.getNumUnkn() > 0)
		{
			ptInSubframeMovement(point.getRelativeUnknIndices(), Eigen::indexing::all) = nullspace.block(point.getFirstUidx(), 0, point.getNumUnkn(), dimNullspace);
			ptJacobian = (sub2Root.getPartialDerivativeWrtPosition()) * ptInSubframeMovement;
		}
		// the contributions from the chain of transformations
		std::vector<std::pair<TAdjustableHelmertTransformation, TDenseMatrix>> chainSensitivities = sub2Root.getPartialDerivativesWrtHelmertParameters(positionInSubframe);
		for (auto &pair : chainSensitivities)
		{
			TAdjustableHelmertTransformation trafo = pair.first;
			TDenseMatrix partDerivWrtParameters = pair.second;
			if (trafo.getNumUnkn() > 0)
			{
				TDenseMatrix Hp(3, trafo.getNumUnkn());
				Hp.setZero();
				Hp = partDerivWrtParameters(Eigen::indexing::all, trafo.getRelativeUnknIndices());
				TDenseMatrix helmertDirections(trafo.getNumUnkn(), dimNullspace);
				helmertDirections = nullspace.block(trafo.getFirstUidx(), 0, trafo.getNumUnkn(), dimNullspace);
				ptJacobian += Hp * helmertDirections;
			}
		}
        insensitiveDirections.block(pointCounter*3,0,3,dimNullspace)=ptJacobian;
		pointCounter++;
	}
	return insensitiveDirections;
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


void TLSConsCheck::whichConstraintsDoWeNeed(Eigen::VectorXd combi)
{
	if (!(combi.size() == 7))
	{
		logWarning() << "linear combination needs to have 7 coefficients because a Helmert transformation has 7 coefficients." << std::endl;
		return;
	}
	double threshold = 1e-6;
	logWarning() << "a linear combination of ";
	if (abs(combi(0)) > threshold)
	{
		logWarning() << "a x-translation, ";
	}
	if (abs(combi(1)) > threshold)
	{
		logWarning() << "a y-translation, ";
	}
	if (abs(combi(2)) > threshold)
	{
		logWarning() << "a z-translation, ";
	}
	if (abs(combi(3)) > threshold)
	{
		logWarning() << "a rotation around x, ";
	}
	if (abs(combi(4)) > threshold)
	{
		logWarning() << "a rotation around y, ";
	}
	if (abs(combi(5)) > threshold)
	{
		logWarning() << "a rotation around z, ";
	}
	if (abs(combi(6)) > threshold)
	{
		logWarning() << "a scaling ";
	}
	logWarning()<<std::endl;
	logWarning() << "adding a constraint prohibiting any of these movements will eliminate this degree of freedom."<< std::endl;

}


bool TLSConsCheck::getResultStatus() {
    return resultStatus;
}
set<set<int>> TLSConsCheck::identifyConnectedNullspaceGroups() {
    set<set<int>> connectedGroups;
    for (auto obj : nullspaceObjects) {
        // only search for nontrivial objects
        set<int> group = getConnectedNullspaceGroup(obj);
        connectedGroups.insert(group);
    }
    return connectedGroups;
}

set<int> TLSConsCheck::getConnectedNullspaceGroup(int i) {
    // get the connected group of object i
    set<int> group({ i });
    set<int> newGroup;
    set<int> added({ i });
    while (added.size() > 0) {
        newGroup = group;
        for (auto object : added) {
            // add the neighbors of objs that were added in the previous round that are in the kernel
            for (auto toBeInserted : nullspaceNeighbors[object]) {
                newGroup.insert(toBeInserted);
            }
        }
        //check which points have been added
        added.clear();
        set_difference(newGroup.begin(), newGroup.end(), group.begin(), group.end(), inserter(added, added.begin()));
        group = newGroup;
    }
    return group;
}

void TLSConsCheck::initialize(const TLGCData& data)
{
    for (auto object : data.getPoints())
    {
        addObject(object, "Point");
    }
    for (auto object : data.getLines())
    {
        addObject(object, "Line");
    }
    for (auto object : data.getAngles())
    {
        addObject(object, "Angle");
    }
    for (auto object : data.getPlanes())
    {
        addObject(object, "Plane");
    }
    for (auto object : data.getLength())
    {
        addObject(object, "Length");
    }
    // now the unknowns associated to transformations.. (as in TLSResultsMatricesExtractor::extractTransformationParams)
    // as there is no "adjustable transformation collection", we have to iterate over the tree and get them on our own.
    for (auto it(data.getTree().begin()); it != data.getTree().end(); ++it) {
        auto trafo(it.node->data.get()->frame);
        addObject(trafo, "Transformation");
    }

    // compute neighbors
    // initialize neighbors
    for (auto object : objectNames) {
        set<int> empty;
        neighbors.push_back(empty);
        nullspaceNeighbors.push_back(empty);
    }
    for (int i = 0; i < firstDgnMatrix.rows(); i++) {
		vector<int> colIndices = getIndicesOfRow(firstDgnMatrix, i);
		set<int> contributingToRow = objectsFromIndices(colIndices);
        //set<int> contributingToRow = contributingObjects(firstDgnMatrix.row(i).transpose());
        for (auto object : contributingToRow) {
            for (auto objectToInsert : contributingToRow) {
                neighbors[object].insert(objectToInsert);
            }
        }
    }
    nullspace = computeNullspace();

    // compute nullspaceNeighbors (only allow connections to objects contributing
    // to kernel)
    nullspaceObjects = contributingObjects(nullspace);
    for (int i = 0; i < objectNames.size(); i++) {
        // nullspaceNeighbors are neighbors and contribute to kernel
        for (auto object : neighbors[i]) {
            if (nullspaceObjects.count(object) > 0) {
                nullspaceNeighbors[i].insert(object);
            }
        }
    }
}
void TLSConsCheck::computeMasterJacobianAndInsensitiveRootDirections(const TLGCData &data)
{
	masterJacobian = getMasterJacobian(data);
	insensitiveRootDirections = getInsensitiveDirectionsInRoot(data);
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

TDenseMatrix TLSConsCheck::computeNullspace()
{

    // get only the columns of the first design matrix that correspond to the test_set of objects
    TDenseMatrix firstDense = firstDgnMatrix;
    //TDenseMatrix firstWithTestIndices = firstDense(Eigen::indexing::all, testIndices);
    // compute kernel representation of this matrix
    // with pullpivlu
    Eigen::FullPivLU<TDenseMatrix> lu(firstDense);
    lu.setThreshold(pivotThreshold);
    TDenseMatrix nullspace= lu.kernel();
    if (nullspace.isZero()) {
        // if only the zero matrix is returned by eigen, the nullspace has dimension 0
//        kernWrtIndices.conservativeResize(firstDense.cols(), 0);
        nullspace.conservativeResize(firstDense.cols(), 0);
    }
    //TDenseMatrix backProjectedKernel = TDenseMatrix::Zero(firstDense.rows(), kernWrtIndices.cols());

    //backProjectedKernel(testIndices, Eigen::indexing::all) = kernWrtIndices;

    return nullspace;
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

set<int> TLSConsCheck::contributingObjects(TDenseMatrix M) {
    // get objects such that M has at least one column that is nonzero on that object-row
    set<int> contributing;
    for (int i = 0; i < objectNames.size(); i++) {
        TDenseMatrix aux;
        aux = M(objectIndices[i], Eigen::indexing::all);
        if (!aux.isZero()) {
            contributing.insert(i);
        }
    }
    return contributing;
}


pair<set<int>, int> TLSConsCheck::externalConnections(set<int> group) {
    // for a given group of objects compute the objects in the complement that are connected to this group
    set<int> externalConnectedObjects;
    int nConnections = 0;

    set<int> complementOfGroup;
    for (int i = 0; i < objectNames.size(); i++) {
        if (group.count(i) == 0) {
            //i is not in group
            complementOfGroup.insert(i);
        }
    }
    vector<int> internalIndices = indicesFromSet(group);
    vector<int> externalIndices = indicesFromSet(complementOfGroup);


    for (int row = 0; row < firstDgnMatrix.rows(); row++) {
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

        if (dependsOnGroup && dependsOnComplement) {
            // the measurement involves internal and external objects
            nConnections++;
            for (auto object : contributing) {
                if (group.count(object) == 0) {
                    externalConnectedObjects.insert(object);
                }
            }
        }
    }

    return { externalConnectedObjects, nConnections };
}

