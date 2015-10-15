////////////////////////////////////////////////////////////////////
// THorDistConverter.cpp
/*!
Write Horizontal Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"THorDistConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"

#include	"TLGCProject.h"
/////////////////////////////////////////////////////////////////////

//ClassImp(THorDistConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
//default constructor
THorDistConverter::THorDistConverter(TAStreamFormatter& stream) :
TLengthObsConverter(stream)
{
	insertKeyChar(EOF);
	insertKeyChar('\n');
	insertKeyChar('/');
	insertKeyChar('$');
	insertKeyChar('%');
}

THorDistConverter::~THorDistConverter()
{//destructor
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	THorDistConverter::writeResultsHeader(const int)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"POINT 1"); //first point's Name
	(*stream).writeStringLeft(nameWidth,	"POINT 2"); //second point's Name
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



void	THorDistConverter::writeResults(LSHorDistConstIter obsIt)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();

	//write Point 1
	(*stream).writeStringLeft(nameWidth, obsIt->getStPointName());
	//write Point 2
	(*stream).writeStringLeft(nameWidth, obsIt->getTgPointName());
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
	(*stream)<<(residue)<<(separator);


	//write the offset / sigma
	stream->setPrecisionFormat(2);
	(*stream)<<(obsIt->getResDivSig())<<(separator)<<endl;
	(*stream).setDataSpacing();

	return;
}


/////////////////////////////////////////////////////////////////////////
//RELIABILITY
/////////////////////////////////////////////////////////////////////////
void	THorDistConverter::writeReliabilityData(TLSCalcHorDistObservation& obs)
{
	this->TLengthObsConverter::writeReliabilityData(	obs,
														obs.getStPointName(),
														obs.getTgPointName(),
														"");
	return;
}


void	THorDistConverter::writeReliabilityHeader()
{
	this->TObservationConverter::writeReliabilityHeader("STATION",
														"TARGET",
														"",
														"OBSERVE",
														"M",
														"MM");
	return;
}



/////////////////////////////////////////////////////////////////////////
//PUNCH MES
/////////////////////////////////////////////////////////////////////////
void	THorDistConverter::writeMesData(LSHorDistConstIter obsIt)
{
	TAngle gis, v0;

	formatMesLine(	obsIt->getId(),
					obsIt->getStPointName(),
					obsIt->getTgPointName(), 
					"", 
					gis, 
					obsIt->getCorrectedObsDist(),
					obsIt->getCorrectedEstDist(), 
					obsIt->getResidue(), 
					obsIt->getSigmaAPriori(), 
					v0,
					obsIt->getComment());
	return;
}






////////////////////////////////////////////////////////////////////////////////////////////
//READ FUNCTION
////////////////////////////////////////////////////////////////////////////////////////////
int THorDistConverter::readObservation(int i, int& numOfMeas, TLGCProject* project)
{
	TAStreamFormatter*	stream = getStream();

	char lineNum[9];
	string iLine, line, error;

	TDistConstants cte;
	TDistMeasStation* newDistSt = new TDistMeasStation;
	string headCommentLine = "";
	bool firstLine = true;
	
	TReal colSigma = LITERAL(1.0), colSigmaPpm = LITERAL(0.0);
	bool noColSig = readColSigma(colSigma, colSigmaPpm, i);
	if(noColSig)
	{
		project->getDataSet()->setDefaultHorDistSig(true);
	}


	//read the horizontal angle measurements
	while ((stream->peek()) != '*')
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
			TLength obsDist(LITERAL(0.0)), sigma(LITERAL(0.0)), sigmaPpm(LITERAL(0.0));

			readDistStation(*newDistSt);
			readPtName(tgName);
			readMeasurement(obsDist);
			readSigma(sigma, sigmaPpm);
			//create the horizontal distance measurement
			THorizontalDistMeas* newHDist = new THorizontalDistMeas(tgName, obsDist, sigma);
			newHDist->ifNotDoneSetSigmaPpm(sigmaPpm);

			// set of the global sigma, if necessary
			TLength cSigma, cSigmaPpm;
			cSigma.setMMetresValue(colSigma);
			cSigmaPpm.setMMetresValue(colSigmaPpm);
			newHDist->ifNotDoneSetSigma(cSigma);
			newHDist->ifNotDoneSetSigmaPpm(cSigmaPpm);
		
			// add to fError errors from the read of measurement
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


			//read the comment, id. and constant
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

			setId(newHDist, id, numOfMeas);
			setComments(newHDist, headCommentLine, comdb);
			addConstant2List(cte, project);
			newHDist->ifNotDoneSetMeasConst(cte);
		
			if (firstLine == true && 
				project->getDataSet()->getWorkingPoints()->getPoint(newDistSt->getStationedPoint()) !=
				project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
			{
				// initialize the station with ROM and horizontal angle measurement
				THorizontalDistROM* newHDRom = new THorizontalDistROM;
				newHDRom->addHorDist(newHDist);
				newDistSt->addHorDistROM(newHDRom);
				// add the station to the working stations list
				project->getDataSet()->getWorkingStations()->addDistStation(newDistSt);
				insert = true;
				delete newHDRom;
				firstLine = false;
			}
			else
			{
				insert = insertHorDistMeas(newDistSt, newHDist, project, i);
			}

			if(insert)
			{
				project->getDataSet()->addToHorDistNum();
			}

			delete newHDist;
		}
		stream->readLine();
		stream->skipWhiteSpace();
		i++;		
	}
	delete newDistSt;

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


bool THorDistConverter::insertHorDistMeas( TDistMeasStation* newDistSt, THorizontalDistMeas* newHDist,
										  TLGCProject* project, int i )
{
	char lineNum[9];
	string iLine, line;
	bool insert = false;

	PointIterator iterEnd = project->getDataSet()->getWorkingPoints()->getPointsEndIterator();
	DistStIterator iterStEnd = project->getDataSet()->getWorkingStations()->getDistStEndIterator();
	DistStIterator station;
	if (iterStEnd == project->getDataSet()->getWorkingStations()->getDistStBeginIterator())
	{
		station = iterStEnd;
	}
	else
	{
		station = --iterStEnd;
	}


	// check if points are still in TWorkingPointsList
	if (project->getDataSet()->getWorkingPoints()->getPoint(newDistSt->getStationedPoint()) != iterEnd)
	{
		if (project->getDataSet()->getWorkingPoints()->getPoint(newHDist->getTargetPoint()) != iterEnd)
		{
			if (project->getDataSet()->getWorkingStations()->numberOfDistMeasStations() == 0)
			{
				// initialize the station with ROM and horizontal angle measurement
				THorizontalDistROM* newHDRom = new THorizontalDistROM;
				newHDRom->addHorDist(newHDist);
				newDistSt->addHorDistROM(newHDRom);
				// add the station to the working stations list
				project->getDataSet()->getWorkingStations()->addDistStation(newDistSt);
				insert = true;
				delete newHDRom;
			}
			else
			{
				// if newDistSt is not the current station
				if (!(*newDistSt == *(station)))
				{
					// initialize the station with ROM and horizontal angle measurement
					THorizontalDistROM* newHDRom = new THorizontalDistROM;
					newHDRom->addHorDist(newHDist);
					newDistSt->addHorDistROM(newHDRom);
					// add the station to the working stations list
					project->getDataSet()->getWorkingStations()->addDistStation(newDistSt);
					insert = true;
					delete newHDRom;
				}
				else // station is current
				{
					if (station->getHorDistROMDimension() == 0)
					{
						THorizontalDistROM* newHDRom = new THorizontalDistROM;
						newHDRom->addHorDist(newHDist);
						station->addHorDistROM(newHDRom);
						insert = true;
						delete newHDRom;
					}

					else
					{
						HorDistROMIterator ROM;
						ROM = --(station->getHorDistROMEndIterator());
						ROM->addHorDist(newHDist);
						insert = true;
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
		error=error+line+ "Reference Point doesn't exist" + '\n';
		project->setError(error);
	}
	return insert;
}















/////////////////////////////////////////////////////////////////////////
//END
/////////////////////////////////////////////////////////////////////////





