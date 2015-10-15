// 
// Scale.h : header file
//
// Class for a scale
//
//  Patterns:
//  
//  
//  Copyright 2002 CERN EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////


#ifndef SU_SCALE
#define SU_SCALE

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include <hash_map>
using namespace stdext;

#include "TAFreeInstrument.h"
#include "TLength.h"
#include "TDistConstants.h"

//! Class for a theodolite instrument station
class Scale : public TAFreeInstrument
{
public:

	Scale(const string& id, const TLength* spDSigma, const TLength* ppmE, const TDistConstants* dc, const TLength* sdc,
		const TLength* sic, const TLength* to, const TLength* sto, int dcun);

	~Scale();
	
	const TDistConstants* getDistanceCorrection() const { return distanceConstant; }

	const TLength* getDistanceCorrectionSigma() const { return distanceConstantSigma; }

    const TLength* getSpatialDistanceSigma() const { return spatialDistanceSigma; }

    const TLength* getPPM() const { return ppm; }

    const TLength* getTargetOffset() const { return targetOffset; }

    const TLength* getTargetOffsetSigma() const { return targetOffsetSigma; }

	int getDistanceCorrectionUnknownNumber() const { return distanceCorrectionUnknownNumber; }

private:

	const TDistConstants* distanceConstant;
	const TLength* distanceConstantSigma;

	const TLength* spatialDistanceSigma;
	const TLength* ppm;
	const TLength* targetOffset;
	const TLength* targetOffsetSigma;

	int distanceCorrectionUnknownNumber;

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
