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
#include	"GyroscopeStationConverter.h"
#include	"TheodoliteStationConverter.h"
#include	"THorAngleConverter.h"
#include	"TheodoliteTarget.h"
#include	"TTheodolite.h"


GyroscopeStationConverter::GyroscopeStationConverter(TAStreamFormatter& stream) : TAngleObsConverter(stream)
{
}

bool GyroscopeStationConverter::readObservations(int& lineNumber, TLGCProject* project)
{
	string sp;
	string gyroID;

	*fStream >> sp;
	*fStream >> gyroID;

	PointIterator p = project->getDataSet()->getWorkingPoints()->getPoint(sp);
	if (p == project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
	{
		char lineNum[10];
		_itoa(lineNumber, lineNum, 10);
		project->setError(project->getError() + "Line: " + lineNum + " - Unknown point " + sp + "\n");
		return false;
	}
	TSpatialPoint* stationPoint = &*p;
	Gyroscope* gyro = project->getDataSet()->getWorkingInstruments()->getGyroscopeNamed(gyroID);
	if (gyro == NULL)
	{
		char lineNum[10];
		_itoa(lineNumber, lineNum, 10);
		project->setError(project->getError() + "Line: " + lineNum + " - Unknown Gyroscope " + gyroID + "\n");
		return false;
	}
	
	const TLength* sic = readOptionalValue(TLength::kMillimetres, "ICSE", gyro->getInstrumentCenteringSigma());
	TAngle t = gyro->getAngleConstant()->getValue();
	TAngle* angle = readOptionalValue(TAngle::kCCs, "CST", &t);
	
	TAngleConstants* constant = new TAngleConstants(TAngle(*angle), TAMeasurement::kFixed);
	delete angle;

	TGyroOrientationROM* rom = new TGyroOrientationROM();

	lineNumber++;

	bool disallowDuplicates = project->getDataSet()->disallowsDuplicates();

	hash_set<THorAngleMeasurement, hash_function_for_observations<THorAngleMeasurement> > observationsSet;

	while (fStream->peek() != '*')
	{
		if (isComment())
		{
			fStream->readLine();
		}
		else
		{
			THorAngleMeasurement* meas = readObservation(gyro, project, lineNumber);
			if (meas == NULL)
			{
				delete constant;
				delete rom;
				return false;
			}
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
			fStream->skipWhiteSpace();
		}
	}

	GyroscopeStation* station = new GyroscopeStation(stationPoint, gyro, sic, constant, rom);

	project->getDataSet()->addToGyroOrieNum(rom->getMeasurements().size());

	project->getDataSet()->getWorkingStations()->addGyroscopeStation(station);

	return true;
}


THorAngleMeasurement* GyroscopeStationConverter::readObservation(const Gyroscope* gyro, TLGCProject* project, int& i)
{
	string targetPoint;
	TAngle* observedValue = new TAngle();

	*fStream >> targetPoint;
	fStream->setAngleUnits(TAngle::kGons);
	*fStream >> *observedValue;

	TSpatialPoint* point = &*(project->getDataSet()->getWorkingPoints()->getPoint(targetPoint));
	
	TAngle* angleSigma = readOptionalValue(TAngle::kCCs, "OBSE", gyro->getAngleSigma());
	const TLength* targetCenteringSigma = readOptionalValue(TLength::kMillimetres, "TCSE", gyro->getTargetCenteringSigma());

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

	THorAngleMeasurement* newMeas = new THorAngleMeasurement(project->getDataSet()->getObservationsCount()++, point, NULL,
		observedValue, angleSigma, targetCenteringSigma);

	setComments(newMeas, headCommentLine, comdb);

	fStream->readLine();

	return newMeas;
}

void	GyroscopeStationConverter::writeResultsHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,		"POINT"); //second point's Name
	(*stream).writeString(obsWidth,		"OBSERVE"); //mesured angle
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsWidth,		"CALCULE"); //estimated angle
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset (cc)
	(*stream).writeString(obsResWidth,	"ECART"); //offset (mm)
	(*stream).writeString(obsResWidth,	"RES"); //offset/sigma
	(*stream).writeString(obsWidth,		"DISTANCE"); //distance
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream).writeString(nameWidth,		""); //second point's Name
	(*stream).writeString(obsWidth,		"(GRAD)"); //mesured angle
	(*stream).writeString(obsResWidth,	"(CC)"); //sigma
	(*stream).writeString(obsWidth,		"(GRAD)"); //estimated angle
	(*stream).writeString(obsResWidth,	"(CC)"); //offset (cc)
	(*stream).writeString(obsResWidth,	"(MM)"); //offset (mm)
	(*stream).writeString(obsResWidth,	"/SIG"); //offset/sigma
	(*stream).writeString(obsWidth,		"(M)"); //distance
	(*stream)<<endl<<endl;

	return;
}



void	GyroscopeStationConverter::writeResults(LSGyroOrieConstIter obsIt)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResidualPrecision = getAngleResidualPrecision();
	int					lengthPrecision = getLengthPrecision();
	string				separator = getSeparator();

	//write Point 2
	(*stream).writeStringLeft(nameWidth, obsIt->getTgPointName());
	//write the observed angle
	stream->setAngleUnits(TAngle::kGons);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(anglePrecision);
	(*stream)<<(obsIt->getCorrectedObsAngle())<<(separator);

	//write the sigma
	stream->setAngleUnits(TAngle::k100MicroGons);
	stream->setWidthFormat(obsResWidth);
	stream->setPrecisionFormat(angleResidualPrecision);
	(*stream)<<(obsIt->getSigmaAPriori())<<(separator);

	//write the estimated angle
	stream->setAngleUnits(TAngle::kGons);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(anglePrecision);
	(*stream)<<(obsIt->getCorrectedEstimatedAngle())<<(separator);

	//write the offset (cc) after calculation
	stream->setWidthFormat(obsResWidth);
	stream->setAngleUnits(TAngle::k100MicroGons);
	stream->setPrecisionFormat(angleResidualPrecision);
	TAngle residue = obsIt->getResidue();
	(*stream)<<(residue)<<separator;

	//build and write the offset after calculation 
	stream->setLengthUnits(TLength::kMillimetres);
	stream->setPrecisionFormat(lengthPrecision);
	(*stream)<<(obsIt->getTgMovement())<<(separator);

	//build and write the offset / sigma
	stream->setPrecisionFormat(1);
	TAngle sigma = obsIt->getSigmaAPriori();
	(*stream)<<(obsIt->getResDivSig())<<(separator);

	//write the distance
	stream->setLengthUnits(TLength::kMetres);
	stream->setWidthFormat(obsWidth);
	(*stream)<<(obsIt->getStTgDistance())<<(separator)<<endl;
	(*stream).setDataSpacing();

	return;
}




void	GyroscopeStationConverter::writeMesData(LSGyroOrieConstIter	obsIt)
{
	TAngle v0;

	formatMesLine(	obsIt->getId(),
					obsIt->getStPointName(),
					obsIt->getTgPointName(),
					"",
					obsIt->getCorrectedObsAngle(),
					obsIt->getCorrectedEstimatedAngle(),
					obsIt->getResidue(),
					obsIt->getSigmaAPriori(),
					v0,
					obsIt->getComment());
	return;
}

