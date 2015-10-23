#include "TOtherMeasurementsWriter.h"
#include "TAMeas.h"
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include "TLGCObsSummary.h"
#include "TAdjustablePoint.h"

TOtherMeasurentWriter::TOtherMeasurentWriter(TAStreamFormatter& stream): TObservationWriter(stream)
{}

TOtherMeasurentWriter::~TOtherMeasurentWriter()
{}

void	TOtherMeasurentWriter::writeDVERReliabilityHeader()
{
this->TObservationWriter::writeReliabilityHeader("POINT 1","POINT 2", "", "OBSERVATION", "M", "MM");
	return;
}

void	TOtherMeasurentWriter::writeDVERReliabilityData(const std::vector<TDVER>& fDVER, const TLGCStatistic& stat)
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
	for(auto const& ItDver : fDVER)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItDver.getFirstObservationIndex();

		// get Point 1 of the line or the linenumber?
		(*stream).writeStringLeft(nameWidth, ItDver.station->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItDver.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision,ItDver.getDistance());
		//get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDver.getObservedStDev()* M2MM);
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDver.getDistanceResidual()* M2MM);


		writeReliability(index, stat);
		(*stream).setDataSpacing();
	}
	return;
}

void TOtherMeasurentWriter::writeDVERResultsHeader()
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
	(*stream).writeStringLeft(nameWidth, "POINT 1"); //Position of the inst
	(*stream).writeStringLeft(nameWidth, "POINT 2"); //Position of the scale
	(*stream).writeString(obsWidth,	"OBSERVE"); //observed dver
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma 
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated dver 
	(*stream).writeString(obsResWidth,	"RESIDU"); //residual
	(*stream).writeString(obsResWidth,	"RES/SIG");//residual/sigma
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //Position of the inst 
	(*stream).writeStringLeft(nameWidth,""); //Position of the scale
	(*stream).writeString(obsWidth,	"(M)"); //observed dver
	(*stream).writeString(obsResWidth,"(MM)"); //sigma observed value
	(*stream).writeString(obsWidth,	"(M)"); //estimated dver
	(*stream).writeString(obsResWidth,	"(MM)"); //residual
	(*stream).writeString(obsResWidth,	"");    //residual/sigma

	(*stream)<<endl;
}

void TOtherMeasurentWriter::writeDVERResults(const std::vector<TDVER>& fDVER)
{
TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	//Precision fro MM value
	int lengthResPrecision = lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0;

	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kDVER), fDVER.size());
	(*stream)<<endl;

	writeDVERResultsHeader(); // write the title line for the observations

	//for output residual mean and the standart deviation of the residuals
	// directly calculate here due to the instrument absence
	TLGCObsSummary summary;

	for(auto const& ItDVER : fDVER)
	{
		(*stream)<<TABs;
		//write INST POSITION
		(*stream).writeStringLeft(nameWidth, ItDVER.station->getName());
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItDVER.targetPos->getName());

		//write the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDVER.getDistance());//Output value in meters [m], stored in [m]

		//write the sigma DIST
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDVER.getObservedStDev() * M2MM);//Output value in meters [mm], stored in [m]

		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDVER.getDistance() + ItDVER.getDistanceResidual());//Output value in meters [m], stored in [m]

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDVER.getDistanceResidual() * M2MM);//Output value in meters [mm], stored in [m]

		//write the residual/sigam
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDVER.getDistanceResidual()/ItDVER.getObservedStDev());//Output value in meters [mm], stored in [m]
		(*stream)<<endl;

		//for output residual mean and the standart deviation of the residuals
		summary.addNewResidual(ItDVER.getDistanceResidual()*M2MM);
	}
	(*stream)<<endl;
	
	//for output residual mean and the standart deviation of the residuals
	writeDistanceResultsSummary(summary, TABs);
}