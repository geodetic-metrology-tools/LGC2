////////////////////////////////////////////////////////////////////
// TVertDistConverter.cpp
/*!
Write Vertical Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TVertDistConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"
#include	"TheodoliteStationConverter.h"

#include	"TLGCProject.h"
/////////////////////////////////////////////////////////////////////

//ClassImp(TVertDistConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
//default constructor
TVertDistConverter::TVertDistConverter(TAStreamFormatter& stream) :
TLengthObsConverter(stream)
{
	insertKeyChar(EOF);
	insertKeyChar('\n');
	insertKeyChar('/');
	insertKeyChar('$');
	insertKeyChar('%');

}

//destructor
TVertDistConverter::~TVertDistConverter()
{}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TVertDistConverter::writeResultsHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"TARGET 1"); //first point's Name
	(*stream).writeStringLeft(nameWidth,	"TARGET 2"); //second point's Name
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured distance
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated distance
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset (mm)
	(*stream).writeString(obsResWidth,	"RES"); //offset/sigma
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream).writeString(nameWidth,	""); //first point's Name
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	"(M)"); //mesured distance
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma
	(*stream).writeString(obsWidth,	"(M)"); //estimated distance
	(*stream).writeString(obsResWidth,	"(MM)"); //offset (mm)
	(*stream).writeString(obsResWidth,	"/SIG"); //offset/sigma
	(*stream)<<endl<<endl;
	
	return;
}



void	TVertDistConverter::writeResults(LSVertDistConstIter obsIt)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();

	//write Point 1
	(*stream).writeStringLeft(nameWidth, obsIt->getTg1PointName());
	//write Point 2
	(*stream).writeStringLeft(nameWidth, obsIt->getTg2PointName());
	//write the observed distance
	stream->setLengthUnits(TLength::kMetres);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);
	(*stream)<<(obsIt->getCorrectedObsDist())<<(separator);

	//write the sigma
	stream->setLengthUnits(TLength::kMillimetres);
	stream->setWidthFormat(obsResWidth);
	stream->setPrecisionFormat(lengthResidualPrecision);
	(*stream)<<(obsIt->getSigmaAPriori())<<(separator);

	//write the estimated distance
	stream->setLengthUnits(TLength::kMetres);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);
	(*stream)<<(obsIt->getCorrectedEstDist())<<(separator);

	//write the offset (mm) after calculation
	stream->setWidthFormat(obsResWidth);
	stream->setLengthUnits(TLength::kMillimetres);
	stream->setPrecisionFormat(lengthResidualPrecision);
	TLength residue = obsIt->getResidue();
	(*stream)<<(residue)<<separator;


	//write the offset / sigma
	stream->setPrecisionFormat(2);
	(*stream)<<(obsIt->getResDivSig())<<(separator)<<endl;
	(*stream).setDataSpacing();

	return;
}


/////////////////////////////////////////////////////////////////////////
//RELIABILITY
/////////////////////////////////////////////////////////////////////////
void	TVertDistConverter::writeReliabilityData(TLSCalcVertDistObservation& obs)
{
	// TODO: fix!
	/*this->TLengthObsConverter::writeReliabilityData(	obs,
														obs.getTg1PointName(),
														obs.getTg2PointName(),
														"");
	return;*/
}

void	TVertDistConverter::writeReliabilityHeader()
{
	// TODO: fix!
	/*this->TObservationConverter::writeReliabilityHeader("TARGET 1",
														"TARGET 2",
														"",
														"OBSERVE",
														"M",
														"MM");
	return;*/
}



/////////////////////////////////////////////////////////////////////////
//PUNCH MES
/////////////////////////////////////////////////////////////////////////
void	TVertDistConverter::writeMesData(LSVertDistConstIter obsIt)
{
	/*TAngle gis, noV0;
	formatMesLine(	obsIt->getId(),
					obsIt->getRefPointName(),
					obsIt->getTgPointName(),
					"",
					gis,
					obsIt->getCorrectedObsDist(),
					obsIt->getCorrectedEstDist(),
					obsIt->getResidue(),
					obsIt->getSigmaAPriori(),
					noV0,
					obsIt->getComment());
	return;*/
}






/////////////////////////////////////////////////////////////////////////
//READ FUNCTION
/////////////////////////////////////////////////////////////////////////


VerticalDistanceROM* TVertDistConverter::readObservations(int& lineNumber, TLGCProject* project)
{
	string levelID;

	*fStream >> levelID;

	Level* level = project->getDataSet()->getWorkingInstruments()->getLevelNamed(levelID);
	if (level == NULL)
	{
		char lineNum[10];
		_itoa(lineNumber, lineNum, 10);
		project->setError(project->getError() + "Line: " + lineNum + " - Unknown Level " + levelID + "\n");
		return NULL;
	}

	string s = readOptional("TRGT");

	const Staff* staff1, *staff2;
	if (s == "")
	{
		staff1 = level->getDefaultStaff();
	}
	else
	{
		staff1 = level->getStaffNamed(s);
		if (staff1 == NULL)
		{
			char lineNum[10];
			_itoa(lineNumber, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - Unknown staff " + s + "\n");
			return NULL;
		}
	}
	staff2 = staff1;

	VerticalDistanceROM* rom = new VerticalDistanceROM(level);

	lineNumber++;

	bool disallowDuplicates = project->getDataSet()->disallowsDuplicates();

	hash_set<TVerticalDistMeasurement, hash_function_for_observations<TVerticalDistMeasurement> > observationsSet;

	while (fStream->peek() != '*')
	{
		if (isComment())
		{
			fStream->readLine();
		}
		else
		{
			TVerticalDistMeasurement* meas = readObservation(level, staff1, staff2, project, lineNumber);
			if (meas == NULL)
			{
				delete rom;
				return NULL;
			}
			staff1 = meas->getFirstStaff();
			staff2 = meas->getSecondStaff();
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

	return rom;
}


TVerticalDistMeasurement* TVertDistConverter::readObservation(const Level* level, const Staff* st1, const Staff* st2, TLGCProject* project, int& i)
{
	TSpatialPointName tg1Name, tg2Name;
	TLength obsDist(LITERAL(0.0));

	readPtName(tg1Name);
	readPtName(tg2Name);
	readMeasurement(obsDist);

	TSpatialPoint* point1 = &*(project->getDataSet()->getWorkingPoints()->getPoint(tg1Name));
	TSpatialPoint* point2 = &*(project->getDataSet()->getWorkingPoints()->getPoint(tg2Name));

	string s1 = readOptional("TRGT1");
	string s2 = readOptional("TRGT2");

	const Staff* staff1, *staff2;
	if (s1 == "")
	{
		staff1 = st1;
	}
	else
	{
		staff1 = level->getStaffNamed(s1);
	}
	if (s2 == "")
	{
		staff2 = st2;
	}
	else
	{
		staff2 = level->getStaffNamed(s2);
	}
	
	const TLength* obsSigma1 = readOptionalValue(TLength::kMillimetres, "OBSE1", staff1->getDistanceSigma());
	const TLength* ppmE1 = readOptionalValue(TLength::kMillimetres, "PPM1", staff1->getPPM());
	const TLength* obsSigma2 = readOptionalValue(TLength::kMillimetres, "OBSE2", staff2->getDistanceSigma());
	const TLength* ppmE2 = readOptionalValue(TLength::kMillimetres, "PPM2", staff2->getPPM());

	const TLength* th1 = readOptionalValue(TLength::kMetres, "TH1", staff1->getTargetHeight());
	const TLength* th2 = readOptionalValue(TLength::kMetres, "TH2", staff2->getTargetHeight());
	const TLength* thse1 = readOptionalValue(TLength::kMillimetres, "THSE1", staff1->getTargetHeightSigma());
	const TLength* thse2 = readOptionalValue(TLength::kMillimetres, "THSE2", staff2->getTargetHeightSigma());

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

	TVerticalDistMeasurement* newMeas = new TVerticalDistMeasurement(project->getDataSet()->getObservationsCount()++, point1, point2,
		new TLength(obsDist), obsSigma1, ppmE1, obsSigma2, ppmE2, th1, thse1, th2, thse2, staff1, staff2);

	setComments(newMeas, headCommentLine, comdb);

	fStream->readLine();

	return newMeas;
}








/////////////////////////////////////////////////////////////////////////
//END
/////////////////////////////////////////////////////////////////////////
