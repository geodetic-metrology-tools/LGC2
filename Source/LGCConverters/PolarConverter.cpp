////////////////////////////////////////////////////////////////////
// THorAngleConverter.cpp
/*!
Write Horizontal Angle Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


#include "PolarConverter.h"
#include "TSpatialPoint.h"
#include "TAngle.h"
#include "TLength.h"

PolarConverter::PolarConverter(TAStreamFormatter& stream) : TObservationConverter(stream)
{
}

PolarMeasurement* PolarConverter::readObservation(TLGCProject* project, const TheodoliteTarget* currentTarget, TTheodolite* theo, int& i)
{
	string targetPoint;
	TAngle* observedAngle = new TAngle();
	TAngle* zenDValue = new TAngle();
	TLength* spaDistValue = new TLength();

	*fStream >> targetPoint;
	fStream->setAngleUnits(TAngle::kGons);
	*fStream >> *observedAngle;
	*fStream >> *zenDValue;
	fStream->setLengthUnits(TLength::kMetres);
	*fStream >> *spaDistValue;

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
	}
	
	const TLength* targetHeight = readOptionalValue(TLength::kMetres, "TH", target->getTargetHeight());
	const TLength* targetHeightSigma = readOptionalValue(TLength::kMillimetres, "THSE", target->getTargetHeightSigma());
	const TLength* targetCenteringSigma = readOptionalValue(TLength::kMillimetres, "TCSE", target->getTargetCenteringSigma());
	TAngle* angleSigma = readOptionalValue(TAngle::kCCs, "ASE", target->getAngleSigma());
	TAngle* zenithDistanceSigma = readOptionalValue(TAngle::kCCs, "ZSE", target->getZenithDistanceSigma());
	const TLength* spatialDistanceSigma = readOptionalValue(TLength::kMillimetres, "DSE", target->getDistanceSigma());
	const TLength* ppm = readOptionalValue(TLength::kMillimetres, "PPM", target->getPPM());
	
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

	PolarMeasurement* newMeas = new PolarMeasurement(project->getDataSet()->getObservationsCount()++,
		point, targetCenteringSigma, observedAngle, zenDValue, spaDistValue,
		angleSigma, zenithDistanceSigma, spatialDistanceSigma, ppm, target, targetHeight, targetHeightSigma);

	setComments(newMeas, headCommentLine, comdb);

	fStream->readLine();

	return newMeas;
}

void PolarConverter::writeResults(LSPolarIter	obsIt)
{
	// TODO: implement
}

void PolarConverter::writeResultsHeader()
{
	// TODO: implement
}

void PolarConverter::writeMesData(LSPolarIter	obsIt)
{
	// TODO: implement
}

void PolarConverter::writeReliabilityData(TLSCalcPolarObservation& obs)
{
	// TODO: implement
}

void PolarConverter::writeReliabilityHeader()
{
	// TODO: implement
}
