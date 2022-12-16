/*
© Copyright CERN 2000-2023. All rights reserved. This software is released under a CERN proprietary software license.
Any permission to use it shall be granted in writing. Request shall be addressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_CONSI_H
#define TLS_CONSI_H

#include <TLGCData.h>
#include <TVAdjustableObject.h>

#include "TSparseMatrix.h"

using namespace std;

class TLSConsCheck
{
public:
	// constructor
	TLSConsCheck(TLGCData &data, const TLSInputMatrices &inputMtr);
	bool getResultStatus();
	// get set of object indices of the Nullspace connected to certain object in the Nullspace
	set<int> getConnectedNullspaceGroup(int);

private:
	TLGCData &projData;
	// true if check is passed (no problem detected), false otherwise
	bool resultStatus = false;
	// threshold for rank and kernel computations
	// used for Eigen LU decomposition
	double pivotThreshold = 1e-6;
	// the first design matrix here is the first design matrix of the measurements and the first design matrix of the constraints (if present)
	Eigen::SparseMatrix<double, Eigen::RowMajor> firstDgnMatrix;
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

	// convert set of object-indices to variable indices
	vector<int> indicesFromSet(set<int> objectSet);
	// get set of object indices from variable indices
	set<int> objectsFromIndices(vector<int> x);
	void initialize();
	void addObject(TVAdjustableObject &object, string objectType);
	// get  points in group
	set<int> getPoints(set<int> group);
	// compute full Nullspace representation
	TDenseMatrix computeNullspace();
	// compute Nullspace representation with only certain objects contributing
	vector<TDenseMatrix> computeKernelWrtObjectSet(set<int> allowedObjects);
	// get set of objects that contribute to matrix
	set<int> contributingObjects(TDenseMatrix);
	// compute all connected object groups that contribute to the Nullspace
	set<set<int>> identifyConnectedNullspaceGroups();
	// find neighbors of objects in the nullspace
	set<int> getNullspaceNeighbors(int object);
	// get set of object indices from the complement of a group that are connected to this group
	pair<set<int>, int> externalConnections(set<int> group);
	// write a warning message
	void generateGroupWarning(const set<int> component, const vector<TDenseMatrix> kernGroupBaseVectors, const int groupNumber);
	// computing the jacobian of the virtual master helmert trafo acting on the point in root coordinates, will be a 3x7 matrix
	TDenseMatrix getMasterDirections(string pointName);
	// given a point and certain columns of the Nullspace, compute the resulting movement in Root considering all the contributions from the point in subframe and the helmert parameters from the chain to root
	TDenseMatrix getAmbiguousDirectionsInRoot(string pointName, TDenseMatrix nullspaceBlock);
	// return column indices from sparsity pattern of a row of a sparse  matrix
	vector<int> getIndicesOfRow(const Eigen::SparseMatrix<double, Eigen::RowMajor> &M, int rowNumber);
	vector<string> involvedHelmertComponents(TVector linComb);
	void checkGroupInRoot(set<int> groupOfPoints, vector<TDenseMatrix> kernGroupBaseVectors);
};

#endif
