// Station.h


#ifndef SU_STATION
#define SU_STATION

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Target.h"
#include "TSpatialPoint.h"
#include "TAFreeInstrument.h"
#include "TLength.h"

class Station {

public:

	const TSpatialPoint* getStationedPoint() const { return stationedPoint; }
	
	const TLength* getInstrumentHeight() const { return instrumentHeight; }
	const TLength* getInstrumentHeightSigma() const { return instrumentHeightSigma; }

	bool getInstrumentHeightVariable() const { return instrumentHeightVariable; }

	const TLength* getInstrumentCenteringSigma() const { return instrumentCenteringSigma; }

	virtual ~Station()
	{
		delete instrumentHeight;
		delete instrumentHeightSigma;
		delete instrumentCenteringSigma;
	}

	int getInstrumentHeightUnknownNumber() const { return instrumentHeightUnknownNumber; }

protected:

	Station(const TSpatialPoint* sp, const TLength* ih, const TLength* ihs, bool ihVariable, const TLength* ics, int ihnumber)
	{
		stationedPoint = sp;
		instrumentHeight = ih;
		instrumentHeightVariable = ihVariable;
		instrumentCenteringSigma = ics;
		instrumentHeightSigma = ihs;
		instrumentHeightUnknownNumber = ihnumber;
	}

	const TSpatialPoint* stationedPoint;
	const TLength* instrumentHeight;
	const TLength* instrumentHeightSigma;
	bool instrumentHeightVariable;
	const TLength* instrumentCenteringSigma;

	int instrumentHeightUnknownNumber;

};

#endif //SU_STATION
