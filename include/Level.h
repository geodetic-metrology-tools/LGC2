// 
// Level.h : header file
//
// Class for an Level
//
//  Patterns:
//  
//  
//  Copyright 2002 CERN EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////


#ifndef SU_LEVEL
#define SU_LEVEL

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include <hash_map>
using namespace stdext;

#include "TAFreeInstrument.h"
#include "Staff.h"
#include "TLength.h"

//! Class for an EDM station
class Level : public TAFreeInstrument
{
public:

	Level(const string& id) : TAFreeInstrument(id, NULL) { }

	~Level();

	const Staff* getDefaultStaff() const { return defaultStaff; }
	const Staff* getStaffNamed(const string& staffName) const { return staffs.find(staffName)->second; }

	void addStaff(const Staff* staff, bool def);

private:

	const Staff* defaultStaff;

	hash_map<string, const Staff*> staffs; // target name to target pairs

};

#endif









//#include "TUnknownLength.h"
//#include "UEOIndices.h"
//#include "TZenithDistROM.h"
	
	//virtual void							sendDisactivatedToChildren();
	//virtual void							sendActivatedToChildren();

//	TUnknownLength					fInstrumentHeight;



	//inherited from TVParentMeasTreeNode
	//virtual void							childReadyToActivate(int measCount, int eqCount, int unkCount);
	//virtual void							activateReadyChildren();
