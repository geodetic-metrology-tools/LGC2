// LevelStation.h


#ifndef SU_LEVEL_STATION
#define SU_LEVEL_STATION

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Station.h"
#include "Level.h"
#include "Staff.h"
#include "ObservationROM.h"
#include "LevelMeasurement.h"
#include "THorizontalDistMeas.h"

#include <list>
using namespace std;

class LevelStation : public Station
{

public:

	LevelStation(const TSpatialPoint* sp, const Level* instr, const ObservationROM<LevelMeasurement>* r,
		const ObservationROM<THorizontalDistMeas<Staff> >* hdr)
		: Station(sp, NULL, NULL, false, NULL, -1)
	{
		instrument = instr;
		verticalDistanceROM = r;
		horizontalDistanceROM = hdr;
	}

	~LevelStation()
	{
		delete verticalDistanceROM;
		delete horizontalDistanceROM;
	}

	const Level* getInstrument() const { return instrument; }

	const ObservationROM<LevelMeasurement>* getVerticalDistanceROM() const { return verticalDistanceROM; }

	const ObservationROM<THorizontalDistMeas<Staff> >* getHorizontalDistanceROM() const { return horizontalDistanceROM; }

private:

	const Level* instrument;

	const ObservationROM<LevelMeasurement>* verticalDistanceROM;
	const ObservationROM<THorizontalDistMeas<Staff> >* horizontalDistanceROM;
};

#endif //SU_LEVEL_STATION
