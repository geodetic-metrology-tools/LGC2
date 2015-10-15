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
void	TVertDistConverter::writeResultsHeader(const int)
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
	(*stream).writeStringLeft(nameWidth, obsIt->getRefPointName());
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
	this->TLengthObsConverter::writeReliabilityData(	obs,
														obs.getRefPointName(),
														obs.getTgPointName(),
														"");
	return;
}

void	TVertDistConverter::writeReliabilityHeader()
{
	this->TObservationConverter::writeReliabilityHeader("REF.",
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
void	TVertDistConverter::writeMesData(LSVertDistConstIter obsIt)
{
	TAngle gis, noV0;
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
	return;
}






/////////////////////////////////////////////////////////////////////////
//READ FUNCTION
/////////////////////////////////////////////////////////////////////////
int TVertDistConverter::readObservation(int i, int& numOfMeas, TLGCProject* project, VertDistMode mode)
{
	TAStreamFormatter*	stream = getStream();

	char lineNum[9];
	string iLine, line, error;

	TDistConstants cte;
	TVerticalDistROM* newVDRom = new TVerticalDistROM;
//	TVerticalDistMeasurement* newVDist = new TVerticalDistMeasurement;
	string headCommentLine = "";

	TReal colSigma = LITERAL(1.0);
	bool noColSig = readColSigma(colSigma, i);
	if(noColSig)
	{
		project->getDataSet()->setDefaultVertDistSig(true, mode==kDLEV);
	}

	//read the vertical distance measurements
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
			TLength obsDist(LITERAL(0.0)), sigma(LITERAL(0.0));
			TLength cSigma;

			readVerticalDistStation(*newVDRom);

			readPtName(tgName);
			readMeasurement(obsDist);
			readSigma(sigma);
				
			//create the horizontal angle measurement
			TVerticalDistMeasurement* newVDist = new TVerticalDistMeasurement(tgName, obsDist, sigma);

			// set of the global sigma, if necessary
			cSigma.setMMetresValue(colSigma);
			newVDist->ifNotDoneSetSigma(cSigma);

		
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
			int id1=-1, id2=-1;
			string EOLComments="";
			string comdb="";
			string comment="";
	
			while (!(stream->peek() == '\n'))
			{

				switch (stream->peek())
				{
					case '/':
						readConstant(cte);
						break;

					case '$':
						// identifier and comment
						stream->readChar();

						stream->TAStreamFormatter::operator>>(id1);
		
						if (stream->peek() == '#')
						{
							stream->readChar();
							if (stream->peek() == '$')
							{
								stream->readChar();
								stream->TAStreamFormatter::operator>>(id2);
		
								if (stream->peek() == '#')
								{
									stream->readChar();
									while(stream->peek() != EOF && stream->peek() != '\n')
									{
										stream->TAStreamFormatter::operator>>(comment);
										comdb += comment+ " ";
										stream->skipWhiteSpace();
									}
								}
								else
								{
									_itoa(i, lineNum, 10);
									iLine = lineNum;
									line = "line number " + iLine + " : ";
									string error = project->getError();
									error=error+line+ "Missing ""#"" after id. number" + '\n';
									project->setError(error);
								}
							}
							else
							{
								while(stream->peek() != EOF && stream->peek() != '\n' && stream->peek() != '$')
								{
									stream->TAStreamFormatter::operator>>(comment);
									comdb += comment+ " ";
									stream->skipWhiteSpace();
								}
							}
						}
						else
						{
							_itoa(i, lineNum, 10);
							iLine = lineNum;
							line = "line number " + iLine + " : ";
							string error = project->getError();
							error=error+line+ "Missing ""#"" after id. number" + '\n';
							project->setError(error);
						}
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
	
			if (newVDist->getId() == -1)
			{newVDist->setId(++numOfMeas);}

			// add comments
			if (id2 == -1)
			{
				newVDist->setId(id1);
			}
			else
			{
				newVDist->setId(id2);
				newVDRom->setId(id1);
			}
			newVDist->setComment(comdb);
			newVDist->setHeaderComment(headCommentLine);
			// "clear" the comment line storage
			headCommentLine="";
			
			addConstant2List(cte, project);
			newVDist->ifNotDoneSetMeasConst(cte);
		
			// This method handles the cases where new stations/ROMs are needed
			insert = insertVertDistMeas(newVDRom, newVDist, project, mode, i);
			
			if(insert)
			{
				project->getDataSet()->addToVertDistNum(mode==kDLEV);
			}

			delete newVDist;
		}
		stream->readLine();
		stream->skipWhiteSpace();
		i++;
	}
	delete newVDRom;

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





void TVertDistConverter::readVerticalDistStation(TVerticalDistROM& levelROM)
{//inputs a vertical distance measurement ROM from the text stream
	TAStreamFormatter*	stream = getStream();

	TSpatialPointName refPtName;
	stream->TAStreamFormatter::operator>>(refPtName);

	TVerticalDistROM* rom = new TVerticalDistROM(refPtName);
	levelROM = *rom;
	
	delete rom;

	stream->skipWhiteSpace();

	return;
}


bool TVertDistConverter::insertVertDistMeas(TVerticalDistROM* newVDRom, TVerticalDistMeasurement* newVDist,
											TLGCProject* project, VertDistMode mode, int i )
{
		char lineNum[9];
		string iLine, line;
		bool insert = false;
		bool isDLEV = (mode==kDLEV);

		PointIterator iterEnd = project->getDataSet()->getWorkingPoints()->getPointsEndIterator();
		DistStIterator iterStEnd = project->getDataSet()->getWorkingStations()->getDistStEndIterator();


		// check if points are still in TWorkingPointsList
		if (project->getDataSet()->getWorkingPoints()->getPoint(newVDRom->getRefPtName()) != iterEnd)
		{
			if (project->getDataSet()->getWorkingPoints()->getPoint(newVDist->getTargetPoint()) != iterEnd)
			{
				// insert measurement in the last level station
				// if ROM has not already been created
				LevelStIterator iter =project->getDataSet()->getWorkingStations()->getLevelStBeginIterator(isDLEV);
				LevelStIterator iterEnd =project->getDataSet()->getWorkingStations()->getLevelStEndIterator(isDLEV);
				// if level station container is empty : create a new station, 
				// add current ROM and measurement
				if (iter==iterEnd)
				{
					TLevelStation* newLevelSt = new TLevelStation();
					newVDRom->addVertDist(newVDist);
					newLevelSt->addVertDistROM(newVDRom);
					project->getDataSet()->getWorkingStations()->addLevelStation(newLevelSt, isDLEV);
					insert = true;
					delete newLevelSt;

				}
				
				else 
				{	// if last ROM of the station is the current ROM
					LevelStIterator currentStation = --iterEnd;
					VertDistROMIterator currentROM = --(currentStation->getVertDistROMEndIterator());
					if (currentROM->getRefPtName() == newVDRom->getRefPtName())
					{
						currentROM->addVertDist(newVDist);	
						insert = true;
					}
					// if not, create a new ROM
					else
					{
						TLevelStation* newLevelSt = new TLevelStation();
						newVDRom->addVertDist(newVDist);
						newLevelSt->addVertDistROM(newVDRom);
						project->getDataSet()->getWorkingStations()->addLevelStation(newLevelSt, isDLEV);
						insert = true;
						delete newLevelSt;
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
