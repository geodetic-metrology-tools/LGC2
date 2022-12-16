/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#include <Logger.hpp>
#include <TFileLogger.h>
#include <iostream>
#include <algorithm>
#include <list>
#include <TAdjustableHelmertTransformation.h>
#include <TLGCData.h>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER


#ifndef _LGC_FRAMECONSTRAINTGROUP_H
#define _LGC_FRAMECONSTRAINTGROUP_H
#if USE_SERIALIZER
class LGCFrameConstraintGroup: public Serializable
#else
class LGCFrameConstraintGroup
#endif // USE_SERIALIZER
{
public:
	//constructor
	LGCFrameConstraintGroup(){};
	//destructor
	~LGCFrameConstraintGroup(){};

	void setGroupName(std::string name) { groupName = name; };
	std::string getGroupName() { return groupName; };
	void addFrameToGroup(std::string frame, const TLGCData& projData);
	int getConstraintDimension() { return constraintDim; };
	int getFirstCIndex() { return firstCIndex; };
	void setFirstCIndex(int j) { firstCIndex = j; };
	std::list<std::string> slaves;
	// checks if a frame is part of this group
	bool isPartOfGroup(std::string frame);

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif


private:
	std::string groupName{""};
	int firstCIndex{-1};
	int constraintDim{0};

};

#endif

