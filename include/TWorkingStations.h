// 
// TWorkingStations.h : Header File
//

//

#ifndef SU_WORKING_STATION
#define SU_WORKING_STATION

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include "TheodoliteStation.h"
#include "EDMStation.h"
#include "GyroscopeStation.h"
#include "LevelStation.h"

#include <list>
using namespace std;

//!Class storing all the instrument stations
class TWorkingStations
{
public:

	const list<TheodoliteStation*>& getTheodoliteStations() const { return theodoliteStations; }

	void addTheodoliteStation(TheodoliteStation* s) { theodoliteStations.push_back(s); }

	const list<EDMStation*>& getEDMStations() const { return edmStations; }

	void addEDMStation(EDMStation* s) { edmStations.push_back(s); }

	const list<GyroscopeStation*>& getGyroscopeStations() const { return gyroscopeStations; }

	void addGyroscopeStation(GyroscopeStation* s) { gyroscopeStations.push_back(s); }

	const list<LevelStation*>& getLevelStations() const { return levelStations; }

	void addLevelStation(LevelStation* s) { levelStations.push_back(s); }

private:

	list<TheodoliteStation*> theodoliteStations;
	list<EDMStation*> edmStations;
	list<GyroscopeStation*> gyroscopeStations;
	list<LevelStation*> levelStations;

};

#endif
