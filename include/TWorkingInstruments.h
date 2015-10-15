// 
// TWorkingInstruments.h : Header File
//

//

#ifndef SU_WORKING_INSTRUMENT
#define SU_WORKING_INSTRUMENT

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include "TTheodolite.h"
#include "Scale.h"
#include "EDM.h"
#include "Gyroscope.h"
#include "Level.h"

#include <list>
using namespace std;

#include <hash_map>
using namespace stdext;

//!Class storing all the instruments
class TWorkingInstruments
{
public:

	// TODO: some error checking whether it exists
	TTheodolite* getTheodoliteNamed(const string& name) { return theodolitesMap.find(name)->second; }

	void addTheodolite(TTheodolite* theo)
	{
		theodolitesList.push_back(theo);
		theodolitesMap[theo->getInstrumentID()] = theo;
	}
	
	Scale* getScaleNamed(const string& name) { return scalesMap.find(name)->second; }

	void addScale(Scale* scale)
	{
		scalesList.push_back(scale);
		scalesMap[scale->getInstrumentID()] = scale;
	}
	
	EDM* getEDMNamed(const string& name) { return edmsMap.find(name)->second; }

	void addEDM(EDM* edm)
	{
		edmsList.push_back(edm);
		edmsMap[edm->getInstrumentID()] = edm;
	}
	
	Gyroscope* getGyroscopeNamed(const string& name) { return gyroscopesMap.find(name)->second; }

	void addGyroscope(Gyroscope* gyro)
	{
		gyroscopesList.push_back(gyro);
		gyroscopesMap[gyro->getInstrumentID()] = gyro;
	}
	
	Level* getLevelNamed(const string& name) { return levelsMap.find(name)->second; }

	void addLevel(Level* level)
	{
		levelsList.push_back(level);
		levelsMap[level->getInstrumentID()] = level;
	}

    const list<TTheodolite*>& getTheodolitesList() const { return theodolitesList; }

    const list<Scale*>& getScalesList() const { return scalesList; }

    const list<EDM*>& getEDMsList() const { return edmsList; }

    const list<Gyroscope*>& getGyroscopesList() const { return gyroscopesList; }

    const list<Level*>& getLevelsList() const { return levelsList; }

private :

	list<TTheodolite*> theodolitesList;
	list<Scale*> scalesList;
	list<EDM*> edmsList;
	list<Gyroscope*> gyroscopesList;
	list<Level*> levelsList;

	hash_map<string, TTheodolite*> theodolitesMap;
	hash_map<string, Scale*> scalesMap;
	hash_map<string, EDM*> edmsMap;
	hash_map<string, Gyroscope*> gyroscopesMap;
	hash_map<string, Level*> levelsMap;

};

#endif
