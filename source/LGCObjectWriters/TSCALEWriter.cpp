#include "TSCALEWriter.h"
#include "TAMeas.h"
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include "Global.h"
#include "TLGCObsSummary.h"
#include "RoundOfMeasurements.h"
#include "TAdjustablePoint.h"

TSCALEWriter::TSCALEWriter(TAStreamFormatter& stream): TObservationWriter(stream)
{}

TSCALEWriter::~TSCALEWriter(){}

//Results
void TSCALEWriter::writeECHOResultsHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);


	////////////////////////////////////////////////////////////
	//First line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the scale
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured offset
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma DIST
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated offset 
	(*stream).writeString(obsResWidth,	"RESIDU"); //residual
	(*stream).writeString(obsResWidth,	"RES/SIG");    //residual/sigma
	(*stream).writeString(nameWidth,	"SCALE ID");    //scale ID
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //POSITION
	(*stream).writeString(obsWidth,	"(M)"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth,"(MM)"); //sigma observed value
	(*stream).writeString(obsWidth,	"(M)"); //estimated offset
	(*stream).writeString(obsResWidth,	"(MM)"); //residual
	(*stream).writeString(obsResWidth,	"");    //residual/sigma
	(*stream).writeString(nameWidth,	"");    //scale ID

	(*stream)<<endl;
}

void TSCALEWriter::writeECSPResultsHeader()
{}

void TSCALEWriter::writeECVEResultsHeader()
{}

void TSCALEWriter::writeECHOResults(const  TECHOROM& echorom)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();
	std::string         TABs1 = stream->getCurrSpaceExtended(1);
	std::string         TABs3 = stream->getCurrSpaceExtended(3);

	//Precision fro MM value
	int lengthResPrecision = lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0;

	//first line
	(*stream)<<TABs1<<"ECHO MEASUREMENTS"<<endl;

	///////////////////////////////////////////////////////////////////////////////////
	//first line
	(*stream)<<TABs1;
	(*stream).writeStringLeft(nameWidth,"REF POINT"); //Reference point
	(*stream).writeString(nameWidth,"X"); 
	(*stream).writeString(nameWidth,"Y");
	(*stream).writeString(nameWidth,"Z");
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs1;
	(*stream).writeStringLeft(nameWidth,echorom.fMeasuredPlane->getReferencePoint()->getName()); //Reference point
	(*stream).writeDouble(obsWidth,obsWidth, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(0));
	(*stream).writeDouble(obsWidth,obsWidth, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(1));
	(*stream).writeDouble(obsWidth,obsWidth, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(2));
	(*stream)<<endl<<endl;
	///////////////////////////////////////////////////////////////////////////////////

	//data summury
	this->writeObsTitle(TABs3 + this->getObsDescriptionEN(TALGCObjectWriter::kECHO), echorom.measECHO.size());
	(*stream)<<endl;
	
	writeECHOResultsHeader(); // write the title line for the observations

	for(auto const& ItECHO : echorom.measECHO)
	{
		(*stream)<<TABs3;
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItECHO.targetPos->getName());

		//write the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECHO.getDistance());//Output value in meters [m], stored in [m]

		//write the sigma DIST
      (*stream).writeDouble(obsResWidth, lengthResPrecision, ItECHO.target.sigmaD.getMMetresValue());//Output value in milimeters [mm], stored in [m]

		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECHO.getDistance() + ItECHO.getDistanceResidual());//Output value in meters [m], stored in [m]

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECHO.getDistanceResidual() * M2MM);//Output value in milimeters [mm], stored in [m]

		//write the residual/sigam
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECHO.getDistanceResidual()/ItECHO.target.sigmaD);//Output value in meters [m], stored in [m]

		//write the scale ID
		(*stream).writeString(nameWidth, ItECHO.target.ID);
		(*stream)<<endl;
	}
	(*stream)<<endl;

	writeDistanceResultsSummary(echorom.getECHOObsSummary(), TABs3);
}


void TSCALEWriter::writeECHOSIMUResults(const  TECHOROM& echorom)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kECHO), echorom.measECHO.size());
	(*stream)<<endl;

	///////////////////////////////////////////////////////////////////////////////////
	//first line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"REF POINT"); //Reference point
	(*stream).writeString(nameWidth,"X"); 
	(*stream).writeString(nameWidth,"Y");
	(*stream).writeString(nameWidth,"Z");
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,echorom.fMeasuredPlane->getReferencePoint()->getName()); //Reference point
	(*stream).writeDouble(obsWidth,obsWidth, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(0));
	(*stream).writeDouble(obsWidth,obsWidth, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(1));
	(*stream).writeDouble(obsWidth,obsWidth, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(2));
	(*stream)<<endl<<endl;
	///////////////////////////////////////////////////////////////////////////////////


	writeDistanceResultsSummary(echorom.getECHOObsSummary(), stream->getCurrSpaceExtended(3));
}

void TSCALEWriter::writeECSPResults()
{}

void TSCALEWriter::writeECVEResults()
{}



//Reliability 
void	TSCALEWriter::writeECHOReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("PT REF","STATION", "", "OBSERVATION", "M", "MM");
	return;
}

void	TSCALEWriter::writeECVEReliabilityHeader()
{
this->TObservationWriter::writeReliabilityHeader("PT REF","STATION", "", "OBSERVATION", "M", "MM");
	return;
}

void	TSCALEWriter::writeECSPReliabilityHeader()
{
this->TObservationWriter::writeReliabilityHeader("PT REF","STATION", "", "OBSERVATION", "M", "MM");
	return;
}

void	TSCALEWriter::writeECHOReliabilityData(const  TECHOROM& echorom, const TLGCStatistic& stat, const std::vector<TECHO> measECHO)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	string				separator = getSeparator();

	//Precision fro MM value
	int lengthResPrecision = lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0;

	//For each ECHO measurement of the station
	for(auto const& ItEcho : measECHO)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItEcho.getFirstObservationIndex();

		// get Ref  Point 
		(*stream).writeStringLeft(nameWidth, echorom.fMeasuredPlane->getReferencePoint()->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItEcho.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision,ItEcho.getDistance());
		//get the standard deviation
      (*stream).writeDouble(obsResWidth, lengthResPrecision, ItEcho.target.sigmaD.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItEcho.getDistanceResidual()* M2MM);


		writeReliability(index, stat);
		(*stream).setDataSpacing();
	}
	return;
}

void	TSCALEWriter::writeECVEReliabilityData()
{}

void	TSCALEWriter::writeECSPReliabilityData()
{}