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
	// the different connected problematic groups
	std::set<std::set<int>> connectedNullspaceGroups;
	// matrix representation of Kernel of first design matrix & constraints
	TDenseMatrix nullspace;
	// maps column index (unknown index) to object id, -1 for columns without object
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
	// movements in Root that the kernel directions of a group cause on a set of points
	struct RootMovementAnalysis
	{
		// dimension of the movements in Root; the remaining kernel directions do not move any point
		int dimRoot = 0;
		// dimension of the part of the movements that are Helmert movements of the points
		int dimHelmert = 0;
		// dimension of the part of the movements that are pure translations
		int dimTranslation = 0;
		// orthonormal basis of the movements in Root, 3 rows per point
		TDenseMatrix rootBasis;
		// linearised LIBR constraints TX, TY, TZ, RX, RY, RZ, SCL (barycentre, momentum, scale) applied to rootBasis, 7 rows
		TDenseMatrix librResponse;
		// Helmert coefficients TX, TY, TZ, RX, RY, RZ, SCL of rootBasis (rotations and scale about the barycentre), 7 rows
		TDenseMatrix helmertCoefficients;
		// Helmert components of the movement of each kernel column, "stationary" or "notInterpretable"
		std::vector<std::string> labels;
	};
	// write a warning message
	void generateGroupWarning(const std::set<int> &component, const TDenseMatrix &kernel, const int groupNumber);
	// Helmert movements of the points in Root (3 rows per point, columns TX, TY, TZ, RX, RY, RZ, SCL), rotations and scale about the barycentre
	TDenseMatrix getCentredHelmertMovements(const std::set<std::string> &points);
	// given a point and certain columns of the Nullspace, compute the resulting movement in Root considering all the contributions from the point in subframe and the helmert parameters from the chain to root
	TDenseMatrix getAmbiguousDirectionsInRoot(const std::string &pointName, const TDenseMatrix &nullspaceBlock);
	// points of the group and all points whose Root coordinates move with the kernel
	std::set<std::string> getAffectedPoints(const std::set<int> &group, const TDenseMatrix &kernel);
	// decompose the movements in Root of the kernel directions into Helmert movements and the rest
	RootMovementAnalysis analyseRootMovements(const std::set<std::string> &points, const TDenseMatrix &kernel);
	// choose the LIBR components whose rows of librResponse block the movements, with dimTranslation barycentre constraints and the largest
	// Helmert coefficients; false if they cannot be blocked
	bool chooseLIBRComponents(const TDenseMatrix &librResponse, const TDenseMatrix &helmertCoefficients, int dimTranslation, std::array<bool, 7> &chosenConstraints);
};

#endif
