#include "THLSRWriter.h"
#include <TAMeas.h>
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include <Global.h>
#include "RoundOfMeasurements.h"
#include "LGCAdjustablePoint.h"

THLSRWriter::THLSRWriter(TAStreamFormatter& stream, bool /*hist*/) : TObservationWriter(stream)
{}

THLSRWriter::~THLSRWriter() {}

//------------------ Result header---------------------------------------------------------------------------
void THLSRWriter::writeHLSRResultsHeader()
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	std::string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(2);


	////////////////////////////////////////////////////////////
	//First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the hlsr
	(*stream).writeString(obsWidth, "OBSERVE"); //mesured ws distance
	(*stream).writeString(obsResWidth, "SIGMA"); //sigma DIST
	(*stream).writeString(obsWidth, "CALCULE"); //estimated offset
	(*stream).writeString(obsResWidth, "RESIDU"); //residual
	(*stream).writeString(obsResWidth, "RES/SIG");    //residual/sigma
	(*stream).writeString(obsResWidth, "OBSE"); //OBSE value
	(*stream).writeString(obsResWidth, "IHSE"); //IHSE value
	(*stream).writeString(obsResWidth, "ICSE"); //ICSE value
	(*stream).writeString(obsResWidth, "WSSE"); //WSSE value
	(*stream).writeString(nameWidth, "HLSR ID");    //hlsr ID
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); //POSITION
	(*stream).writeString(obsWidth, "(M)"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth, "(MM)"); //sigma observed value
	(*stream).writeString(obsWidth, "(M)"); //estimated offset
	(*stream).writeString(obsResWidth, "(MM)"); //residual
	(*stream).writeString(obsResWidth, "");    //residual/sigma
	(*stream).writeString(obsResWidth, "MM"); //OBSE value
	(*stream).writeString(obsResWidth, "MM"); //IHSE value
	(*stream).writeString(obsResWidth, "MM"); //ICSE value
	(*stream).writeString(obsResWidth, "MM"); //WSSE value
	(*stream).writeString(nameWidth, "");    //hlsr ID

	(*stream) << endl;
}


//------------------ Result data---------------------------------------------------------------------------
void THLSRWriter::writeECWSResults(const  TECWSROM& ecwsrom)
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	int					lengthPrecision = getLengthPrecision();
	std::string         TABs = stream->getCurrSpaceExtended(2);


	//write header
	(*stream) << endl;
	(*stream) << TABs << "ECWS" << endl;
	(*stream) << TABs << "HAUTEUR SURFACE D'EAU (M):" << TABs;
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwsrom.fMeasuredWSHeight->getEstimatedValue().getMetresValue());//Output value in meters [m], stored in [m]
	(*stream) << endl;
	(*stream) << TABs << "PRECISION HAUTEUR SURFACE D'EAU (MM):" << TABs;
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecwsrom.fMeasuredWSHeight->getEstimatedPrecision().getMMetresValue());//Output value in milimeters [mm], stored in [m]
	(*stream) << endl;


	////write the the water surface sigma
	//(*stream).writeString(nameWidth, "SIGMA WS");
	//h(*stream).writeDouble(obsResWidth, lengthResPrecision, ecwsrom.sigmaWS); //Output value in mm

	//data summury
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kECWS), (int)ecwsrom.measECWS.size());
	writeHLSRResultsHeader(); // write the title line for the observations


	for (auto const& ItECWS : ecwsrom.measECWS)
	{
		(*stream) << TABs;
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItECWS.targetPos->getName());

		//write the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECWS.getDistance());//Output value in meters [m], stored in [m]

		//write the sigma DIST
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWS.target.sigmaCombinedDist.getMMetresValue());//Output value in milimeters [mm], stored in [m]

		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECWS.getDistance() + ItECWS.getDistanceResidual());//Output value in meters [m], stored in [m]

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWS.getDistanceResidual().getMMetresValue());//Output value in milimeters [mm], stored in [m]

		//write the residual/sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWS.getDistanceResidual() / ItECWS.target.sigmaCombinedDist);//Output value in meters [m], stored in [m]

		//write the OBSE
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWS.target.sigmaD.getMMetresValue());//Output value in mm

		//write the IHSE
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWS.target.sigmaInstrHeight.getMMetresValue());//Output value in mm

		//write the ICSE
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWS.target.sigmaInstrCentering.getMMetresValue());//Output value in mm

		//write the ICSE
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWS.target.sigmaWS.getMMetresValue());//Output value in mm

		//write the scale ID
		(*stream).writeString(nameWidth, ItECWS.target.ID);

		(*stream) << endl;
	}
	(*stream) << endl;

}
//------------------ Simu data--------------------------------------------------------------------------

void THLSRWriter::writeECWSSIMUResults(const  TECWSROM& ecwsrom)
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					lengthPrecision = getLengthPrecision();
	std::string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kECWS), (int)ecwsrom.measECWS.size());

	for (auto const& ItECWS : ecwsrom.measECWS) {

		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "REFERENCE POINT"); //Reference point
		(*stream).writeStringLeft(nameWidth, ItECWS.targetPos->getName()); //Reference point
		(*stream).writeString(nameWidth, "DISTANCE TO WS (MM)");
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECWS.getDistance().getMMetresValue());

		//getObservedOffset().getMMetresValue());
		(*stream) << endl << endl;
	}

	writeDistanceResultsSummary(ecwsrom.getECWSObsSummary(), stream->getCurrSpaceExtended(2));
}


//------------------ Synthesis header------------------------------------------------------------------------
void THLSRWriter::writeHLSRSynthesisHeader()
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	// int				obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	std::string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);


	////////////////////////////////////////////////////////////
	//First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "REF. POINT"); //plane name
	(*stream).writeString(obsResWidth, "RES_MAX"); //residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); //residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); //residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); //ecart type
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(MM)");

	(*stream) << endl;
}
void THLSRWriter::writeDefResultsSynthesis(std::list<const TLGCObsSummary*>& meassum, int obsResWidth, int ResPrecision) {
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	for (auto const& ItMEAS : meassum) {
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, ItMEAS->getObsText()); //Reference point
		(*stream).writeDouble(obsResWidth, ResPrecision, ItMEAS->getResMax());//residu max
		(*stream).writeDouble(obsResWidth, ResPrecision, ItMEAS->getResMin());//residu min
		(*stream).writeDouble(obsResWidth, ResPrecision, ItMEAS->getMean());//residu moy
		(*stream).writeDouble(obsResWidth, ResPrecision, ItMEAS->getVariance());//ecart type
		(*stream) << endl;
	}
}


void THLSRWriter::writeECWSRResultsSynthesis(const  TECWSROM& ecwsrom)
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	// int				obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	// int				lengthPrecision = getLengthPrecision();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	const auto& ecwsSummary = ecwsrom.getECWSObsSummary();

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ecwsSummary.getObsText()); //Reference point
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecwsSummary.getResMax());//residu max
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecwsSummary.getResMin());//residu min
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecwsSummary.getMean());//residu moy
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecwsSummary.getVariance());//ecart type
	(*stream) << endl;


}


//------------------ Reliability header----------------------------------------------------------------------
void	THLSRWriter::writeECWSReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("PT REF", "STATION", "", "OBSERVATION", "M", "MM");
	return;
}


//------------------ Reliability data----------------------------------------------------------------------
void	THLSRWriter::writeECWSReliabilityData(const TLGCStatistic& stat, const std::list<TECWS>& measECWS)
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);


	//For each ECHO measurement of the station
	for (auto const& ItEcws : measECWS)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItEcws.getFirstObservationIndex();

		// get Ref  Point 
		(*stream).writeStringLeft(nameWidth, ItEcws.targetPos->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItEcws.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision, ItEcws.getDistance());
		//get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItEcws.targetPos->getStandDev(0));
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItEcws.getDistanceResidual().getMMetresValue());

		writeReliabilityMM(index, stat);
	}
	return;
}