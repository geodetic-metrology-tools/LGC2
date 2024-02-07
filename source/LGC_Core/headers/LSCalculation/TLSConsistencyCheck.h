/*
© Copyright CERN 2000-2023. All rights reserved. This software is released under a CERN proprietary software license.
Any permission to use it shall be granted in writing. Request shall be addressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_CONSI_H
#define TLS_CONSI_H

#include <TLGCData.h>
#include <TVAdjustableObject.h>
#include <LGCPointConstraintGroup.h>

#include "TSparseMatrix.h"

using namespace std;

class TLSConsCheck
{
public:
	// constructor
	TLSConsCheck(TLGCData &data, const TLSInputMatrices &inputMtr);
	bool getResultStatus();
	// write the error message with the logger methods
	void generateErrorMessage();
	// get set of object indices of the Nullspace connected to certain object in the Nullspace
	set<int> getConnectedNullspaceGroup(int);
	// compute a list of point group constraints that can make the problem computable.
	bool computeNecessaryLIBRConstraints(std::list<LGCPointConstraintGroup> &proposedPointGroupConstraints);
	std::array<bool, 7> whatToBlock(Eigen::MatrixXd mat);
	// compute a representation of the intersection of the spans of columns of matrix A and matrix B
	Eigen::MatrixXd intersect(Eigen::MatrixXd A, Eigen::MatrixXd B);
	bool isSubspace(const Eigen::MatrixXd &A, const Eigen::MatrixXd &B);
	// U representing a subspace of V. returns the orthogonal complement of U in V
	Eigen::MatrixXd orthogonalComplement(const Eigen::MatrixXd &U, const Eigen::MatrixXd &V);

private:
	TLGCData &projData;
	// true if check is passed (no problem detected), false otherwise
	bool resultStatus = false;
	// threshold for rank and kernel computations
	// used for Eigen decomposition
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
	// the different connected problematic groups
	set<set<int>> connectedNullspaceGroups;
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
	// for a group check for each direction if it can be interpreted as movements from a helmert transformation in ROOT
	vector<vector<string>> interpreteGroupDirectionsAsHelmertMovements(set<int> pointsInGroup, const vector<TDenseMatrix> &kernGroupBaseVectors);
	//std::unordered_map<std::string, Eigen::Vector3d> interpreteNullSpaceDirectionAsPointMovementInRoot(Eigen::VectorXd nullspaceVector);
	// return a set of points whose root coordinates are affected by the nullspaceDirection + the exact direction in which they move in root + their position in root.
	std::tuple<std::set<std::string>, Eigen::VectorXd, Eigen::VectorXd> getAffectedPointsAndRootMovements(std::set<int> group, const Eigen::VectorXd &nullspaceVector);
	void plotTransformationMessage(vector<vector<string>>);
	// having n 3d points with a vector of their concatenated positions and a vector of concatenated directions (or matrix if there are several directions)
	//  try to find a point such that the directions can be interpreted as rotation around that point
	//  returns true if a rotation axis/center was found otherwise false
	bool findRotationCenter(const Eigen::VectorXd &pos, const Eigen::MatrixXd &directions);
	// method for interpreting nullspace directions of a group as translations/rotations
	// the method first tries to isolate pure translations and then tries to interprete the remaining orthogonasl complement of the nullspace as rotations.
	// it will also try to find the corresponding rotation axis
	bool findDirectionsToBlock(std::array<bool, 7>& chosenConstraints, const Eigen::MatrixXd &helmertMovements, const Eigen::VectorXd &pointPositions, const Eigen::MatrixXd &nullspaceDirections);
};

#endif
