////////////////////////////////////////////////////////////////////
//OffsetToVerticalPlaneConverter.cpp
/*!

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"OffsetToVerticalPlaneConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"
#include	"TLGCProject.h"
#include	"TheodoliteStationConverter.h"


OffsetToVerticalPlaneConverter::OffsetToVerticalPlaneConverter(TAStreamFormatter& stream) : TLengthObsConverter(stream)
{
}

OffsetToLineOrPlaneROM* OffsetToVerticalPlaneConverter::readObservations(int& lineNumber, TLGCProject* project)
{
	string scaleID;

	*fStream >> scaleID;

	Scale* scale = project->getDataSet()->getWorkingInstruments()->getScaleNamed(scaleID);
	if (scale == NULL)
	{
		char lineNum[10];
		_itoa(lineNumber, lineNum, 10);
		project->setError(project->getError() + "Line: " + lineNum + " - Unknown Scale " + scaleID + "\n");
		return NULL;
	}

	OffsetToLineOrPlaneROM* rom = new OffsetToLineOrPlaneROM();

	lineNumber++;

	TheodoliteStationConverter conv(*fStream);
	conv.readObservationROMWithScale<OffsetToLineOrPlaneROM, TOffsetToLineOrPlaneMeasurement, OffsetToVerticalPlaneConverter, void>
		(lineNumber, project, rom, scale, NULL);

	return rom;
}

TOffsetToLineOrPlaneMeasurement* OffsetToVerticalPlaneConverter::readObservation(TLGCProject* project, const Scale* currentScale, void* unused, int& i)
{
	string targetPoint;
	TLength* observedValue = new TLength();

	*fStream >> targetPoint;
	fStream->setLengthUnits(TLength::kMetres);
	*fStream >> *observedValue;

	PointIterator p = project->getDataSet()->getWorkingPoints()->getPoint(targetPoint);
	if (p == project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
	{
		char lineNum[10];
		_itoa(i, lineNum, 10);
		project->setError(project->getError() + "Line: " + lineNum + " - Unknown point " + targetPoint + "\n");
		return NULL;
	}
	TSpatialPoint* point = &*p;

	string scaleID = readOptional("INSTR");

	const Scale* instrument;
	if (scaleID == "")
	{
		instrument = currentScale;
	}
	else
	{
		instrument = project->getDataSet()->getWorkingInstruments()->getScaleNamed(scaleID);
	}

	if (instrument == NULL)
	{
		return NULL;
	}

	const TLength* observationSigma = readOptionalValue(TLength::kMillimetres, "OBSE", instrument->getSpatialDistanceSigma());
	const TLength* ppmE = readOptionalValue(TLength::kMillimetres, "PPM", instrument->getPPM());
	const TLength* instrumentCenteringSigma = readOptionalValue(TLength::kMillimetres, "ICSE", instrument->getInstrumentCenteringSigma());

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

	TOffsetToLineOrPlaneMeasurement* newMeas = new TOffsetToLineOrPlaneMeasurement(project->getDataSet()->getObservationsCount()++, point,
		instrument, observedValue, observationSigma, ppmE, instrumentCenteringSigma);

	setComments(newMeas, headCommentLine, comdb);

	fStream->readLine();

	return newMeas;
}


void	OffsetToVerticalPlaneConverter::writeResults(LSOffsetToVerPlaneConstIter	obsIt)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();

	//write Point
	(*stream).writeStringLeft(nameWidth, obsIt->getMeasuredPointName());
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

void	OffsetToVerticalPlaneConverter::writeInformation(const LSFreeVecIter& posVec)
{
	TAStreamFormatter*	stream = getStream();
	*stream << "Plane: a = " << posVec->getEstimatedValue().getX() << "; b = " << posVec->getEstimatedValue().getY()
		<< endl << endl;
}

void	OffsetToVerticalPlaneConverter::writeResultsHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"POINT"); // point's Name
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured offset
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated offset
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset (mm)
	(*stream).writeString(obsResWidth,	"RES"); //offset/sigma
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream).writeString(nameWidth,	""); // point's Name
	(*stream).writeString(obsWidth,	"(M)"); //mesured offset
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma
	(*stream).writeString(obsWidth,	"(M)"); //estimated offset
	(*stream).writeString(obsResWidth,	"(MM)"); //offset (mm)
	(*stream).writeString(obsResWidth,	"/SIG"); //offset/sigma
	(*stream)<<endl<<endl;
	
	return;
}

/////////////////////////////////////////////////////////////////////////
//RELIABILITY
/////////////////////////////////////////////////////////////////////////
void	OffsetToVerticalPlaneConverter::writeReliabilityData(TLSCalcOffsetToVerPlaneObservation& obs)
{
	this->TLengthObsConverter::writeReliabilityData(	obs,
														obs.getMeasuredPointName());
	return;
}


void	OffsetToVerticalPlaneConverter::writeReliabilityHeader()
{
	this->TObservationConverter::writeReliabilityHeader("STATION",
														"OBSERVE",
														"M",
														"MM");
	return;
}



/////////////////////////////////////////////////////////////////////////
//PUNCH MES
/////////////////////////////////////////////////////////////////////////
void	OffsetToVerticalPlaneConverter::writeMesData(LSOffsetToVerPlaneConstIter obsIt)
{
	// TODO: fix!
	/*TAngle gis, noV0;

	formatMesLine(	obsIt->getId(),
						obsIt->getFirstTgPointName(),
						obsIt->getStPointName(),
						obsIt->getSecondTgPointName(), 
						gis, 
						obsIt->getCorrectedObsDist(), 
						obsIt->getCorrectedEstDist(), 
						obsIt->getResidue(), 
						obsIt->getSigmaAPriori(), 
						noV0, 
						obsIt->getComment());*/
	return;
}
