/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/
#ifndef _LGC_POINTCONSTRAINTGROUP_H
#	define _LGC_POINTCONSTRAINTGROUP_H
#include <algorithm>
#include <iostream>
#include <list>

#include <Logger.hpp>
#include <TAdjustableHelmertTransformation.h>
#include <TFileLogger.h>
#include <TLGCData.h>
#include <TLOR2LOR.h>
#include <Logger.hpp>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER



#	if USE_SERIALIZER


class LGCPointConstraintGroup : public Serializable
#	else
class LGCPointConstraintGroup
#	endif // USE_SERIALIZER
{
public:
	//using constraintSignature = std::array<bool, 7>;
	// constructor
	LGCPointConstraintGroup(const TLGCData &projData) : data(projData){};
	// destructor
	~LGCPointConstraintGroup(){};
	// set of points that will be constrained
	void setAffectedPoints(std::set<std::string> affectedPoints);
	// convenience method which adds all points to the affected points, equivalent to call setAffectedPoints with a set containing all points
	void allPointsAreAffected();
	std::set<std::string> getAffectedPoints() const { return fAffectedPoints; };
	// set of imposed constraints , subset of (TX,TY,TZ,RX,RY,RZ,SCL) meaning center of Gravity constraint of x,y,z coordinate, momentum constraint x,y,z rotation, scale constraint
	void setConstraintSignature(std::array<bool, 7> usedConstraints);
	const std::array<bool, 7> getConstraintSignature() { return fConstraints; };
	const int getConstraintDimension() { return constraintDim; }
	int getFirstCIndex() { return firstCIndex; };
	void setFirstCIndex(int j) { firstCIndex = j; };
	Eigen::Vector3d getProvRootPos(std::string pointName) const; 
	Eigen::Vector3d getProvCOG() const { return provCOG; };
	double getProvScale() const { return provScale; };
	void plotGroupData();
	//
//	void setGroupName(std::string name) { groupName = name; };
//	std::string getGroupName() { return groupName; };
//	void addFrameToGroup(std::string frame, const TLGCData &projData);
//	int getConstraintDimension() { return constraintDim; };
//	std::list<std::string> slaves;
//	// checks if a frame is part of this group
//	bool isPartOfGroup(std::string frame);

#	if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#	endif

private:
	//! Reference to a TLGCData
	const TLGCData& data;
	// the points of the group
	std::set<std::string> fAffectedPoints;
	// the coordinates of the points transformed to root (using provisional values)
	std::map<std::string, Eigen::Vector3d> fProvPosInRoot;
	std::array<bool, 7> fConstraints;
	int firstCIndex{-1};
	int constraintDim{0};
	//the center of gravity in root coordinates with provisional values.
	Eigen::Vector3d provCOG;
	double provScale = 0;
	//void setCOG();
};

#endif

