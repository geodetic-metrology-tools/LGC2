////////////////////////////////////////////////////////////////////
// TZenithDistConverter.cpp
/*!
Write Zenithal Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TZenithDistConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"

#include	"TLGCProject.h"

/////////////////////////////////////////////////////////////////////

//ClassImp(TZenithDistConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
//default constructor
TZenithDistConverter::TZenithDistConverter(TAStreamFormatter& stream,
										   const TALSCalcParameter::ELSStatus status) :
TAngleObsConverter(stream)
{
	fStatus = status;

	insertKeyChar(EOF);
	insertKeyChar('\n');
	insertKeyChar('/');
	insertKeyChar('\\');
	insertKeyChar('$') ; insertKeyChar('%'); /*comments*/

}

//destructor
TZenithDistConverter::~TZenithDistConverter()
{}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TZenithDistConverter::writeResultsHeader(const int)
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
	(*stream).writeString(obsWidth,	"HAUTEUR"); //Instrument's heigth
	(*stream).writeString(obsWidth,	"RALLONGE"); //Target's heigth
	(*stream)<<endl;	
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream).writeString(nameWidth,	""); //first point's Name
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	"(GRAD)"); //mesured angle
	(*stream).writeString(obsResWidth,	"(CC)"); //sigma
	(*stream).writeString(obsWidth,	"(GRAD)"); //estimated angle
	(*stream).writeString(obsResWidth,	"(CC)"); //offset (cc)
	(*stream).writeString(obsResWidth,	"(MM)"); //offset (mm)
	(*stream).writeString(obsResWidth,	"/SIG"); //offset/sigma
	(*stream).writeString(obsWidth,	"D'INSTR"); //Instrument's heigth 
	(*stream).writeString(obsWidth,	"(M)"); //Target's heigth 
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//third line
	(*stream).writeString(nameWidth,	""); //first point's Name
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	""); //mesured angle
	(*stream).writeString(obsResWidth,	""); //sigma
	(*stream).writeString(obsWidth,	""); //estimated angle
	(*stream).writeString(obsResWidth,	""); //offset (cc)
	(*stream).writeString(obsResWidth,	""); //offset (mm)
	(*stream).writeString(obsResWidth,	""); //offset/sigma
	(*stream).writeString(obsWidth,	"(M)"); //Instrument's heigth 
	(*stream)<<endl<<endl;

	return;
}



bool TZenithDistConverter::writeResults(LSZenDistConstIter obsIt)
{
	bool willWrite(obsIt->getHInstrStatus() == fStatus);

	if(willWrite)
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
		(*stream)<<(obsIt->getObsAngle())<<(separator);

		//write the sigma
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(angleResidualPrecision);
		(*stream)<<(obsIt->getSigmaAPriori())<<(separator);

		//write the estimated angle
		stream->setAngleUnits(TAngle::kGons);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<<(obsIt->getEstimatedAngle())<<(separator);

		//write the offset (cc) after calculation
		stream->setWidthFormat(obsResWidth);
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setPrecisionFormat(angleResidualPrecision);
		TAngle residue = obsIt->getResidue();
		(*stream)<<(residue)<<separator;

		//write the offset after calculation ((MM) TLength)
		stream->setLengthUnits(TLength::kMillimetres);
		stream->setPrecisionFormat(lengthPrecision);
		(*stream)<<(obsIt->getTgMovement())<<separator;

		//write the offset / sigma (TDouble)
		stream->setPrecisionFormat(1);
		(*stream)<<(obsIt->getResDivSig())<<separator;

		//write Instrument's heigth ( (M) TLength)
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
		(*stream)<<(obsIt->getEstimatedHInstr())<<separator;

		//write Target's heigth ( (M) TLength)
		(*stream)<<(obsIt->getHPrism())<<separator<<endl;
		(*stream).setDataSpacing();
	}

	return willWrite;
}


void	TZenithDistConverter::writeReliabilityData(TLSCalcZenithDistObservation& obs)
{
	if(obs.getHInstrStatus() == fStatus)
	{
		this->TAngleObsConverter::writeReliabilityData(obs);
	}
	return;
}


void	TZenithDistConverter::writeMesData(LSZenDistConstIter	obsIt)
{
	if(obsIt->getHInstrStatus() == fStatus)
	{
		TAngle noV0;
		formatMesLine(	obsIt->getId(),
						obsIt->getStPointName(),
						obsIt->getTgPointName(),
						"",
						obsIt->getObsAngle(),
						obsIt->getEstimatedAngle(),
						obsIt->getResidue(),
						obsIt->getSigmaAPriori(),
						noV0,
						obsIt->getComment());
	}
	return;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//READ FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
int TZenithDistConverter::readObservation(TAMeasurement::ECalcStatus status, int i,
										  int& numOfMeas, TLGCProject* project)
{
	TAStreamFormatter*	stream = getStream();

	char lineNum[9];
	string iLine, line, error;

	TTheodoliteStation* newThSt = new TTheodoliteStation;
	string headCommentLine = "";
	bool firstLine = true;
		
	TReal colSigma = LITERAL(0.0001);
	bool noColSig = readColSigma(colSigma, i, project);
	if(noColSig)
	{
		project->getDataSet()->setDefaultZenDistSig(true, status);
	}


	//read the zenithal distances measurements
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

			// read the station
			readTheoStation(*newThSt);
			newThSt->setInstHeightStatus(status);
			if(status == TAMeasurement::kVariable)
			{
				TLength hiProvisionnal (LITERAL(0.2));
				newThSt->setInstrumentHeight(hiProvisionnal);
			}
			stream->skipWhiteSpace();
		
			readMeasurement(tgName, obsAngle, sigma);
			TZenithDistMeasurement* newZDist = new TZenithDistMeasurement(tgName, obsAngle, sigma);

			// set of the global sigma, if necessary
			cSigma.setGonsValue(colSigma);
			newZDist->ifNotDoneSetSigma(cSigma);

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


			//read the instrument and prism heights
			TLength hInst, hPrism;
			int id=-1;
			string EOLComments="";
			string comdb="";
		
			while (!(stream->peek()=='\n'))
			{
				char c = stream->peek();
				switch (c)
				{
					case '\t':
						stream->readChar();
						break;
					case '/':
						stream->readChar();
						// instrument height
						if (status == TAMeasurement::kFixed)
						{
							stream->TAStreamFormatter::operator>>(hInst);
							newThSt->setInstrumentHeight(hInst);
						}
						else
						// prism height
						{
							stream->TAStreamFormatter::operator>>(hPrism);
							newZDist->setPrismeHeight(hPrism);
						}
						break;

					case '\\':
					
						stream->readChar();
						if (status == TAMeasurement::kFixed)
						{// prism height
							stream->TAStreamFormatter::operator>>(hPrism);
							newZDist->setPrismeHeight(hPrism);
						}
						else
						// error
						{
							_itoa(i, lineNum, 10);
							iLine = lineNum;
							line = "line number " + iLine + " : ";
							string error = project->getError();
							error=error+line+ "Syntax error in setting Instrument/Prism height" + '\n';
							project->setError(error);
						}
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
	
			setComments(newZDist, headCommentLine, comdb);
			setId(newZDist, id, numOfMeas);



			if (firstLine == true&& 
				project->getDataSet()->getWorkingPoints()->getPoint(newThSt->getStationedPoint()) != project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
			{
				// initialize the station with ROM and horizontal angle measurement
				TZenithDistROM* newZDRom = new TZenithDistROM;
				newZDRom->addZenithDist(newZDist);
				newThSt->addZenithDistROM(newZDRom);
				// add the station to the working stations list
				project->getDataSet()->getWorkingStations()->addTheodStation(newThSt);
				insert = true;
				delete newZDRom;
				firstLine = false;
			}
			else
			{
				insert = insertZenithDistMeas(newThSt, newZDist, project, i);
			}

			if(insert)
			{
				project->getDataSet()->addToZenDistNum(newThSt);
			}
			delete newZDist;
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


bool TZenithDistConverter::insertZenithDistMeas( TTheodoliteStation* newThSt,
												TZenithDistMeasurement* newZDist,
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
		if (project->getDataSet()->getWorkingPoints()->getPoint(newZDist->getTargetPoint()) != iterEnd)
		{
			if (project->getDataSet()->getWorkingStations()->numberOfTheodStations() == 0)
			{
				// initialize the station with ROM and horizontal angle measurement
				TZenithDistROM* newZDRom = new TZenithDistROM;
				newZDRom->addZenithDist(newZDist);
				newThSt->addZenithDistROM(newZDRom);
				// add the station to the working stations list
				project->getDataSet()->getWorkingStations()->addTheodStation(newThSt);
				insert = true;
				delete newZDRom;
			}
			else
			{
				// if newDistSt is not the current station
				if (!(*newThSt == *(station)))
				{
					// initialize the station with ROM and horizontal angle measurement
					TZenithDistROM* newZDRom = new TZenithDistROM;
					newZDRom->addZenithDist(newZDist);
					newThSt->addZenithDistROM(newZDRom);
					// add the station to the working stations list
					project->getDataSet()->getWorkingStations()->addTheodStation(newThSt);
					insert = true;
					delete newZDRom;
				}
				else // station is current
				{
					if ((!(station->getInstrumentHeight() == newThSt->getInstrumentHeight())
						|| !(station->getInstHeightStatus() == newThSt->getInstHeightStatus()))
						&& newThSt->getInstrumentHeight().getMetresValue() != LITERAL(0.0))
					{
						_itoa(i, lineNum, 10);
						iLine = lineNum;
						line = "line number " + iLine + " : ";
						string error = project->getError();
						error=error+line+ "Instrument height already set" + '\n';
						project->setError(error);
					}


					if (station->getZenithDistROMDimension() == 0)
					{
						TZenithDistROM* newZDRom = new TZenithDistROM;
						newZDRom->addZenithDist(newZDist);
						station->addZenithDistROM(newZDRom);
						insert = true;
						delete newZDRom;
					}

					else
					{
						ZenithDistROMIterator ROM;
						ROM = --(station->getZenithDistROMEndIterator());
						ROM->addZenithDist(newZDist);
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
		error=error+line+ "Stationed Point doesn't exist" + '\n';
		project->setError(error);
	}
	return insert;
}












