////////////////////////////////////////////////////////////////////
//TGyroOrientationConverter.cpp
/*!
Write Gyro Orientation Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TGyroOrientationConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"
#include	"TLGCProject.h"


/////////////////////////////////////////////////////////////////////

//ClassImp(TGyroOrientationConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
//default constructor
TGyroOrientationConverter::TGyroOrientationConverter(TAStreamFormatter& stream) :
TAngleObsConverter(stream)
{	
	insertKeyChar(EOF);
	insertKeyChar('\n');
	insertKeyChar('/');/*constant*/
	insertKeyChar('$') ; insertKeyChar('%'); /*comments*/
}

TGyroOrientationConverter::~TGyroOrientationConverter()
{//destructor
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TGyroOrientationConverter::writeResultsHeader(const int)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,		"POINT 1"); //first point's Name
	(*stream).writeStringLeft(nameWidth,		"POINT 2"); //second point's Name
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
	(*stream).writeString(nameWidth,		""); //first point's Name
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



void	TGyroOrientationConverter::writeResults(LSGyroOrieConstIter obsIt)
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
	(*stream).writeStringLeft(nameWidth, obsIt->getStPointName());
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




void	TGyroOrientationConverter::writeMesData(LSGyroOrieConstIter	obsIt)
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





/////////////////////////////////////////////////////////////////////////////////////////////////
//READER
/////////////////////////////////////////////////////////////////////////////////////////////////

int TGyroOrientationConverter::readObservation(int i, int& numOfMeas, TLGCProject* project)
{
	TAStreamFormatter*	stream = getStream();

	char lineNum[9];
	string iLine, line, error;
	
	TTheodoliteStation* newThSt = new TTheodoliteStation;
	TAngleConstants cte;
	string headCommentLine = "";
	bool firstLine = true;
		
	TReal colSigma = LITERAL(0.0001);
	bool noColSig = readColSigma(colSigma, i, project);
	if(noColSig)
	{
		project->getDataSet()->setDefaultGyroOrieSig(true);
	}


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
			TSpatialPointName tgName;
			TAngle obsAngle(LITERAL(0.0)), sigma(LITERAL(0.0));
			TAngle cSigma;
			readTheoStation(*newThSt);
			// read the angle measurement
			readMeasurement(tgName, obsAngle, sigma);
			//create the orientation measurement
			THorAngleMeasurement* newOrie = new THorAngleMeasurement(tgName, obsAngle, sigma);
			// set of the global sigma, if necessary
			cSigma.setGonsValue(colSigma);
			newOrie->ifNotDoneSetSigma(cSigma);


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

			//read measurement comments and identifier, and measurement constante
			int id=-1;
			string EOLComments="";
			string comdb="";
			
			while (!(stream->peek() == '\n'))
			{

				switch (stream->peek())
				{
					case '/':
						readConstant(cte);
						break;

					case '$':
						readDBComments(	lineNum, iLine, line,
										i,
										id, comdb, EOLComments,
										project);
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
						error=error+line+ "Too much informations to read" + '\n';
						project->setError(error);
						while (!(stream->peek() == '\n'))
						{
							stream->readChar();
						}
						break;
				}
				stream->skipWhiteSpace();
			}
			setId(newOrie, id, numOfMeas);
			setComments(newOrie, headCommentLine, comdb);
			setConstant(cte, newThSt, project);

			if (firstLine == true && 
				project->getDataSet()->getWorkingPoints()->getPoint(newThSt->getStationedPoint()) !=
				project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
			{
				// initialize the station with ROM and horizontal angle measurement
				TGyroOrientationROM* newOrieRom = new TGyroOrientationROM;
				newOrieRom->addGyroOrientation(newOrie);
				newThSt->addGyroOrieROM(newOrieRom);
				// add the station to the working stations list
				project->getDataSet()->getWorkingStations()->addTheodStation(newThSt);
				insert = true;
				delete newOrieRom;
				firstLine = false;
			}
			else
			{
				insert = insertGyroOrieMeas(newThSt, newOrie, project, i);
			}
	
			if(insert)
			{
				project->getDataSet()->addToGyroOrieNum();
			}

			delete newOrie;	
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



bool TGyroOrientationConverter::insertGyroOrieMeas( TTheodoliteStation* newThSt,
												   THorAngleMeasurement* newOrie,
												   TLGCProject* project,
												   int i )
{
	char lineNum[9];
	string iLine, line;
	bool insert = false;

	PointIterator iterEnd = project->getDataSet()->getWorkingPoints()->getPointsEndIterator();
	TheodStIterator iterStEnd = project->getDataSet()->getWorkingStations()->getTheodStEndIterator();
	TheodStIterator station;
	if (iterStEnd == project->getDataSet()->getWorkingStations()->getTheodStBeginIterator())
	{
		station = iterStEnd;
	}
	else
	{
		station = --iterStEnd;
	}



	// check if points are still in TWorkingPointsList
	if (project->getDataSet()->getWorkingPoints()->getPoint(newThSt->getStationedPoint()) != iterEnd)
	{
		if (project->getDataSet()->getWorkingPoints()->getPoint(newOrie->getTargetPoint()) != iterEnd)
		{
			if (project->getDataSet()->getWorkingStations()->numberOfTheodStations() == 0)
			{
				// initialize the station with ROM and horizontal angle measurement
				TGyroOrientationROM* newOrieRom = new TGyroOrientationROM;
				newOrieRom->addGyroOrientation(newOrie);
				newThSt->addGyroOrieROM(newOrieRom);
				// add the station to the working stations list
				project->getDataSet()->getWorkingStations()->addTheodStation(newThSt);
				insert = true;
				delete newOrieRom;
			}
			else
			{
				// if newDistSt is not the current station
				if (!(*newThSt == *(station)))
				{
					// initialize the station with ROM and horizontal angle measurement
					TGyroOrientationROM* newOrieRom = new TGyroOrientationROM;
					newOrieRom->addGyroOrientation(newOrie);
					newThSt->addGyroOrieROM(newOrieRom);
					// add the station to the working stations list
					project->getDataSet()->getWorkingStations()->addTheodStation(newThSt);
					insert = true;
					delete newOrieRom;
				}
				else // station is current
				{
					if (station->getGyroOrieROMDimension() == 0)
					{
						TGyroOrientationROM* newOrieRom = new TGyroOrientationROM;
						newOrieRom->addGyroOrientation(newOrie);
						station->addGyroOrieROM(newOrieRom);
						insert = true;
						delete newOrieRom;
					}

					else
					{
						GyroOrieROMIterator ROM;
						ROM = --(station->getGyroOrieROMEndIterator());
						ROM->addGyroOrientation(newOrie);
						insert = true;
					}

					if (!(newThSt->getAngleConst() == station->getAngleConst()))
					{
						_itoa(i, lineNum, 10);
						iLine = lineNum;
						line = "line number " + iLine + " : ";
						string error = project->getError();
						error=error+line+ "Station has already an angle constant : inserted in the next station" + '\n';
						project->setError(error);
					}
				}
			}
		}
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