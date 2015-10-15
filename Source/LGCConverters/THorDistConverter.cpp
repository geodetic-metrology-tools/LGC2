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
#include	"TSpatialDistMeasurement.h"

THorDistConverter::THorDistConverter(TAStreamFormatter& stream) : TLengthObsConverter(stream)
{
}

THorizontalDistMeas<TheodoliteTarget>* THorDistConverter::readObservation(TLGCProject* project, const TheodoliteTarget* currentTarget, TTheodolite* theo, int& i)
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
			project->setError(project->getError() + "Line: " + lineNum + " - Unknown Theodolite target " + targetID + "\n");
			return false;
		}
	}
	
	const TLength* sigma = readOptionalValue(TLength::kMillimetres, "OBSE", target->getDistanceSigma());
	const TLength* ppmE = readOptionalValue(TLength::kMillimetres, "PPM", target->getPPM());
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

	THorizontalDistMeas<TheodoliteTarget>* newMeas = new THorizontalDistMeas<TheodoliteTarget>(project->getDataSet()->getObservationsCount()++,
		point, target, observedValue, sigma, ppmE, targetCenteringSigma);

	setComments(newMeas, headCommentLine, comdb);

	fStream->readLine();

	return newMeas;
}


void	THorDistConverter::writeResultsHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"POINT"); //second point's Name
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured distance
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated distance
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset (mm)
	(*stream).writeString(obsResWidth,	"RES"); //offset/sigma
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
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
void	THorDistConverter::writeReliabilityData(TLSCalcHorDistObservation<TheodoliteTarget>& obs)
{
	this->TLengthObsConverter::writeReliabilityData(	obs,
														obs.getStPointName());
	return;
}


void	THorDistConverter::writeReliabilityHeader()
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

