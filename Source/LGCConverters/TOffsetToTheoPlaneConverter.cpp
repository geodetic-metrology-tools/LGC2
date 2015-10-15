////////////////////////////////////////////////////////////////////
// TOffsetToTheoPlaneConverter.cpp
/*!
Write Offset to a Theodolite Plane Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TOffsetToTheoPlaneConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"

#include	"TLGCProject.h"
/////////////////////////////////////////////////////////////////////

//ClassImp(TOffsetToTheoPlaneConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
//default constructor
TOffsetToTheoPlaneConverter::TOffsetToTheoPlaneConverter(TAStreamFormatter& stream) : TLengthObsConverter(stream)
{
}

TOffsetToTheoPlaneMeasurement* TOffsetToTheoPlaneConverter::readObservation(TLGCProject* project, const Scale* currentScale, TAngle* angle, int& i)
{
	string stationPoint;
	TLength* observedValue = new TLength();

	*fStream >> stationPoint;
	fStream->setLengthUnits(TLength::kMetres);
	*fStream >> *observedValue;

	PointIterator p = project->getDataSet()->getWorkingPoints()->getPoint(stationPoint);
	if (p == project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
	{
		char lineNum[10];
		_itoa(i, lineNum, 10);
		project->setError(project->getError() + "Line: " + lineNum + " - Unknown point " + stationPoint + "\n");
		return NULL;
	}
	TSpatialPoint* point = &*p;

	string instrumentID = readOptional("INSTR");

	const Scale* scale;
	if (instrumentID == "")
	{
		scale = currentScale;
	}
	else
	{
		scale = project->getDataSet()->getWorkingInstruments()->getScaleNamed(instrumentID);
		if (scale == NULL)
		{
			char lineNum[10];
			_itoa(i, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - Unknown Scale " + instrumentID + "\n");
			return NULL;
		}
	}

	const TLength* sigma = readOptionalValue(TLength::kMillimetres, "OBSE", scale->getSpatialDistanceSigma());
	const TLength* ppmE = readOptionalValue(TLength::kMillimetres, "PPM", scale->getPPM());
	const TLength* instrumentCenteringSigma = readOptionalValue(TLength::kMillimetres, "ICSE", scale->getInstrumentCenteringSigma());
	
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

	TOffsetToTheoPlaneMeasurement* newMeas = new TOffsetToTheoPlaneMeasurement(project->getDataSet()->getObservationsCount()++,
		point, scale, angle, observedValue, sigma, ppmE, instrumentCenteringSigma);

	setComments(newMeas, headCommentLine, comdb);

	fStream->readLine();

	return newMeas;
}

void	TOffsetToTheoPlaneConverter::writeResultsHeader(bool hasOwnV0)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"POINT 2"); //second point's Name
	(*stream).writeString(obsWidth,	"DIRECTION"); //direction
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured offset
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated offset
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset (mm)
	(*stream).writeString(obsResWidth,	"RES"); //offset/sigma
	if (hasOwnV0)
	{
		(*stream).writeString(obsWidth,	"V0"); //V0
	}
	(*stream)<<endl;	

	/////////////////////////////////////////////////////////////////
	//second line
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	"(GRAD)"); //direction
	(*stream).writeString(obsWidth,	"(M)"); //mesured offset
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma
	(*stream).writeString(obsWidth,	"(M)"); //estimated offset
	(*stream).writeString(obsResWidth,	"(MM)"); //offset (mm)
	(*stream).writeString(obsResWidth,	"/SIG"); //offset/sigma
	if (hasOwnV0)
	{
		(*stream).writeString(obsWidth,	"(GRAD)"); //V0
	}
	(*stream)<<endl<<endl;	
	
	return;
}



void	TOffsetToTheoPlaneConverter::writeResults(LSOffsetToTheoPlaneConstIter obsIt)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	int					anglePrecision = getAnglePrecision();
//	int					angleResidualPrecision = getAngleResidualPrecision();
	string				separator = getSeparator();


	//write Point 2
	(*stream).writeStringLeft(nameWidth, obsIt->getStPointName());
	//write theo angle
	stream->setAngleUnits(TAngle::kGons);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(anglePrecision);
	(*stream)<<(obsIt->getTgAngle())<<separator;

	//write the observed offset
	stream->setLengthUnits(TLength::kMetres);
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
	(*stream)<<(obsIt->getResDivSig())<<separator;

	if (obsIt->hasOwnV0())
	{
		//V0
		stream->setWidthFormat( obsWidth );
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<<obsIt->getEstV0()<<separator;
	}
	*stream << endl;
	(*stream).setDataSpacing();

	return;
}


/////////////////////////////////////////////////////////////////////////
//RELIABILITY
/////////////////////////////////////////////////////////////////////////
void	TOffsetToTheoPlaneConverter::writeReliabilityData(TLSCalcOffsetToTheoPlaneObservation& obs)
{
	this->TLengthObsConverter::writeReliabilityData(	obs,
														obs.getTgPointName());
	return;
}


void	TOffsetToTheoPlaneConverter::writeReliabilityHeader()
{
	this->TObservationConverter::writeReliabilityHeader("TARGET",
														"OBSERVE",
														"M",
														"MM");
	return;
}



/////////////////////////////////////////////////////////////////////////
//PUNCH MES
/////////////////////////////////////////////////////////////////////////
void	TOffsetToTheoPlaneConverter::writeMesData(LSOffsetToTheoPlaneConstIter obsIt)
{	
	formatMesLine(	obsIt->getId(),
					obsIt->getTgPointName(),
					obsIt->getStPointName(),
					"",
					obsIt->getTgAngle(),
					obsIt->getCorrectedObsDist(),
					obsIt->getCorrectedEstDist(),
					obsIt->getResidue(),
					obsIt->getSigmaAPriori(),
					obsIt->getEstV0(),
					obsIt->getComment());
	return;
}
