////////////////////////////////////////////////////////////////////
// TSpatialDistConverter.cpp
/*!
Write Spatial Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TSpatialDistConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"

#include	"TLGCProject.h"
/////////////////////////////////////////////////////////////////////

//ClassImp(TSpatialDistConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
//default constructor
TSpatialDistConverter::TSpatialDistConverter(TAStreamFormatter& stream, const TALSCalcParameter::ELSStatus status) :
TLengthObsConverter(stream)
{	
	fStatus = status;

	insertKeyChar(EOF);
	insertKeyChar('\n');
	insertKeyChar('/'); 
	insertKeyChar('\\');
	insertKeyChar('C');
	insertKeyChar('$');
	insertKeyChar('%');
}

//destructor
TSpatialDistConverter::~TSpatialDistConverter()
{}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TSpatialDistConverter::writeResultsHeader(const int)
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
	(*stream).writeString(obsResWidth,	"RES/SIG"); //offset/sigma
	(*stream).writeString(obsResWidth,	"SENSI"); //sensitivity
	(*stream).writeString(obsWidth,	"HAUTEUR"); //instrument's height 
	(*stream).writeString(obsWidth,	"HAUTEUR"); //prism's height 
	(*stream).writeString(obsResWidth,	"CONST"); //distance's constant 
	(*stream).writeString(obsResWidth,	"SCONST"); //sigma distance's constant 
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream).writeString(nameWidth,	""); //first point's Name
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	"(M)"); //mesured distance
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma
	(*stream).writeString(obsWidth,	"(M)"); //estimated distance
	(*stream).writeString(obsResWidth,	"(MM)"); //offset (mm)
	(*stream).writeString(obsResWidth,	""); //offset/sigma
	(*stream).writeString(obsResWidth,	"BILITE"); //sensitivity
	(*stream).writeString(obsWidth,	"D'INST"); //instrument's height 
	(*stream).writeString(obsWidth,	"DE PRISM"); //prism's height 
	(*stream).writeString(obsResWidth,	"(MM)"); //distance's constant 
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma distance's constant 
	(*stream)<<endl;
	
	///////////////////////////////////////////////////////////////////////////////////
	//Third line
	(*stream).writeString(nameWidth,	""); //first point's Name
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	""); //mesured distance
	(*stream).writeString(obsResWidth,	""); //sigma
	(*stream).writeString(obsWidth,	""); //estimated distance
	(*stream).writeString(obsResWidth,	""); //offset (mm)
	(*stream).writeString(obsResWidth,	""); //offset/sigma
	(*stream).writeString(obsResWidth,	"(MM/CM)"); //sensitivity
	(*stream).writeString(obsWidth,	"(M)"); //instrument's height 
	(*stream).writeString(obsWidth,	"(M)"); //prism's height 
	(*stream)<<endl<<endl;
	
	return;
}



void	TSpatialDistConverter::writeResults(LSSpaDistConstIter obsIt)
{

	if(obsIt->getHInstStatus() == fStatus)
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

		//write the offset / sigma (TDouble (MM))
		stream->setPrecisionFormat(2);
		(*stream)<<(obsIt->getResDivSig())<<separator;

		//write the sensitivity (TReal (MM/CM))
		(*stream)<<(obsIt->getSensitivity())<<separator;

		//write the instrument's height (TLength (M))
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
		(*stream)<<(obsIt->getEstHInst())<<separator;

		//write the target's height (TLength (M))
		(*stream)<<(obsIt->getHPrism())<<separator;

		//write the distance cste (TLength (MM))
		stream->setLengthUnits(TLength::kMillimetres);
		stream->setWidthFormat(obsResWidth);
		(*stream)<<(obsIt->getEstValueForVariableDistConst())<<separator;

		//write the distance cste sigma (TLength (MM))
		(*stream)<<(obsIt->getEstSigmaValueForVariableDistConst())<<separator<<endl;
		(*stream).setDataSpacing();
	}

	return;
}


/////////////////////////////////////////////////////////////////////////
//RELIABILITY
/////////////////////////////////////////////////////////////////////////
void	TSpatialDistConverter::writeReliabilityData(TLSCalcSpatialDistObservation& obs)
{
	if(obs.getHInstStatus() == fStatus)
	{
		this->TLengthObsConverter::writeReliabilityData(	obs,
															obs.getStPointName(),
															obs.getTgPointName(),
															"");
	}
	return;
}


void	TSpatialDistConverter::writeReliabilityHeader()
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
void	TSpatialDistConverter::writeMesData(LSSpaDistConstIter obsIt)
{
	
	if(obsIt->getHInstStatus() == fStatus)
	{
		TAngle gis, noV0;
		formatMesLine(	obsIt->getId(),
						obsIt->getStPointName(),
						obsIt->getTgPointName(),
						"",
						gis,
						obsIt->getCorrectedObsDist(), 
						obsIt->getCorrectedEstDist(), 
						obsIt->getResidue(),
						obsIt->getSigmaAPriori(),
						noV0, 
						obsIt->getComment());
	}
	return;
}



////////////////////////////////////////////////////////////////////////////////////////////
//READ FUNCTION
////////////////////////////////////////////////////////////////////////////////////////////
int TSpatialDistConverter::readObservation(TAMeasurement::ECalcStatus status, int i,
										int& numOfMeas, TLGCProject* project)
{
	TAStreamFormatter*	stream = getStream();

	char lineNum[9];
	string iLine, line, error;

	TDistConstants cte;
	TDistMeasStation* newDistSt = new TDistMeasStation;
//	TSpatialDistMeasurement* newSDist = new TSpatialDistMeasurement;
	string headCommentLine = "";
	bool firstLine = true;
			
	TReal colSigma = LITERAL(1.0), colSigmaPpm = LITERAL(0.0);
	bool noColSig = readColSigma(colSigma, colSigmaPpm, i);
	if(noColSig)
	{
		project->getDataSet()->setDefaultSpaDistSig(true, status);
	}

	//read the spatial distance measurements
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
			newDistSt->setInstHeightStatus(status);
			readPtName(tgName);
			readMeasurement(obsDist);
			readSigma(sigma, sigmaPpm);
			//create the spatial distance measurement
			TSpatialDistMeasurement* newSDist = new TSpatialDistMeasurement(tgName, obsDist, sigma);
			newSDist->ifNotDoneSetSigmaPpm(sigmaPpm);

			// set of the global sigma, if necessary
			TLength cSigma, cSigmaPpm;
			cSigma.setMMetresValue(colSigma);
			cSigmaPpm.setMMetresValue(colSigmaPpm);
			newSDist->ifNotDoneSetSigma(cSigma);
			newSDist->ifNotDoneSetSigmaPpm(cSigmaPpm);

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
			TLength hInst, hPrism, zero(LITERAL(0.0));
		
			while (!(stream->peek()=='\n')){

				char c = stream->peek();
				switch (c)
				{
					case '\t':
						stream->readChar();
						break;
					case '\\':					
						if (status == TAMeasurement::kFixed)
						{// instrument height
							stream->readChar();
							stream->TAStreamFormatter::operator>>(hInst);
							newDistSt->setInstrumentHeight(hInst);
							stream->skipWhiteSpace();

						// prism height
							if (!(stream->atEnd()) && stream->peek()!='\n'){
								stream->TAStreamFormatter::operator>>(hPrism);
							}
							else {
								_itoa(i, lineNum, 10);
								iLine = lineNum;
								line = "line number " + iLine + " : ";
								project->addWarning(line + "No Reflector Height (default value [LITERAL(0.00000)] assigned)");
								hPrism.setMetresValue(LITERAL(0.00000));
								
							}
						}
						else
						{
							stream->readChar();
							stream->TAStreamFormatter::operator>>(hPrism);
						}
						newSDist->setPrismHeight(hPrism);
						break;

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
							char cc = stream->readChar();}
						break;
				}
				stream->skipWhiteSpace();
			}

			setId(newSDist, id, numOfMeas);
			setComments(newSDist, headCommentLine, comdb);
			addConstant2List(cte, project);
			newSDist->ifNotDoneSetMeasConst(cte);

			if (firstLine == true && 
				project->getDataSet()->getWorkingPoints()->getPoint(newDistSt->getStationedPoint()) !=
				project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
			{
				// initialize the station with ROM and spatial distance measurement
				TSpatialDistROM* newSDRom = new TSpatialDistROM;
				newSDRom->addSpatialDist(newSDist);
				newDistSt->addSpatialDistROM(newSDRom);
				// add the station to the working stations list
				project->getDataSet()->getWorkingStations()->addDistStation(newDistSt);
				insert = true;
				delete newSDRom;
				firstLine = false;
			}
			else
			{
				insert = insertSpatialDistMeas(newDistSt, newSDist, project, i);
			}		

			if (insert)
			{
				project->getDataSet()->addToSpaDistNum(newDistSt);
			}

			delete newSDist;
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



bool TSpatialDistConverter::insertSpatialDistMeas( TDistMeasStation* newDistSt, TSpatialDistMeasurement* newSDist,
												   TLGCProject* project,int i )
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
		if (project->getDataSet()->getWorkingPoints()->getPoint(newSDist->getTargetPoint()) != iterEnd)
		{
			// if newDistSt is not the current station
			if (!(*newDistSt == *(station)))
			{
				// initialize the station with ROM and spatial distance measurement
				TSpatialDistROM* newSDRom = new TSpatialDistROM;
				newSDRom->addSpatialDist(newSDist);
				newDistSt->addSpatialDistROM(newSDRom);
				// add the station to the working stations list
				project->getDataSet()->getWorkingStations()->addDistStation(newDistSt);
				insert = true;
				delete newSDRom;
			}
			else // station is current
			{
				if (station->getSpatialDistROMDimension() == 0)
				{
					TSpatialDistROM* newSDRom = new TSpatialDistROM;
					newSDRom->addSpatialDist(newSDist);
					station->addSpatialDistROM(newSDRom);
					insert = true;
					delete newSDRom;
				}

				else
				{
					SpatialDistROMIterator ROM;
					ROM = --(station->getSpatialDistROMEndIterator());
					ROM->addSpatialDist(newSDist);
					insert = true;
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





/////////////////////////////////////////////////////////////////////////
//END
/////////////////////////////////////////////////////////////////////////




