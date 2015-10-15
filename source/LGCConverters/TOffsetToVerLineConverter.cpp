////////////////////////////////////////////////////////////////////
// TOffsetToVerLineConverter.cpp
/*!
Write Offset to a Vertival Lone Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TOffsetToVerLineConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"

#include	"TLGCProject.h"
/////////////////////////////////////////////////////////////////////

//ClassImp(TOffsetToVerLineConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
//default constructor
TOffsetToVerLineConverter::TOffsetToVerLineConverter(TAStreamFormatter& stream) :
TLengthObsConverter(stream)
{
	insertKeyChar(EOF);
	insertKeyChar('\n');
	insertKeyChar('/');
	insertKeyChar('$');
	insertKeyChar('%');
}


//destructor
TOffsetToVerLineConverter::~TOffsetToVerLineConverter()
{}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TOffsetToVerLineConverter::writeResultsHeader(const int)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"POINT 1"); //first point's Name
	(*stream).writeStringLeft(nameWidth,	"POINT 2"); //second point's Name
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured offset
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated offset
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset (mm)
	(*stream).writeString(obsResWidth,	"RES"); //offset/sigma
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream).writeString(nameWidth,	""); //first point's Name
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	"(M)"); //mesured offset
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma
	(*stream).writeString(obsWidth,	"(M)"); //estimated offset
	(*stream).writeString(obsResWidth,	"(MM)"); //offset (mm)
	(*stream).writeString(obsResWidth,	"/SIG"); //offset/sigma
	(*stream)<<endl<<endl;
	
	return;
}



void	TOffsetToVerLineConverter::writeResults(LSOffsetToVerLineConstIter obsIt)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();

	//write Point 1
	(*stream).writeStringLeft(nameWidth, obsIt->getFirstTgPointName());

	//write Point 2
	(*stream).writeStringLeft(nameWidth, obsIt->getStPointName());
	//write the observed offset
	stream->setLengthUnits(TLength::kMetres);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);
	(*stream)<<(obsIt->getCorrectedObsDist())<<separator;

	//write the sigma
	stream->setLengthUnits(TLength::kMillimetres);
	stream->setWidthFormat(obsResWidth);
	stream->setPrecisionFormat(lengthResidualPrecision);
	(*stream)<<(obsIt->getSigmaAPriori())<<(separator);

	//write the estimated offset
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
	(*stream)<<(obsIt->getResDivSig())<<separator<<endl;
	(*stream).setDataSpacing();

	return;
}

/////////////////////////////////////////////////////////////////////////
//RELIABILITY
/////////////////////////////////////////////////////////////////////////
void	TOffsetToVerLineConverter::writeReliabilityData(TLSCalcOffsetToVerLineObservation& obs)
{
	this->TLengthObsConverter::writeReliabilityData(	obs,
														obs.getFirstTgPointName(),
														obs.getStPointName(),
														"");
	return;
}


void	TOffsetToVerLineConverter::writeReliabilityHeader()
{
	this->TObservationConverter::writeReliabilityHeader("PLOMB",
														"STATION",
														"",
														"OBSERVE",
														"M",
														"MM");
	return;
}


/////////////////////////////////////////////////////////////////////////
//PUNCH MES
/////////////////////////////////////////////////////////////////////////
void	TOffsetToVerLineConverter::writeMesData(LSOffsetToVerLineConstIter obsIt)
{
	TAngle gis, noV0;
	formatMesLine(	obsIt->getId(),
					obsIt->getFirstTgPointName(),
					obsIt->getStPointName(), 
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











//////////////////////////////////////////////////////////////////////////////////////////////
//READ FUNCTION
//////////////////////////////////////////////////////////////////////////////////////////////
int TOffsetToVerLineConverter::readObservation(int i, int& numOfMeas, TLGCProject* project)
{
	TAStreamFormatter*	stream = getStream();

	char lineNum[9];
	string iLine, line, error;

	TDistConstants cte;
	TDistMeasStation* newDistSt = new TDistMeasStation;
//	TOffsetToVerLineMeasurement* newOffset = new TOffsetToVerLineMeasurement;
	string headCommentLine = "";
			
	TReal colSigma = LITERAL(1.0);
	bool noColSig = readColSigma(colSigma, i);
	if(noColSig)
	{
		project->getDataSet()->setDefaultOffsetToVerLineSig(true);
	}

	//read the offset measurements
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

			TAMeasurement::ECalcStatus status = TAMeasurement::kFixed;
			TSpatialPointName tgPointName;
			TLength obsOffset(LITERAL(0.0));
			TLength sigma(LITERAL(0.0));
			TLength cSigma;
			TLength zero(LITERAL(0.0));

			readPtName(tgPointName);

			// read the station
			readDistStation(*newDistSt);
			newDistSt->setInstHeightStatus(status);
			newDistSt->setInstrumentHeight(zero);

			readMeasurement(obsOffset);
			readSigma(sigma);

			//create the offset measurement
			TSpatialLineName tgLineName (tgPointName);
			TOffsetToVerLineMeasurement* newOffset = new TOffsetToVerLineMeasurement(tgLineName, obsOffset, sigma);
			//newOffset->setLineParameters(tgLineName, obsOffset, sigma);

			// set of the global sigma, if necessary
			cSigma.setMMetresValue(colSigma);
			newOffset->ifNotDoneSetSigma(cSigma);

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

			while (!(stream->peek()=='\n'))
			{
				switch (stream->peek())
				{
					case '/':
						readConstant(cte);
						break;

					case 'C':
						// constant to be computed
						stream->readChar();
						cte.setValue(zero);
						cte.setStatus(TAMeasurement::kVariable);
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
						while (!(stream->peek()=='\n')){
							stream->readChar();}
						break;
				}
				stream->skipWhiteSpace();
			}


			setId(newOffset, id, numOfMeas);
			setComments(newOffset, headCommentLine, comdb);
			addConstant2List(cte, project);
			newOffset->ifNotDoneSetMeasConst(cte);

			insert = insertOffsetToVerLineMeas(newDistSt, newOffset, project, i);

			if (insert)
			{
				project->getDataSet()->addToOffsetToVerLineNum();
			}
			delete newOffset;
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



bool TOffsetToVerLineConverter::insertOffsetToVerLineMeas(TDistMeasStation* newDistSt, TOffsetToVerLineMeasurement* newOffset,
														  TLGCProject* project, int i)
{
	char lineNum[9];
	string iLine, line;
	bool insert = false;

	PointIterator iterPtEnd = project->getDataSet()->getWorkingPoints()->getPointsEndIterator();
	DistStIterator iterStEnd = project->getDataSet()->getWorkingStations()->getDistStEndIterator();
	// TODO: changed this:
	//DistStIterator station = --iterStEnd;
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
	if (project->getDataSet()->getWorkingPoints()->getPoint(newDistSt->getStationedPoint()) != iterPtEnd)
	{
		if (project->getDataSet()->getWorkingPoints()->getPoint(newOffset->getTargetLineName().getFirstEndName()) != iterPtEnd)
		{
			// initialize the station with offset measurement
			newDistSt->addOffsetToVerLineMeas(newOffset);
			// add the station to the working stations list
			project->getDataSet()->getWorkingStations()->addDistStation(newDistSt);
			insert = true;
			
		}
		else
		{
			_itoa(i, lineNum, 10);
			iLine = lineNum;
			line = "line number " + iLine + " : ";
			string error = project->getError();
			error=error+line+ "Targeted Line doesn't exist" + '\n';
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




















/////////////////////////////////////////////////////////////////////////
//END
/////////////////////////////////////////////////////////////////////////

