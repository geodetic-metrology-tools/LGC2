// 
// TTheodolite.h : header file
//
// Class for a theodolite instrument station
//
//  Patterns:
//  
//  
//  Copyright 2002 CERN EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////


#ifndef SU_THEODOLITE
#define SU_THEODOLITE

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include <hash_map>
using namespace stdext;

#include "TAInstrument.h"
#include "TheodoliteTarget.h"
#include "TLength.h"
#include "TAngleConstants.h"

//! Class for a theodolite instrument station
class TTheodolite : public TAInstrument
{
public:

	TTheodolite(const string& id, const TLength* ih, const TLength* sih, const TLength* sic, const TAngleConstants* ca);

	~TTheodolite();
	
	const TAngleConstants* getAngleConst() const { return angleConstant; }

	const TheodoliteTarget* getDefaultTarget() const { return defaultTarget; }
	const TheodoliteTarget* getTargetNamed(const string& targetName) const { return targets.find(targetName)->second; }

	void addTarget(const TheodoliteTarget* target, bool def);

private:

	const TheodoliteTarget* defaultTarget;

	const TAngleConstants* angleConstant; // angle constant for horizontal angle measurements

	hash_map<string, const TheodoliteTarget*> targets; // target name to target pairs

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
