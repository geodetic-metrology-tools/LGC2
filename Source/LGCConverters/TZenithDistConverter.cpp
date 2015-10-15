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

#include "TZenithDistConverter.h"
#include "TSpatialPoint.h"
#include "TAngle.h"
#include "TLength.h"

TZenithDistConverter::TZenithDistConverter(TAStreamFormatter& stream) : TAngleObsConverter(stream)
{
}

TZenithDistMeasurement* TZenithDistConverter::readObservation(TLGCProject* project, const TheodoliteTarget* currentTarget, TTheodolite* theo, int& i)
{
	string targetPoint;
	TAngle* observedValue = new TAngle();

	*fStream >> targetPoint;
	fStream->setAngleUnits(TAngle::kGons);
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

	string targetID = readOptional("TRGT");

	const TheodoliteTarget* target;
	if (targetID == "")
	{
		target = currentTarget;
	}
	else
	{
		target = theo->getTargetNamed(targetID);

		if (target == NULL)
		{
			char lineNum[10];
			_itoa(i, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - Unknown theodolite target " + targetID + "\n");
			return NULL;
		}
	}

	TAngle* angleSigma = readOptionalValue(TAngle::kCCs, "OBSE", target->getZenithDistanceSigma());
	const TLength* targetHeight = readOptionalValue(TLength::kMetres, "TH", target->getTargetHeight());
	const TLength* targetHeightSigma = readOptionalValue(TLength::kMillimetres, "THSE", target->getTargetHeightSigma());
	const TLength* targetCenteringSigma = readOptionalValue(TLength::kMillimetres, "TCSE", target->getTargetCenteringSigma());
	
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

	TZenithDistMeasurement* newMeas = new TZenithDistMeasurement(project->getDataSet()->getObservationsCount()++,
		point, target, observedValue, angleSigma, targetHeight, targetHeightSigma, targetCenteringSigma);

	setComments(newMeas, headCommentLine, comdb);

	fStream->readLine();

	return newMeas;
}


void	TZenithDistConverter::writeResultsHeader(bool hasOwnInstrumentHeight)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"POINT"); //second point's Name
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured angle
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated angle
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset (cc)
	(*stream).writeString(obsResWidth,	"ECART"); //offset (mm)
	(*stream).writeString(obsResWidth,	"RES"); //offset/sigma
	(*stream).writeString(obsWidth,	"DISTANCE"); //distance
	(*stream).writeString(obsWidth,	"RALLONGE"); //Target's heigth
	if (hasOwnInstrumentHeight)
	{
		(*stream).writeString(obsWidth,	"IH"); //instrument height
	}
	(*stream)<<endl;	
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	"(GRAD)"); //mesured angle
	(*stream).writeString(obsResWidth,	"(CC)"); //sigma
	(*stream).writeString(obsWidth,	"(GRAD)"); //estimated angle
	(*stream).writeString(obsResWidth,	"(CC)"); //offset (cc)
	(*stream).writeString(obsResWidth,	"(MM)"); //offset (mm)
	(*stream).writeString(obsResWidth,	"/SIG"); //offset/sigma
	(*stream).writeString(obsWidth,	"(M)"); //distance
	(*stream).writeString(obsWidth,	"(M)"); //Target's heigth 
	if (hasOwnInstrumentHeight)
	{
		(*stream).writeString(obsWidth,	"(M)"); //instrument height
	}
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//third line
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	""); //mesured angle
	(*stream).writeString(obsResWidth,	""); //sigma
	(*stream).writeString(obsWidth,	""); //estimated angle
	(*stream).writeString(obsResWidth,	""); //offset (cc)
	(*stream).writeString(obsResWidth,	""); //offset (mm)
	(*stream).writeString(obsResWidth,	""); //offset/sigma
	(*stream).writeString(obsWidth,	"(M)"); //distance
	(*stream)<<endl<<endl;

	return;
}



void	TZenithDistConverter::writeResults(	LSZenDistConstIter obsIt)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResidualPrecision = getAngleResidualPrecision();
	int					lengthPrecision = getLengthPrecision();
	string				separator = getSeparator();

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

	//write the distance
	stream->setLengthUnits(TLength::kMetres);
	stream->setWidthFormat(obsWidth);
	(*stream)<<(obsIt->getStTgDistance())<<separator;

	//write Target's heigth ( (M) TLength)
	(*stream)<<(obsIt->getHPrism())<<separator;

	if (obsIt->hasOwnInstrumentHeight())
	{
		//write Instrument's heigth ( (M) TLength)
		(*stream)<<(obsIt->getHInstrument()->getEstimatedValue())<<separator;
	}
	*stream << endl;
	(*stream).setDataSpacing();

	return;
}


void	TZenithDistConverter::writeReliabilityData(TLSCalcZenithDistObservation& obs)
{
	this->TAngleObsConverter::writeReliabilityData(obs);
	return;
}


void	TZenithDistConverter::writeMesData(LSZenDistConstIter	obsIt)
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
	return;
}
