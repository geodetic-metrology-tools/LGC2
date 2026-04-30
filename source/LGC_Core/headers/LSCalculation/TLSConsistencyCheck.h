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
	bool getResultStatus() const { return resultStatus; }
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

	// objects that contribute to row of firstDgnMatrix
	std::set<int> objectsOnRow(int row) const;
	void initialize();
	void addObject(TVAdjustableObject &object, const std::string &objectType);
	// get  points in group
	std::set<int> getPoints(const std::set<int> &group);
	// compute full Nullspace representation
	TDenseMatrix computeNullspace();
	// compute Nullspace representation with only certain objects contributing (each column is a basis vector)
	TDenseMatrix computeKernelWrtObjectSet(const std::set<int> &allowedObjects);
	// compute all connected object groups that contribute to the Nullspace
	std::set<std::set<int>> identifyConnectedNullspaceGroups();
	// get set of object indices from the complement of a group that are connected to this group
	std::pair<std::set<int>, int> externalConnections(const std::set<int> &group);
	// write a warning message
	void generateGroupWarning(const std::set<int> &component, const TDenseMatrix &kernel, const int groupNumber);
	// computing the jacobian of the virtual master helmert trafo acting on the point in root coordinates, will be a 3x7 matrix
	TDenseMatrix getMasterDirections(const std::string &pointName);
	// given a point and certain columns of the Nullspace, compute the resulting movement in Root considering all the contributions from the point in subframe and the helmert parameters from the chain to root
	TDenseMatrix getAmbiguousDirectionsInRoot(const std::string &pointName, const TDenseMatrix &nullspaceBlock);
	// for each ambiguous direction of the group, classify it as stationary / a Helmert combination / not interpretable, and emit one warning row
	void reportGroupHelmertInterpretation(const std::set<int> &pointsInGroup, const TDenseMatrix &kernel);
	// return the set of points whose root coordinates are affected by nullspaceVector + their concatenated root-direction vector
	std::pair<std::set<std::string>, Eigen::VectorXd> getAffectedPointsAndRootMovements(const std::set<int> &group, const Eigen::VectorXd &nullspaceVector);
	// decompose nullspace directions into pure translations + pure rotations/scale and pick blocking constraints
	bool findDirectionsToBlock(std::array<bool, 7> &chosenConstraints, const Eigen::MatrixXd &helmertMovements, const Eigen::MatrixXd &nullspaceDirections);
	// input: matrix with columns representing ambiguous directions in terms of helmert directions. output: constraint signature blocking all of these directions
	std::array<bool, 7> whatToBlock(const Eigen::MatrixXd &mat);
};

#endif
