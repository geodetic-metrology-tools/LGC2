// TVerticalDistMeasurement.cpp
//
/** Class for a vertical distance measurement */
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
//#include "TWorkingStations.h"
//#include "TVerticalDistROM.h"
#include "TVerticalDistMeasurement.h"
////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////
// constructor setting the target and reference names, the measured dist and its precision
//////////////////////////////////////////////////////////////////////////////////////////
TVerticalDistMeasurement::TVerticalDistMeasurement(int obsID, const TSpatialPoint* tg1, const TSpatialPoint* tg2,
		const TLength* obsDist, const TLength* fsSigma, const TLength* fsPpmE, const TLength* sndSigma, const TLength* sndPpmE,
		const TLength* fsTgH, const TLength* fsTgHS, const TLength* sndTgH, const TLength* sndTgHS,
		const Staff* fS, const Staff* sS)
		: TAPointMeasurement(obsID, tg1)
{
	secondTargetPoint = tg2;
	observedValue = obsDist;

	firstStaffSigma = fsSigma;
	firstStaffPpm = fsPpmE;
	secondStaffSigma = sndSigma;
	secondStaffPpm = sndPpmE;
	
	firstTargetHeight = fsTgH;
	firstTargetHeightSigma = fsTgHS;
	
	secondTargetHeight = sndTgH;
	secondTargetHeightSigma = sndTgHS;

	firstStaff = fS;
	secondStaff = sS;
}

/////////////
// Destructor
/////////////
TVerticalDistMeasurement::~TVerticalDistMeasurement()
{
	delete observedValue;
	delete firstStaffSigma;
	delete firstStaffPpm;
	delete secondStaffSigma;
	delete secondStaffPpm;
	delete firstTargetHeight;
	delete firstTargetHeightSigma;
	delete secondTargetHeight;
	delete secondTargetHeightSigma;
}
