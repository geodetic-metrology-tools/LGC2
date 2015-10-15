// 
// EDM.h : header file
//
// Class for an EDM
//
//  Patterns:
//  
//  
//  Copyright 2002 CERN EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////


#ifndef SU_EDM
#define SU_EDM

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include <hash_map>
using namespace stdext;

#include "TAInstrument.h"
#include "EDMTarget.h"
#include "TLength.h"

//! Class for an EDM station
class EDM : public TAInstrument
{
public:

	EDM(const string& id, const TLength* ih, const TLength* sih, const TLength* sic);

	~EDM();

	const EDMTarget* getDefaultTarget() const { return defaultTarget; }
	const EDMTarget* getTargetNamed(const string& targetName) const { return targets.find(targetName)->second; }

	void addTarget(const EDMTarget* target, bool def);

private:

	const EDMTarget* defaultTarget;

	hash_map<string, const EDMTarget*> targets; // target name to target pairs

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
