/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TLS_CONSI_H
#define TLS_CONSI_H

#include <TLGCData.h>
#include <TLGCPointConstraintGroup.h>
#include <TVAdjustableObject.h>

#include "TSparseMatrix.h"

class TLSConsCheck
{
public:
	// constructor
	TLSConsCheck(TLGCData &data, const TLSInputMatrices &inputMtr);
	bool getResultStatus();
	// write the error message with the logger methods
	void generateErrorMessage();
	// get the connected nullspace groups
	const std::set<std::set<int>> &getConnectedNullspaceGroups() const { return connectedNullspaceGroups; }
	// try to compute a list of point group constraints that can make the problem computable.
	bool computeNecessaryLIBRConstraints(std::list<TLGCPointConstraintGroup> &proposedPointGroupConstraints);

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
	std::vector<std::string> objectNames;
	// vector of object types (line, angle, trafo etc.)
	std::vector<std::string> objectTypes;
	// vector of vector of associated variable indices
	std::vector<std::vector<int>> objectIndices;
	// vector of sets of neighbors restricted to objects that contribute to Nullspace of first design matrix
	std::vector<std::set<int>> nullspaceNeighbors;
	// the different connected problematic groups
	std::set<std::set<int>> connectedNullspaceGroups;
	// matrix representation of Kernel of first design matrix & constraints
	TDenseMatrix nullspace;
	// objects contributing to Nullspace
	std::set<int> nullspaceObjects;
	// maps column index (unknown index) to object id
	std::vector<int> colToObject;

	// convert set of object-indices to variable indices
	std::vector<int> indicesFromSet(const std::set<int> &objectSet);
	// get set of object indices from variable indices
	std::set<int> objectsFromIndices(const std::vector<int> &x);
	void initialize();
	void addObject(TVAdjustableObject &object, const std::string &objectType);
	// get  points in group
	std::set<int> getPoints(const std::set<int> &group);
	// compute full Nullspace representation
	TDenseMatrix computeNullspace();
	// compute Nullspace representation with only certain objects contributing
	std::vector<TDenseMatrix> computeKernelWrtObjectSet(const std::set<int> &allowedObjects);
	// get set of objects that contribute to matrix
	std::set<int> contributingObjects(const TDenseMatrix &M);
	// compute all connected object groups that contribute to the Nullspace
	std::set<std::set<int>> identifyConnectedNullspaceGroups();
	// get set of object indices from the complement of a group that are connected to this group
	std::pair<std::set<int>, int> externalConnections(const std::set<int> &group);
	// write a warning message
	void generateGroupWarning(const std::set<int> &component, const std::vector<TDenseMatrix> &kernGroupBaseVectors, const int groupNumber);
	// computing the jacobian of the virtual master helmert trafo acting on the point in root coordinates, will be a 3x7 matrix
	TDenseMatrix getMasterDirections(const std::string &pointName);
	// given a point and certain columns of the Nullspace, compute the resulting movement in Root considering all the contributions from the point in subframe and the helmert parameters from the chain to root
	TDenseMatrix getAmbiguousDirectionsInRoot(const std::string &pointName, const TDenseMatrix &nullspaceBlock);
	// return column indices from sparsity pattern of a row of a sparse  matrix
	std::vector<int> getIndicesOfRow(const Eigen::SparseMatrix<double, Eigen::RowMajor> &M, int rowNumber);
	std::vector<std::string> involvedHelmertComponents(TVector linComb);
	// for a group check for each direction if it can be interpreted as movements from a helmert transformation in ROOT
	std::vector<std::vector<std::string>> interpreteGroupDirectionsAsHelmertMovements(const std::set<int> &pointsInGroup, const std::vector<TDenseMatrix> &kernGroupBaseVectors);
	//  return a set of points whose root coordinates are affected by the nullspaceDirection + the exact direction in which they move in root + their position in root.
	std::tuple<std::set<std::string>, Eigen::VectorXd, Eigen::VectorXd> getAffectedPointsAndRootMovements(const std::set<int> &group, const Eigen::VectorXd &nullspaceVector);
	void plotTransformationMessage(std::vector<std::vector<std::string>>);
	// having n 3d points with a vector of their concatenated positions and a vector of concatenated directions (or matrix if there are several directions)
	//  try to find a point such that the directions can be interpreted as rotation around that point
	//  returns true if a rotation axis/center was found otherwise false
	bool findRotationAxis(const Eigen::VectorXd &pos, const Eigen::MatrixXd &directions);
	// method for interpreting nullspace directions of a group as translations/rotations
	// the method first tries to isolate pure translations and then tries to interprete the remaining orthogonasl complement of the nullspace as rotations.
	// it will also try to find the corresponding rotation axis
	bool findDirectionsToBlock(std::array<bool, 7> &chosenConstraints, const Eigen::MatrixXd &helmertMovements, const Eigen::VectorXd &pointPositions, const Eigen::MatrixXd &nullspaceDirections);
	// input: matrix with columns representing ambiguous directions in terms of helmert directions. output: constraint signature blocking all of these directions
	std::array<bool, 7> whatToBlock(const Eigen::MatrixXd &mat);
};

#endif
