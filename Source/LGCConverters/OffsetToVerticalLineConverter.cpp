////////////////////////////////////////////////////////////////////
//OffsetToVerticalLineConverter.cpp
/*!

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"OffsetToVerticalLineConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"
#include	"TLGCProject.h"
#include	"TheodoliteStationConverter.h"
#include	"OffsetToVerticalPlaneConverter.h"


OffsetToVerticalLineConverter::OffsetToVerticalLineConverter(TAStreamFormatter& stream) : TLengthObsConverter(stream)
{
}

OffsetToVerticalLineROM* OffsetToVerticalLineConverter::readObservations(int& lineNumber, TLGCProject* project)
{
	string scaleID;
	string stationPoint = "";
	string temp;

	*fStream >> temp;

	if (!isComment() && fStream->peek() != '\n')
	{
		stationPoint = temp;
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

	TSpatialPoint* sp = NULL;
	if (stationPoint != "")
	{		
		PointIterator p = project->getDataSet()->getWorkingPoints()->getPoint(stationPoint);
		if (p == project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
		{
			char lineNum[10];
			_itoa(lineNumber, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - Unknown point " + stationPoint + "\n");
			return NULL;
		}
		sp = &*p;
	}

	OffsetToVerticalLineROM* rom = new OffsetToVerticalLineROM(sp);

	lineNumber++;
	fStream->readLine();

	TheodoliteStationConverter conv(*fStream);
	conv.readObservationROMWithScale<OffsetToVerticalLineROM, TOffsetToLineOrPlaneMeasurement, OffsetToVerticalPlaneConverter, void>
		(lineNumber, project, rom, scale, NULL);
	
	project->getLSCalcDataSet()->setIsCombinedCase();

	return rom;
}

void OffsetToVerticalLineConverter::writeResults(LSOffsetToVerLineConstIter	obsIt)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();

	//write Point 2
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
	(*stream)<<(obsIt->getResDivSig())<<separator<<endl << endl;
	(*stream).setDataSpacing();
}

void OffsetToVerticalLineConverter::writeInformation(const LSPosVecIter& posVec)
{
	TAStreamFormatter*	stream = getStream();
	int					obsWidth = getObsWidth();
	int					lengthPrecision =	getLengthPrecision();
	stream->setLengthUnits(TLength::kMetres);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);
	*stream << "Plomb: X = " << posVec->getXEstValue() << "; Y = " << posVec->getYEstValue()
		<< "; Z = " << posVec->getZEstValue() << endl << endl;
}

void OffsetToVerticalLineConverter::writeResultsHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"POINT"); //second point's Name
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured offset
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated offset
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset (mm)
	(*stream).writeString(obsResWidth,	"RES"); //offset/sigma
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	"(M)"); //mesured offset
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma
	(*stream).writeString(obsWidth,	"(M)"); //estimated offset
	(*stream).writeString(obsResWidth,	"(MM)"); //offset (mm)
	(*stream).writeString(obsResWidth,	"/SIG"); //offset/sigma
	(*stream)<<endl<<endl;
}

void OffsetToVerticalLineConverter::writeMesData(LSOffsetToVerLineConstIter	obsIt)
{
	// TODO: fix!
	/*TAngle gis, noV0;
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
	return;*/
}

void OffsetToVerticalLineConverter::writeReliabilityData(TLSCalcOffsetToVerticalLineObservation& obs)
{
	this->TLengthObsConverter::writeReliabilityData(	obs,
														obs.getTgName());
	return;
}

void OffsetToVerticalLineConverter::writeReliabilityHeader()
{
	this->TObservationConverter::writeReliabilityHeader("STATION",
														"OBSERVE",
														"M",
														"MM");
	return;
}
