
#ifndef SU_THEODOLITE_STATION_ROM
#define SU_THEODOLITE_STATION_ROM

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include "TAngleConstants.h"
#include "TAngle.h"

#include "ObservationROM.h"
#include "OffsetToTheodolitePlaneROM.h"

#include <list>
using namespace std;

class TheodoliteStationROM
{
public:

	TheodoliteStationROM(const TheodoliteTarget* trgt, const TAngleConstants* c)
	{
		defaultTarget = trgt;
		totalStationROMAngleConstant = c;
	}

	~TheodoliteStationROM()
	{
		list<const PolarROM*>::iterator iterP3D = polarROMs.begin();

		while (iterP3D != polarROMs.end())
		{
			delete *iterP3D;
			iterP3D++;
		}

		list<const HorizontalAngleROM*>::iterator iter = horizontalAngleROMs.begin();

		while (iter != horizontalAngleROMs.end())
		{
			delete *iter;
			iter++;
		}

		list<const ZenithDistanceROM*>::iterator iterZD = zenithDistanceROMs.begin();

		while (iterZD != zenithDistanceROMs.end())
		{
			delete *iterZD;
			iterZD++;
		}

		list<const SpatialDistanceROM*>::iterator iterSD = spatialDistanceROMs.begin();

		while (iterSD != spatialDistanceROMs.end())
		{
			delete *iterSD;
			iterSD++;
		}

		list<const HorizontalDistanceROM*>::iterator iterHD = horizontalDistanceROMs.begin();

		while (iterHD != horizontalDistanceROMs.end())
		{
			delete *iterHD;
			iterHD++;
		}

		list<const OffsetToTheodolitePlaneROM*>::iterator iterOTD = offsetToTheodolitePlaneROMs.begin();

		while (iterOTD != offsetToTheodolitePlaneROMs.end())
		{
			delete *iterOTD;
			iterOTD++;
		}

		delete totalStationROMAngleConstant;
	}

	const TheodoliteTarget* getDefaultTarget() const { return defaultTarget; }

	const TAngleConstants* getAngleConstant() const { return totalStationROMAngleConstant; }

	const list<const PolarROM*>& getPolarROMs() const { return polarROMs; }
	void addPolarROM(const PolarROM* rom) { polarROMs.push_back(rom); }

	const list<const HorizontalAngleROM*>& getHorizontalAngleROMs() const { return horizontalAngleROMs; }
	void addHorizontalAngleROM(const HorizontalAngleROM* rom) { horizontalAngleROMs.push_back(rom); }

	const list<const ZenithDistanceROM*>& getZenithDistanceROMs() const { return zenithDistanceROMs; }
	void addZenithDistanceROM(const ZenithDistanceROM* rom) { zenithDistanceROMs.push_back(rom); }

	const list<const SpatialDistanceROM*>& getSpatialDistanceROMs() const { return spatialDistanceROMs; }
	void addSpatialDistanceROM(const SpatialDistanceROM* rom) { spatialDistanceROMs.push_back(rom); }

	const list<const HorizontalDistanceROM*>& getHorizontalDistanceROMs() const { return horizontalDistanceROMs; }
	void addHorizontalDistanceROM(const HorizontalDistanceROM* rom) { horizontalDistanceROMs.push_back(rom); }

	const list<const OffsetToTheodolitePlaneROM*>& getOffsetToTheodolitePlaneROMs() const { return offsetToTheodolitePlaneROMs; }
	void addOffsetToTheodolitePlaneROM(const OffsetToTheodolitePlaneROM* rom) { offsetToTheodolitePlaneROMs.push_back(rom); }

private:

	const TheodoliteTarget* defaultTarget;
	const TAngleConstants* totalStationROMAngleConstant;

	list<const OffsetToTheodolitePlaneROM*> offsetToTheodolitePlaneROMs;
	list<const PolarROM*> polarROMs;
	list<const HorizontalAngleROM*> horizontalAngleROMs;
	list<const ZenithDistanceROM*> zenithDistanceROMs;
	list<const SpatialDistanceROM*> spatialDistanceROMs;
	list<const HorizontalDistanceROM*> horizontalDistanceROMs;
};

#endif

