/*
© Copyright CERN 2000-2022. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_CONSI_H
#define TLS_CONSI_H

#include <TVAdjustableObject.h>
#include "TSparseMatrix.h"

using namespace std;

class TLSConsCheck
{
public:
    // constructor
    TLSConsCheck(TLGCData& data, const TLSInputMatrices& inputMtr);
    bool getResultStatus();

private:
    // true if check is passed (no problem detected), false otherwise
    bool resultStatus = false;
    // threshold for rank and kernel computations
    // used for Eigen LU decomposition
    double pivotThreshold = 1e-6;
    // the first design matrix here is the first design matrix of the measurements and the first design matrix of the constraints (if present)
    Eigen::SparseMatrix<double,Eigen::RowMajor> firstDgnMatrix;
    // vector of object names
    vector<string> objectNames;
    // vector of object types (line, angle, trafo etc.)
    vector<string> objectTypes;
    // vector of vector of associated variable indices
    vector<vector<int>> objectIndices;
    // vector of sets of object neighbors (connected via measurement or constraint)
    vector<set<int>> neighbors;
    // vector of sets of neighbors restricted to objects that contribute to Nullspace of first design matrix
    vector<set<int>> nullspaceNeighbors;
    // matrix representation of Kernel of first design matrix & constraints
    TDenseMatrix nullspace;
    // objects contributing to Nullspace
    set<int> nullspaceObjects;
    // Master Jacobian
	TDenseMatrix masterJacobian;
	TDenseMatrix insensitiveRootDirections;

    // convert set of object-indices to variable indices
    vector<int> indicesFromSet(set<int> objectSet);
    // get set of object indices from variable indices
    set<int> objectsFromIndices(vector<int> x);
    void initialize(const TLGCData& data);
    // compute sensitivities of a helmert transformation acting on all points (MasterJacbobian, in root coordinates)
    // and transform Nullspace to directions in root coordinates (InsensitiveRootDirections)
	void computeMasterJacobianAndInsensitiveRootDirections(const TLGCData& data);
    void addObject(TVAdjustableObject& object, string objectType);
    // get  points in group
	set<int> getPoints(set<int> group);
    // compute Nullspace representation for a given set of objects
    TDenseMatrix computeNullspace();
    // get set of objects that contribute to matrix
    set<int> contributingObjects(TDenseMatrix);
    // compute all connected object groups that contribute to the Nullspace
    set<set<int>> identifyConnectedNullspaceGroups();
    // get set of object indices of the Nullspace connected to certain object in the Nullspace
    set<int> getConnectedNullspaceGroup(int);
    // get set of object indices from the complement of a group that are connected to this group
    pair<set<int>, int> externalConnections(set<int> group);
    // write a warning message
    void generateGroupWarning(const set<int> component, const vector<TDenseMatrix> kernGroupBaseVectors, const int groupNumber);
    // compute the Jacobian of a master helmert transformation acting on all points transformed to the root coordinates.
	TDenseMatrix getMasterJacobian(const TLGCData& data);
    // transform the nullspace elements to point movements in the root frame
	TDenseMatrix getInsensitiveDirectionsInRoot(const TLGCData &data);
    // checking the coefficients nneded to linear combine a insensitive direction from master frame movements
	void whichConstraintsDoWeNeed(TVector combi);
    // return column indices from sparsity pattern of a row of a sparse  matrix
    vector<int> getIndicesOfRow(const Eigen::SparseMatrix<double, Eigen::RowMajor> &M, int rowNumber);
	vector<string> involvedHelmertComponents(TVector linComb);
};

#endif

