/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _TLGC_POINTCONSTRAINTGROUP_H
#define _TLGC_POINTCONSTRAINTGROUP_H
#include <algorithm>
#include <iostream>
#include <list>

#include <Logger.hpp>
#include <TAdjustableHelmertTransformation.h>
#include <TFileLogger.h>
#include <TLGCData.h>
#include <TLOR2LOR.h>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER

#if USE_SERIALIZER
class TLGCPointConstraintGroup : public Serializable
#else
class TLGCPointConstraintGroup
#endif // USE_SERIALIZER
{
public:
	// using constraintSignature = std::array<bool, 7>;
	// constructor
	// this will take all points as affected points
	TLGCPointConstraintGroup(const TLGCData &projData, const std::array<bool, 7> &usedConstraints);
	// this will take a specific set of points
	TLGCPointConstraintGroup(const TLGCData &projData, const std::array<bool, 7> &usedConstraints, const std::set<std::string> &affectedPoints);
	// destructor
	~TLGCPointConstraintGroup(){};
	std::set<std::string> getAffectedPoints() const { return fAffectedPoints; };
	// set of imposed constraints , subset of (TX,TY,TZ,RX,RY,RZ,SCL) meaning center of Gravity constraint of x,y,z coordinate, momentum constraint x,y,z rotation, scale constraint
	const std::array<bool, 7> getConstraintSignature() { return fConstraints; };
	// get the dimension of the constraint imposed on the group
	const int getConstraintDimension() { return constraintDim; }
	// indices for the constraints
	int getFirstCIndex() { return firstCIndex; };
	void setFirstCIndex(int j) { firstCIndex = j; };
	// provisional positions in Root, claculated at initialization based on provisional values in subframe and provisional values of any frame transformation involved
	Eigen::Vector3d getProvRootPos(std::string pointName) const;
	// center of gravity of provisional coordinates in Root
	Eigen::Vector3d getProvCOG() const { return provCOG; };
	Eigen::Vector3d computeEstCOG() const;

	// provisional scale
	double getProvScale() const { return provScale; };
	// plot information on the group and the constraints to the log2 file
	void plotGroupData();

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif

private:
	TLGCPointConstraintGroup(const TLGCData &projData) : data(projData){};
	// set of points that will be constrained
	void setAffectedPoints(const std::set<std::string> &affectedPoints);
	// convenience method which adds all points to the affected points, equivalent to call setAffectedPoints with a set containing all points
	void allPointsAreAffected();
	void setConstraintSignature(const std::array<bool, 7> &usedConstraints);
	//! Reference to a TLGCData
	const TLGCData &data;
	// the points of the group
	std::set<std::string> fAffectedPoints;
	// the coordinates of the points transformed to root (using provisional values)
	std::map<std::string, Eigen::Vector3d> fProvPosInRoot;
	// constraint signature
	std::array<bool, 7> fConstraints;
	int firstCIndex{-1};
	int constraintDim{0};
	// the center of gravity in root coordinates with provisional values.
	Eigen::Vector3d provCOG;
	double provScale = 0;
};

#endif
