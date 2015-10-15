////////////////////////////////////////////////////////////////////
//OffsetToSpatialLineConverter.cpp
/*!

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"OffsetToSpatialLineROM.h"
#include	"OffsetToSpatialLineConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"
#include	"TLGCProject.h"
#include	"TheodoliteStationConverter.h"
#include	"OffsetToVerticalPlaneConverter.h"


OffsetToSpatialLineConverter::OffsetToSpatialLineConverter(TAStreamFormatter& stream) : TLengthObsConverter(stream)
{
}

OffsetToSpatialLineROM* OffsetToSpatialLineConverter::readObservations(int& lineNumber, TLGCProject* project)
{
	string scaleID;
	string firstPointOnLine = "";
	string secondPointOnLine = "";
	string temp;

	*fStream >> temp;

	if (!isComment() && fStream->peek() != '\n')
	{
		firstPointOnLine = temp;
		*fStream >> secondPointOnLine;
		*fStream >> scaleID;
	}
	else
	{
		scaleID = temp;
	}

	Scale* scale = project->getDataSet()->getWorkingInstruments()->getScaleNamed(scaleID);
	if (scale == NULL)
	{
		char lineNum[10];
		_itoa(lineNumber, lineNum, 10);
		project->setError(project->getError() + "Line: " + lineNum + " - Unknown Scale " + scaleID + "\n");
		return NULL;
	}

	TSpatialPoint* fst = NULL;
	TSpatialPoint* snd = NULL;
	if (firstPointOnLine != "")
	{
		PointIterator p = project->getDataSet()->getWorkingPoints()->getPoint(firstPointOnLine);
		if (p == project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
		{
			char lineNum[10];
			_itoa(lineNumber, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - Unknown point " + firstPointOnLine + "\n");
			return NULL;
		}
		fst = &*p;
		p = project->getDataSet()->getWorkingPoints()->getPoint(secondPointOnLine);
		if (p == project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
		{
			char lineNum[10];
			_itoa(lineNumber, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - Unknown point " + secondPointOnLine + "\n");
			return NULL;
		}
		snd = &*p;
	}

	fStream->readLine();

	OffsetToSpatialLineROM* rom = new OffsetToSpatialLineROM(fst, snd);

	lineNumber++;

	TheodoliteStationConverter conv(*fStream);
	if (!conv.readObservationROMWithScale<OffsetToSpatialLineROM, TOffsetToLineOrPlaneMeasurement, OffsetToVerticalPlaneConverter, void>
		(lineNumber, project, rom, scale, NULL))
	{
		delete rom;
		return NULL;
	}
	
	project->getLSCalcDataSet()->setIsCombinedCase();

	return rom;
}

void OffsetToSpatialLineConverter::writeResults(LSOffsetToSpaLineConstIter	obsIt)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();


	//write Point
	(*stream).writeStringLeft(nameWidth, obsIt->getTgName());
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

void OffsetToSpatialLineConverter::writeInformation(const LSPosVecIter& ptOnLine, const LSFreeVecIter& unit)
{
	int					obsWidth = getObsWidth();
	int					lengthPrecision =	getLengthPrecision();
	TAStreamFormatter*	stream = getStream();
	stream->setLengthUnits(TLength::kMetres);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);
	*stream << "Point on line: X = " << ptOnLine->getXEstValue() << "; Y = " << ptOnLine->getYEstValue()
		<< "; Z = " << ptOnLine->getZEstValue() << endl;

	*stream << "Line vector:   X = " << unit->getEstimatedValue().getX() << "; Y = " << unit->getEstimatedValue().getY()
		<< "; Z = " << unit->getEstimatedValue().getZ() << endl << endl;
}

void OffsetToSpatialLineConverter::writeResultsHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"POINT"); //point's Name
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
}

void OffsetToSpatialLineConverter::writeMesData(LSOffsetToSpaLineConstIter	obsIt)
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
						obsIt->getComment());
	return;*/
}

void OffsetToSpatialLineConverter::writeReliabilityData(TLSCalcOffsetToVerticalLineObservation& obs)
{
	this->TLengthObsConverter::writeReliabilityData(	obs,
														obs.getTgName());
	return;
}

void OffsetToSpatialLineConverter::writeReliabilityHeader()
{
	this->TObservationConverter::writeReliabilityHeader("STATION",
														"OBSERVE",
														"M",
														"MM");
	return;
}
