////////////////////////////////////////////////////////////////////
// LevelStationConverter.cpp
/*!
Write Level Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"LevelStationConverter.h"
#include	"TheodoliteStationConverter.h"
#include	"THorizontalDistMeas.h"


LevelStationConverter::LevelStationConverter(TAStreamFormatter& stream) : TLengthObsConverter(stream)
{
}

bool LevelStationConverter::readObservations(int& lineNumber, TLGCProject* project)
{
	string sp = "";
	string levelID;
	string target;

	string temp;

	*fStream >> temp;

	if (!isComment() && (target = readOptional("TRGT")) == "" && fStream->peek() != '\n')
	{
		sp = temp;
		*fStream >> levelID;
	}
	else
	{
		levelID = temp;
	}

	target = readOptional("TRGT");

	TSpatialPoint* stationPoint;
	string name;
	if (sp != "")
	{
		PointIterator p = project->getDataSet()->getWorkingPoints()->getPoint(sp);
		if (p == project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
		{
			char lineNum[10];
			_itoa(lineNumber, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - Unknown point " + sp + "\n");
			return false;
		}
		stationPoint = &*p;
	}
	else
	{
		name = "Level_" + levelID;
		while (project->getDataSet()->getWorkingPoints()->getPoint(name) != project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
		{
			name += "_";
		}

		stationPoint = NULL;
	}

	Level* level = project->getDataSet()->getWorkingInstruments()->getLevelNamed(levelID);
	if (level == NULL)
	{
		char lineNum[10];
		_itoa(lineNumber, lineNum, 10);
		project->setError(project->getError() + "Line: " + lineNum + " - Unknown Level " + levelID + "\n");
		return false;
	}
	const Staff* staff;
	if (target == "")
	{
		staff = level->getDefaultStaff();
	}
	else
	{
		staff = level->getStaffNamed(target);

		if (staff == NULL)
		{
			char lineNum[10];
			_itoa(lineNumber, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - Unknown Level staff " + target + "\n");
			return false;
		}
	}
	
	lineNumber++;

	bool disallowDuplicates = project->getDataSet()->disallowsDuplicates();

	hash_set<LevelMeasurement, hash_function_for_observations<LevelMeasurement> > observationsSet;

	ObservationROM<LevelMeasurement>* rom = new ObservationROM<LevelMeasurement>();

	ObservationROM<THorizontalDistMeas<Staff> >* hdROM = new ObservationROM<THorizontalDistMeas<Staff> >();

	while (fStream->peek() != '*')
	{
		if (isComment())
		{
			fStream->readLine();
		}
		else
		{
			LevelMeasurement* meas = readObservation(level, staff, project, lineNumber);
			if (meas == NULL)
			{
				return false;
			}
			if (stationPoint == NULL)
			{
				TPositionVector p = meas->getTargetPoint()->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian);
				stationPoint = new TSpatialPoint(TSpatialPointName(name), new TSpatialPosition(project->getDataSet()
					->getDataParams().getRefFrame(), p.getX().getMetresValue(), p.getY().getMetresValue(),
					p.getZ().getMetresValue() - meas->getObservedValue()->getMetresValue(), TCoordSysFactory::k3DCartesian));
			}
			staff = meas->getStaff();
			lineNumber++;

			if (disallowDuplicates)
			{
				if (observationsSet.find(*meas) == observationsSet.end())
				{
					observationsSet.insert(*meas);
				}
				else
				{
					char lineNum[10];
					_itoa(lineNumber, lineNum, 10);
					project->setError(project->getError() + "Line: " + lineNum + " - Duplicate observation!\n");
				}
			}

			rom->addMeasurement(meas);

			if (meas->hasDistanceMeasurement())
			{
				THorizontalDistMeas<Staff>* m = new THorizontalDistMeas<Staff>(project->getDataSet()->getObservationsCount()++,
					meas->getTargetPoint(), staff, meas->getHorizontalDistanceValue(), meas->getHorizontalDistanceSigma(),
					new TLength(0), new TLength(0));

				hdROM->addMeasurement(m);
			}
			fStream->skipWhiteSpace();
		}
	}

	LevelStation* station = new LevelStation(stationPoint, level, rom, hdROM);

	project->getDataSet()->addToLevelObsNum(rom->getMeasurements().size());
	project->getDataSet()->addToHorDistNum(hdROM->getMeasurements().size());

	project->getDataSet()->getWorkingStations()->addLevelStation(station);
	
	project->getDataSet()->getWorkingPoints()->addPoint(stationPoint);

	return true;
}

LevelMeasurement* LevelStationConverter::readObservation(const Level* level, const Staff* staff, TLGCProject* project, int& i)
{
	TSpatialPointName target;
	TLength obsDist(LITERAL(0.0));

	readPtName(target);
	readMeasurement(obsDist);

	TSpatialPoint* point = &*(project->getDataSet()->getWorkingPoints()->getPoint(target));
	
	bool hasDistance = readOptionalExists("OD");

	TLength* hd = NULL;
	TLength* hds = NULL;

	if (hasDistance)
	{
		hd = new TLength();
		hds = new TLength();
		fStream->setLengthUnits(TLength::kMetres);
		*fStream >> *hd;
		fStream->setLengthUnits(TLength::kMillimetres);
		*fStream >> *hds;
	}

	string st = readOptional("TRGT");

	const Staff* s;
	if (st == "")
	{
		s = staff;
	}
	else
	{
		s = level->getStaffNamed(st);
	}
	const TLength* obsSigma = readOptionalValue(TLength::kMillimetres, "OBSE", s->getDistanceSigma());
	const TLength* ppmE = readOptionalValue(TLength::kMillimetres, "PPM", s->getPPM());
	const TLength* th = readOptionalValue(TLength::kMetres, "TH", s->getTargetHeight());
	const TLength* thse = readOptionalValue(TLength::kMillimetres, "THSE", s->getTargetHeightSigma());

	//read measurement comments, dB identifier, and measurement constant
	int id=-1;
	string comdb="";
	string EOLComments = "";
	char lineNum[9];
	string iLine, line, error;
	string headCommentLine = "";
	// set error message if there was a problem reading the measurement
	if (fStream->getError() != "")
	{
		_itoa(i, lineNum, 10);
		iLine = lineNum;
		line = "line number " + iLine + " : ";
		error = project->getError();
		error=error+ line + fStream->getError() + '\n';
		project->setError(error);
		fStream->setError("");
	}
	
	while (!(fStream->peek() == '\n'))
	{
		fStream->skipWhiteSpace();
		switch (fStream->peek())
		{
			case '$':
				readDBComments(	lineNum, iLine, line,
								i,
								id, comdb, EOLComments,
								project);
				break;

			case '#':
				readP100Comment(EOLComments);
				break;

			case '%':
				readP100Comment(EOLComments);
				break;

			default:
				// other characters to read on this line correspond to an error
				_itoa(i, lineNum, 10);
				iLine = lineNum;
				line = "line number " + iLine + " : ";
				error = project->getError();
				error=error+line+ "Too much information to read" + '\n';
				project->setError(error);
				while (!(fStream->peek() == '\n'))
				{
					fStream->readChar();
				}
				break;
		}
		fStream->skipWhiteSpace();
	}

	LevelMeasurement* newMeas = new LevelMeasurement(project->getDataSet()->getObservationsCount()++, point,
		new TLength(obsDist), obsSigma, ppmE, hd, hds, hasDistance, th, thse, s);

	setComments(newMeas, headCommentLine, comdb);

	fStream->readLine();

	return newMeas;
}
