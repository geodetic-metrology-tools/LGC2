////////////////////////////////////////////////////////////////////
// EDMSpatialDistConverter.cpp
/*!
Write Spatial Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"EDMSpatialDistConverter.h"
#include	"EDMStation.h"
#include	"TheodoliteStationConverter.h"


EDMSpatialDistConverter::EDMSpatialDistConverter(TAStreamFormatter& stream) : TLengthObsConverter(stream)
{
}

bool EDMSpatialDistConverter::readObservations(int& lineNumber, TLGCProject* project)
{
	string sp;
	string edmID;

	*fStream >> sp;
	*fStream >> edmID;

	PointIterator p = project->getDataSet()->getWorkingPoints()->getPoint(sp);
	if (p == project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
	{
		char lineNum[10];
		_itoa(lineNumber, lineNum, 10);
		project->setError(project->getError() + "Line: " + lineNum + " - Unknown point " + sp + "\n");
		return false;
	}
	TSpatialPoint* stationPoint = &*p;
	EDM* edm = project->getDataSet()->getWorkingInstruments()->getEDMNamed(edmID);
	if (edm == NULL)
	{
		char lineNum[10];
		_itoa(lineNumber, lineNum, 10);
		project->setError(project->getError() + "Line: " + lineNum + " - Unknown EDM " + edmID + "\n");
		return false;
	}
	
	string trgt = readOptional("TRGT");
	bool ihFixed = readOptionalExists("IHFIX");

	const EDMTarget* target;

	if (trgt == "")
	{
		target = edm->getDefaultTarget();
	}
	else
	{
		target = edm->getTargetNamed(trgt);

		if (target == NULL)
		{
			char lineNum[10];
			_itoa(lineNumber, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - Unknown EDM target " + trgt + "\n");
			return false;
		}
	}

	const TLength* ih = readOptionalValue(TLength::kMetres, "IH", edm->getInstrumentHeight());
	const TLength* ihse = readOptionalValue(TLength::kMillimetres, "IHSE", edm->getInstrumentHeightSigma());
	const TLength* sic = readOptionalValue(TLength::kMillimetres, "ICSE", edm->getInstrumentCenteringSigma());
	
	EDMSpatialDistanceROM* rom = new EDMSpatialDistanceROM();

	lineNumber++;

	TheodoliteStationConverter conv(*fStream);
	if (!conv.readObservations<EDMSpatialDistanceROM, TSpatialDistMeasurement<EDMTarget>, TSpatialDistConverter,
		EDMTarget, EDM>(lineNumber, project, rom, target, edm))
	{
		delete rom;
		return false;
	}

	EDMStation* station = new EDMStation(stationPoint, edm, ih, ihse, !ihFixed, target, sic, rom, project->getDataSet()->getUnknownsCount()++);

	project->getDataSet()->addToEDMSpaDistNum(rom->getMeasurements().size());

	project->getDataSet()->getWorkingStations()->addEDMStation(station);

	return true;
}
