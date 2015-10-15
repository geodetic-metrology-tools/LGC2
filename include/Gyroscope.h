// 
// Gyroscope.h : header file
//
// Class for a Gyroscope
//
//  Patterns:
//  
//  
//  Copyright 2002 CERN EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////


#ifndef SU_GYROSCOPE
#define SU_GYROSCOPE

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include <hash_map>
using namespace stdext;

#include "TAFreeInstrument.h"
#include "TAngleConstants.h"
#include "TLength.h"

//! Class for an EDM station
class Gyroscope : public TAFreeInstrument
{
public:

	Gyroscope(const string& id, const TLength* sic, const TLength* tcs, const TAngleConstants* ac, const TAngle* as) :
		TAFreeInstrument(id, sic)
	{
		targetCenteringSigma = tcs;
		angleConstant = ac;
		angleSigma = as;
	}

	~Gyroscope()
	{
		delete targetCenteringSigma;
		delete angleConstant;
		delete angleSigma;
	}

	const TLength* getTargetCenteringSigma() const { return targetCenteringSigma; }

	const TAngleConstants* getAngleConstant() const { return angleConstant; }

	const TAngle* getAngleSigma() const { return angleSigma; }

private:

	const TLength* targetCenteringSigma;

	const TAngleConstants* angleConstant;
	const TAngle* angleSigma;
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
