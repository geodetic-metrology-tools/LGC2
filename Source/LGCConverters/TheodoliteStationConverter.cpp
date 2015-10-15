////////////////////////////////////////////////////////////////////
// TheodoliteStationConverter.cpp


#include "TheodoliteStationConverter.h"
#include "THorAngleConverter.h"
#include "TZenithDistConverter.h"
#include "TSpatialDistConverter.h"
#include "THorDistConverter.h"
#include "TOffsetToTheoPlaneConverter.h"
#include "PolarConverter.h"

bool TheodoliteStationConverter::readTheodoliteStation(int& lineNumber, TLGCProject* project)
{
	string stationPtName, instrID;

	*fStream >> stationPtName;
	*fStream >> instrID;

	PointIterator p = project->getDataSet()->getWorkingPoints()->getPoint(stationPtName);
	if (p == project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
	{
		char lineNum[10];
		_itoa(lineNumber, lineNum, 10);
		project->setError(project->getError() + "Line: " + lineNum + " - Unknown point " + stationPtName + "\n");
		return false;
	}
	TSpatialPoint* point = &*p;
	TTheodolite* theo = project->getDataSet()->getWorkingInstruments()->getTheodoliteNamed(instrID);
	if (theo == NULL)
	{
		char lineNum[10];
		_itoa(lineNumber, lineNum, 10);
		project->setError(project->getError() + "Line: " + lineNum + " - Unknown Theodolite " + instrID + "\n");
		return false;
	}

	bool rot3D = readOptionalExists("ROT3D");
	bool ihFixed = true;
	const TLength* ih = NULL;
	const TLength* ihse = NULL;
	if (!rot3D)
	{
		ihFixed = readOptionalExists("IHFIX");
		
		ih = readOptionalValue(TLength::kMetres, "IH", theo->getInstrumentHeight());
		ihse = readOptionalValue(TLength::kMillimetres, "IHSE", theo->getInstrumentHeightSigma());
	}
	if (ih == NULL)
	{
		ih = new TLength(0);
		ihse = new TLength(0);
	}
	string trgt = readOptional("TRGT");
	const TLength* sic = readOptionalValue(TLength::kMillimetres, "ICSE", theo->getInstrumentCenteringSigma());
	
	const TheodoliteTarget* target;

	if (trgt != "")
	{
		target = theo->getTargetNamed(trgt);

		if (target == NULL)
		{
			char lineNum[10];
			_itoa(lineNumber, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - Unknown Theodolite target " + trgt + "\n");
			return false;
		}
	}
	else
	{
		target = theo->getDefaultTarget();
	}

	TheodoliteStation* station = new TheodoliteStation(point, theo, ih, ihse, !ihFixed && !rot3D, target, sic, rot3D, project->getDataSet()->getUnknownsCount()++);
	lineNumber++;
	fStream->readLine();

	while (readOptionalExists("*V0"))
	{
		string trgtROM = readOptional("TRGT");

		const TheodoliteTarget* romTarget;

		if (trgtROM != "")
		{
			romTarget = theo->getTargetNamed(trgtROM);
			if (romTarget == NULL)
			{
				delete station;

				char lineNum[10];
				_itoa(lineNumber, lineNum, 10);
				project->setError(project->getError() + "Line: " + lineNum + " - Unknown Theodolite target " + trgtROM + "\n");
				return false;
			}
		}
		else
		{
			romTarget = target;
		}

		string acstString = readOptional("ACST");
		
		TAngleConstants* acst;
		if (acstString != "")
		{
			fStream->seekg(-4, ios_base::cur);
			TAngle t = theo->getAngleConst()->getValue();
			TAngle* angle = readOptionalValue(TAngle::kCCs, "ACST", &t);
			acst = new TAngleConstants(*angle, TAMeasurement::kFixed);
			delete angle;
		}
		else
		{
			acst = new TAngleConstants(*(theo->getAngleConst()));
		}
		fStream->readLine();

		TheodoliteStationROM* rom = new TheodoliteStationROM(romTarget, acst);

		if (!readTheodoliteStationROM(++lineNumber, project, rom, theo))
		{
			delete station;
			delete rom;

			return false;
		}

		station->addTheodoliteStationROM(rom);

		fStream->skipWhiteSpace();
	}

	project->getDataSet()->getWorkingStations()->addTheodoliteStation(station);

	return true;
}

bool TheodoliteStationConverter::readTheodoliteStationROM(int& lineNumber, TLGCProject* project, TheodoliteStationROM* rom, TTheodolite* theo)
{
	if (readOptionalExists("*ANGL"))
	{
        HorizontalAngleROM* r = readObservationROM<HorizontalAngleROM, THorAngleMeasurement, THorAngleConverter>(lineNumber, project, rom, theo);
		if (r == NULL)
		{
			return false;
		}
		rom->addHorizontalAngleROM(r);

        project->getDataSet()->addToAnglNum(r->getMeasurements().size());
        
		return readTheodoliteStationROM(lineNumber, project, rom, theo);
	}
	else if (readOptionalExists("*ZEND"))
	{
        ZenithDistanceROM* r = readObservationROM<ZenithDistanceROM, TZenithDistMeasurement, TZenithDistConverter>(lineNumber, project, rom, theo);
		if (r == NULL)
		{
			return false;
		}
		rom->addZenithDistanceROM(r);

        project->getDataSet()->addToZenDistNum(r->getMeasurements().size());

		return readTheodoliteStationROM(lineNumber, project, rom, theo);
	}
	else if (readOptionalExists("*DRDL"))
	{
        SpatialDistanceROM* r = readObservationROM<SpatialDistanceROM, TSpatialDistMeasurement<TheodoliteTarget>, TSpatialDistConverter>(lineNumber, project, rom, theo);
		if (r == NULL)
		{
			return false;
		}
		rom->addSpatialDistanceROM(r);

        project->getDataSet()->addToSpaDistNum(r->getMeasurements().size());

		return readTheodoliteStationROM(lineNumber, project, rom, theo);
	}
	else if (readOptionalExists("*DHOR"))
	{
        HorizontalDistanceROM* r = readObservationROM<HorizontalDistanceROM, THorizontalDistMeas<TheodoliteTarget>, THorDistConverter>(lineNumber, project, rom, theo);
		if (r == NULL)
		{
			return false;
		}
		rom->addHorizontalDistanceROM(r);

        project->getDataSet()->addToHorDistNum(r->getMeasurements().size());

		return readTheodoliteStationROM(lineNumber, project, rom, theo);
	}
	else if (readOptionalExists("*PLR"))
	{
		project->getLSCalcDataSet()->setIsCombinedCase();

        PolarROM* r = readObservationROM<PolarROM, PolarMeasurement, PolarConverter>(lineNumber, project, rom, theo);
		if (r == NULL)
		{
			return false;
		}
		rom->addPolarROM(r);

        project->getDataSet()->addToPolar(r->getMeasurements().size());

		return readTheodoliteStationROM(lineNumber, project, rom, theo);
	}
	else if (readOptionalExists("*ECTH"))
	{		
		string scaleID;
		TAngle* observedAngle = new TAngle();

		fStream->setAngleUnits(TAngle::kGons);
		*fStream >> *observedAngle;
		*fStream >> scaleID;

		const Scale* scale = project->getDataSet()->getWorkingInstruments()->getScaleNamed(scaleID);
		if (scale == NULL)
		{
			delete observedAngle;

			char lineNum[10];
			_itoa(lineNumber, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - Unknown Scale " + scaleID + "\n");
			return false;
		}
		
		OffsetToTheodolitePlaneROM* oROM = new OffsetToTheodolitePlaneROM(observedAngle);
		lineNumber++;

		if (!readObservationROMWithScale<OffsetToTheodolitePlaneROM, TOffsetToTheoPlaneMeasurement, TOffsetToTheoPlaneConverter, TAngle>
			(lineNumber, project, oROM, scale, observedAngle))
		{
			delete oROM;
			return false;
		}
		
        project->getDataSet()->addToOffsetToTheoPlaneNum(oROM->getMeasurements().size());

		rom->addOffsetToTheodolitePlaneROM(oROM);

		return readTheodoliteStationROM(lineNumber, project, rom, theo);
	}
	else
	{
		return true;
	}
}
