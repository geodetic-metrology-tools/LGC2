/*
 * SPDX-FileCopyrightText: 2025 CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <algorithm>
#include <iostream>
#include <list>

#include <Logger.hpp>
#include <TAdjustableHelmertTransformation.h>
#include <TFileLogger.h>
#include <TLGCData.h>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER

#ifndef _TLGC_FRAMECONSTRAINTGROUP_H
#	define _TLGC_FRAMECONSTRAINTGROUP_H
#	if USE_SERIALIZER
class TLGCFrameConstraintGroup : public Serializable
#	else
class TLGCFrameConstraintGroup
#	endif // USE_SERIALIZER
{
public:
	// constructor
	TLGCFrameConstraintGroup(){};
	// destructor
	~TLGCFrameConstraintGroup(){};

	void setGroupName(std::string name) { groupName = name; };
	std::string getGroupName() { return groupName; };
	void addFrameToGroup(std::string frame, const TLGCData &projData);
	int getConstraintDimension() { return constraintDim; };
	int getFirstCIndex() { return firstCIndex; };
	void setFirstCIndex(int j) { firstCIndex = j; };
	std::list<std::string> slaves;
	// checks if a frame is part of this group
	bool isPartOfGroup(std::string frame);

#	if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#	endif

private:
	std::string groupName{""};
	int firstCIndex{-1};
	int constraintDim{0};
};

#endif
