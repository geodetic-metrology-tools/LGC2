// EDMStation.h


#ifndef SU_EDM_STATION
#define SU_EDM_STATION

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Station.h"
#include "EDM.h"
#include "EDMTarget.h"
#include "ObservationROM.h"

#include <list>
using namespace std;

class EDMStation : public Station
{

public:

	EDMStation(const TSpatialPoint* sp, const EDM* instr, const TLength* ih, const TLength* ihs, bool ihVariable, const EDMTarget* trgt,
		const TLength* ics, const EDMSpatialDistanceROM* r, int ihun) : Station(sp, ih, ihs, ihVariable, ics, ihun)
	{
		instrument = instr;
		defaultTarget = trgt;
		rom = r;
	}

	~EDMStation()
	{
		delete rom;
	}

	const EDM* getInstrument() const { return instrument; }

	const EDMTarget* getTarget() const { return defaultTarget; }

	const EDMSpatialDistanceROM* getSpatialDistanceROM() const { return rom; }

private:

	const EDM* instrument;

	const EDMTarget* defaultTarget;

	const EDMSpatialDistanceROM* rom;
};

#endif //SU_EDM_STATION
