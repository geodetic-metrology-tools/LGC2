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
    TDenseMatrix firstDgnMatrix;
    // vector of object names
    vector<string> objectNames;
    // vector of object types (line, angle, trafo etc.)
    vector<string> objectTypes;
    // vector of vector of associated variable indices
    vector<vector<int>> objectIndices;
    // vector of sets of object neighbors (connected via measurement or constraint)
    vector<set<int>> neighbors;
    // vector of sets of neighbors restricted to objects that contribute to Nullspace of first design matrix
    vector<set<int>> kernelNeighbors;
    // matrix representation of Kernel of first design matrix & constraints
    TDenseMatrix nullspace;
    // objects contributing to Nullspace
    set<int> nullspaceObjects;

    // convert set of object-indices to variable indices
    vector<int> indicesFromSet(set<int> objectSet);
    // get set of object indices from variable indices
    set<int> objectsFromIndices(vector<int> x);
    void initialize(const TLGCData& data);
    void addObject(TVAdjustableObject& object, string objectType);
    // compute Nullspace representation for a given set of objects
    TDenseMatrix computeKernelWrtObjectSet(set<int> objectSet);
    // get set of objects that contribute to matrix
    set<int> contributingObjects(TDenseMatrix);
    // compute all connected object groups that contribute to the Nullspace
    set<set<int>> identifyConnectedKernelGroups();
    // get set of object indices of the Nullspace connected to certain object in the Nullspace
    set<int> getConnectedKernelGroup(int);
    // get set of object indices from the complement of a group that are connected to this group
    pair<set<int>, int> externalConnections(set<int> group);
    // write a warning message
    void generateGroupWarning(const set<int> component, const TDenseMatrix kernGroup, const int groupNumber);

    // compute the Jacobian of a master helmert transformation acting on all points transformed to the root coordinates.
	TDenseMatrix getMasterJacobian(const TLGCData& data);
    // transform the nullspace elements to point movements in the root frame
	TDenseMatrix getInsensitiveDirectionsInRoot(const TLGCData &data);
    // checking the coefficients nneded to linear combine a insensitive direction from master frame movements
	void whichConstraintsDoWeNeed(TVector combi);

};

#endif

