// TheodoliteStation.h


#ifndef SU_THEODOLITE_STATION
#define SU_THEODOLITE_STATION

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Station.h"
#include "TTheodolite.h"
#include "TheodoliteStationROM.h"

#include <list>
using namespace std;

class TheodoliteStation : public Station
{

public:

	TheodoliteStation(const TSpatialPoint* sp, const TTheodolite* instr, const TLength* ih,
		const TLength* ihs, bool ihVariable, const TheodoliteTarget* trgt,
		const TLength* ics, bool r3D, int ihun) : Station(sp, ih, ihs, ihVariable, ics, ihun)
	{
		rot3D = r3D;
		instrument = instr;
		defaultTarget = trgt;
	}

	~TheodoliteStation()
	{
		list<TheodoliteStationROM*>::iterator iter = theodoliteStationROMs.begin();

		while (iter != theodoliteStationROMs.end())
		{
			delete *iter;
			iter++;
		}
	}

	const TTheodolite* getInstrument() const { return instrument; }

	bool getRot3D() const { return rot3D; }

	const TheodoliteTarget* getTarget() const { return defaultTarget; }

	const list<TheodoliteStationROM*>& getTheodoliteStationROMs() const { return theodoliteStationROMs; }

	void addTheodoliteStationROM(TheodoliteStationROM* rom) { theodoliteStationROMs.push_back(rom); }

private:

	bool rot3D;

	const TTheodolite* instrument;

	const TheodoliteTarget* defaultTarget;

	list<TheodoliteStationROM*> theodoliteStationROMs;
};

#endif //SU_THEODOLITE_STATION
