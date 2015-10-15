////////////////////////////////////////////////////////////////////
// THorAngleConverter.cpp
/*!
Write Horizontal Angle Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"THorAngleConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"
//#include	"TLGCProject.h"
#include	<iostream>

/////////////////////////////////////////////////////////////////////

//ClassImp(THorAngleConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
//default constructor
THorAngleConverter::THorAngleConverter(TAStreamFormatter& stream) :
TAngleObsConverter(stream)
{
	insertKeyChar(EOF);
	insertKeyChar('\n');
	insertKeyChar('/');/*constant*/
	insertKeyChar('$') ; insertKeyChar('%');  /*comments*/
}

THorAngleConverter::~THorAngleConverter()
{//destructor
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	THorAngleConverter::writeResultsHeader(const int)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"POINT 1"); //first point's Name
	(*stream).writeStringLeft(nameWidth,	"POINT 2"); //second point's Name
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured angle
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated angle
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset (cc)
	(*stream).writeString(obsResWidth,	"ECART"); //offset (mm)
	(*stream).writeString(obsResWidth,	"RES"); //offset/sigma
	(*stream).writeString(obsWidth,	"DISTANCE"); //distance
	(*stream).writeString(obsWidth,	"V0"); //V0 
	(*stream)<<endl;	
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream).writeStringLeft(nameWidth,	""); //first point's Name
	(*stream).writeStringLeft(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	"(GRAD)"); //mesured angle
	(*stream).writeString(obsResWidth,	"(CC)"); //sigma
	(*stream).writeString(obsWidth,	"(GRAD)"); //estimated angle
	(*stream).writeString(obsResWidth,	"(CC)"); //offset (cc)
	(*stream).writeString(obsResWidth,	"(MM)"); //offset (mm)
	(*stream).writeString(obsResWidth,	"/SIG"); //offset/sigma
	(*stream).writeString(obsWidth,	"(M)"); //distance
	(*stream).writeString(obsWidth,	"(GRAD)"); //V0 
	(*stream)<<endl<<endl;
	return;
}



void	THorAngleConverter::writeResults(const TZEND& fAngl)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResidualPrecision = getAngleResidualPrecision();
	int					lengthPrecision = getLengthPrecision();
	string				separator = getSeparator();


	//write Point 1
	//(*stream).writeStringLeft(nameWidth, obsIt->getStPointName());

	//write Tg point
	(*stream).writeStringLeft(nameWidth, fAngl.targetPos->getName());

	//write the observed angle
	stream->setAngleUnits(TAngle::kGons);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(anglePrecision);
	(*stream)<<fAngl.getAngle().gon()<<(separator);

	//write the sigma
	stream->setAngleUnits(TAngle::k100MicroGons);
	stream->setWidthFormat(obsResWidth);
	stream->setPrecisionFormat(angleResidualPrecision);
	//(*stream)<<(obsIt->getSigmaAPriori())<<(separator);

	//write the estimated angle
	stream->setAngleUnits(TAngle::kGons);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(anglePrecision);
	(*stream)<<(fAngl.getAngle()+fAngl.getAngleResidual()).gon()<<(separator);

	//write the offset (cc) after calculation
	stream->setWidthFormat(obsResWidth);
	stream->setAngleUnits(TAngle::k100MicroGons);
	stream->setPrecisionFormat(angleResidualPrecision);
	(*stream)<<fAngl.getAngleResidual().gon()<<separator;

	//build and write the offset after calculation 
	stream->setLengthUnits(TLength::kMillimetres);
	stream->setPrecisionFormat(lengthPrecision);
	//(*stream)<<(obsIt->getTgMovement())<<(separator);

	//build and write the offset / sigma
	stream->setPrecisionFormat(1);
	//TAngle sigma = obsIt->getSigmaAPriori();
	//(*stream)<<(obsIt->getResDivSig())<<(separator);

	//write the distance
	stream->setLengthUnits(TLength::kMetres);
	stream->setWidthFormat(obsWidth);
	//(*stream)<<(obsIt->getStTgDistance())<<separator;

	//write V0
	stream->setAngleUnits(TAngle::kGons);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(anglePrecision);
	//(*stream)<<(obsIt->getEstimatedV0())<<separator<<endl;
	(*stream).setDataSpacing();

	return;
}

#if 0

void	THorAngleConverter::writeMesData(const TZEND&	obsIt)
{
	//formatMesLine(	obsIt->getId(),
	//				obsIt->getStPointName(),
	//				obsIt->getTgPointName(),
	//				"",
	//				obsIt->getCorrectedObsAngle(),
	//				obsIt->getCorrectedEstimatedAngle(),
	//				obsIt->getResidue(),
	//				obsIt->getSigmaAPriori(),
	//				obsIt->getEstimatedV0(),
	//				obsIt->getComment());
	return;
}



int		THorAngleConverter::readObservation(int i, int& numOfMeas, TLGCProject* project)
{
	TAStreamFormatter*	stream = getStream();

	char lineNum[9];
	string iLine, line, error;
	
	TTheodoliteStation* newThSt = new TTheodoliteStation;
	TAngleConstants cte;
	string headCommentLine = "";
	bool firstMeasurement = true;	
	
	TReal colSigma = LITERAL(0.0001);
	TAngle cSigma;
	bool noColSig = readColSigma(colSigma, i, project);
	if(noColSig)
	{
		project->getDataSet()->setDefaultAnglSig(true);
	}
	cSigma.setGonsValue(colSigma);
	
	//read the horizontal angle measurements
	while (((stream->peek()) != '*') && ((stream->peek()) != EOF))
	{
		bool insert = false;
		stream->skipWhiteSpace();
		
		if(stream->peek()=='%')
		{
			readP100Comment(headCommentLine);
		}

		else
		{
			// read the measurement
			TSpatialPointName tgName;
			TAngle obsAngle(LITERAL(0.0)), sigma(LITERAL(0.0));
			readTheoStation(*newThSt);
			readMeasurement(tgName, obsAngle, sigma);
			//create the horizontal angle measurement
			THorAngleMeasurement* newHAng = new THorAngleMeasurement(tgName, obsAngle, sigma);
			// set of the global sigma, if necessary
			newHAng->ifNotDoneSetSigma(cSigma);

			// set error message if there was a problem reading the measurement
			if (stream->getError() != "")
			{
				_itoa(i, lineNum, 10);
				iLine = lineNum;
				line = "line number " + iLine + " : ";
				string error = project->getError();
				error=error+ line + stream->getError() + '\n';
				project->setError(error);
				stream->setError("");
			}

			//read measurement comments, dB identifier, and measurement constant
			int id=-1;
			string comdb="";
			string EOLComments = "";
			
			while (!(stream->peek() == '\n'))
			{
				stream->skipWhiteSpace();
				char c = stream->peek();
				switch (c)
				{
					case '\t':
						stream->readChar();
						break;
					case '/':
						readConstant(cte);
						break;

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
						string error = project->getError();
						error=error+line+ "Too much information to read" + '\n';
						project->setError(error);
						while (!(stream->peek() == '\n'))
						{
							stream->readChar();
						}
						break;
				}
				stream->skipWhiteSpace();
			}

			// set the measurement and station parameters
			setId(newHAng, id, numOfMeas);
			setComments(newHAng, headCommentLine, comdb);
			setConstant(cte, newThSt, project);

			// insert the measurement, and the station if necessary
			insert = insertHorAngleMeas(newThSt, newHAng, project, i, firstMeasurement);
			
			if(insert)
			{
				project->getDataSet()->addToAnglNum();
				firstMeasurement = false;
			}

			delete newHAng;	
		}
		stream->readLine();
		stream->skipWhiteSpace();
		i++;
	}
	delete newThSt;	

	if (stream->peek() == EOF)
	{
		_itoa(i, lineNum, 10);
		iLine = lineNum;
		line = "line number " + iLine + " : ";
		string error = project->getError();
		error=error+line+ """*END"" keyword expected" + '\n';
		project->setError(error);
	}
	return i;
}



bool THorAngleConverter::insertHorAngleMeas(TTheodoliteStation* newThSt,
											THorAngleMeasurement* newHAng,
											TLGCProject* project,
											int i,
											bool firstMeasurement)
{
	char lineNum[9];
	string iLine, line;
	bool insert = false;

	PointIterator iterEnd = project->getDataSet()->getWorkingPoints()->getPointsEndIterator();
	TheodStIterator iterStEnd = project->getDataSet()->getWorkingStations()->getTheodStEndIterator();
	// TODO: changed this:
	//TheodStIterator station = --iterStEnd;
	TheodStIterator station;
	if (iterStEnd == project->getDataSet()->getWorkingStations()->getTheodStBeginIterator())
	{
		station = iterStEnd;
	}
	else
	{
		station = --iterStEnd;
	}

	// check if measurement points are in TWorkingPointsList
	// check the stationed point
	if (project->getDataSet()->getWorkingPoints()->getPoint(newThSt->getStationedPoint()) != iterEnd)
	{
		// check the measured point
		if (project->getDataSet()->getWorkingPoints()->getPoint(newHAng->getTargetPoint()) != iterEnd)
		{
			// if the station is a new station (including the first station)
			// create a new ROM, add the measurement to the ROM
			// add the ROM to the stations, and add the station to the working stations
//			if (project->getDataSet()->getWorkingStations()->numberOfTheodStations() == 0
			if (firstMeasurement || !(*newThSt == *(station)) )
			{
				// initialize the station with ROM and horizontal angle measurement
				THorAngleROM* newHARom = new THorAngleROM;
				newHARom->addHorAngle(newHAng);
				newThSt->addHorAngleROM(newHARom);
				// add the station to the working stations list
				project->getDataSet()->getWorkingStations()->addTheodStation(newThSt);
				insert = true;
				delete newHARom;
			}
			// station unchanged - add the measurement to the current ROM of the current station
			else
			{
				// add the measurement to the current ROM
				HorAngROMIterator ROM;
				ROM = --(station->getHorAngROMEndIterator());
				ROM->addHorAngle(newHAng);
				insert = true;

				// check if another constant value has been enterred for the current station
				// and if it has generate an appropriate error message
				if (!(newThSt->getAngleConst() == station->getAngleConst()))
				{
					_itoa(i, lineNum, 10);
					iLine = lineNum;
					line = "line number " + iLine + " : ";
					string error = project->getError();
					error=error+line+ "Station already has an angle constant" + '\n';
					project->setError(error);
				}
			}
		}
		// the measured point is not declared in the input file
		// generate the appropriate error message
		else
		{
			_itoa(i, lineNum, 10);
			iLine = lineNum;
			line = "line number " + iLine + " : ";
			string error = project->getError();
			error=error+line+ "Targeted Point doesn't exist" + '\n';
			project->setError(error);
		}
	}
	// the stationed point is not declared in the input file
	// generate the appropriate error message
	else
	{
		_itoa(i, lineNum, 10);
		iLine = lineNum;
		line = "line number " + iLine + " : ";
		string error = project->getError();
		error=error+line+ "Stationed Point doesn't exist" + '\n';
		project->setError(error);
	}

	return insert;
}
#endif





