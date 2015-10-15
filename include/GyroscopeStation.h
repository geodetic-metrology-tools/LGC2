// GyroscopeStation.h


#ifndef SU_GYRO_STATION
#define SU_GYRO_STATION

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Station.h"
#include "TAngleConstants.h"
#include "ObservationROM.h"
#include "Gyroscope.h"


#include <list>
using namespace std;

class GyroscopeStation : public Station
{
public:

	GyroscopeStation(const TSpatialPoint* sp, const Gyroscope* g, const TLength* ics, const TAngleConstants* consts, const TGyroOrientationROM* r)
		: Station(sp, NULL, NULL, false, ics, -1)
	{
		gyroscope = g;
		rom = r;
		constant = consts;
	}

	~GyroscopeStation()
	{
		delete rom;
		delete constant;
	}
	
	const Gyroscope* getGyroscope() const { return gyroscope; }

	const TGyroOrientationROM* getGyroscopeROM() const { return rom; }

	const TAngleConstants* getAngleConstant() const { return constant; }

private:

	const Gyroscope* gyroscope;

	const TGyroOrientationROM* rom;

	const TAngleConstants* constant;
};

#endif //SU_GYRO_STATION
