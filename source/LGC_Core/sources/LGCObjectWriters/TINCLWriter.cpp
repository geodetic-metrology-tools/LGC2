#include "TINCLWriter.h"
#include <TAMeas.h>
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include <Global.h>
#include "RoundOfMeasurements.h"
#include "LGCAdjustablePoint.h"

TINCLWriter::TINCLWriter(TAStreamFormatter& stream, bool /*hist*/) : TObservationWriter(stream)
{}

TINCLWriter::~TINCLWriter() {}

//------------------ Result header---------------------------------------------------------------------------
void TINCLWriter::writeINCLResultsHeader()
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
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the scale
	(*stream).writeString(obsWidth, "OBSERVE"); //mesured offset
	(*stream).writeString(obsResWidth, "SIGMA"); //sigma DIST
	(*stream).writeString(obsWidth, "CALCULE"); //estimated offset 
	(*stream).writeString(obsResWidth, "RESIDU"); //residual
	(*stream).writeString(obsResWidth, "RES/SIG");    //residual/sigma
	(*stream).writeString(nameWidth, "INCL ID");    //inclinometer ID
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); //POSITION
	(*stream).writeString(obsWidth, "(GON)"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth, "(CC)"); //sigma observed value
	(*stream).writeString(obsWidth, "(GON)"); //estimated offset
	(*stream).writeString(obsResWidth, "(CC)"); //residual
	(*stream).writeString(obsResWidth, "");    //residual/sigma
	(*stream).writeString(nameWidth, "");    //inclinometer ID

	(*stream) << endl;
}


//------------------ Result data---------------------------------------------------------------------------
void TINCLWriter::writeINCLYResults(const  TINCLYROM& inclyrom)
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);
	int					anglePrecision = getAnglePrecision();
	std::string         TABs = stream->getCurrSpaceExtended(2);

	//write header
	(*stream) << endl;
	(*stream) << TABs << "INCLY" << endl;

	
	//data summury
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kINCLY), (int)inclyrom.measINCLY.size());
	writeINCLResultsHeader(); // write the title line for the observations

	for (auto const& ItINCLY : inclyrom.measINCLY)
	{
		(*stream) << TABs;
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItINCLY.targetPos->getName());

		//write the observed angle
		(*stream).writeDouble(obsWidth, anglePrecision, ItINCLY.getAngle().getGonsValue());//Output value in gradians [gon], stored in [gon]

		//write the sigma angle
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.sigmaCombined.getSignedCCValue());//Output value in cc [cc], stored in [gon]

		//write the estimated angle
		(*stream).writeDouble(obsWidth, anglePrecision, ItINCLY.getAngle().getGonsValue() + ItINCLY.getAngleResidual().getGonsValue());//Output value in gradians [gon], stored in [gon]

		//write the residual
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.getAngleResidual().getSignedCCValue());//Output value in cc [cc], stored in [gon]

		//write the residual/sigma
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.getAngleResidual().getRadiansValue() / ItINCLY.target.sigmaCombined.getRadiansValue());//Output value unitless

		//write the scale ID
		(*stream).writeString(nameWidth, ItINCLY.target.ID);
		(*stream) << endl;
	}
	(*stream) << endl;

}

//------------------ Simu data--------------------------------------------------------------------------
void TINCLWriter::writeINCLYSIMUResults(const  TINCLYROM& inclyrom)
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					anglePrecision = getAnglePrecision();
	std::string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kINCLY), (int)inclyrom.measINCLY.size());
}



//------------------ Reliability header----------------------------------------------------------------------
void	TINCLWriter::writeINCLYReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("FRAME","STATION", "", "OBSERVATION", "GON", "CC");
	return;
}



//------------------ Reliability data----------------------------------------------------------------------
void	TINCLWriter::writeINCLYReliabilityData(const  TINCLYROM& inclyrom, const TLGCStatistic& stat, const std::list<TINCLY> measINCLY)
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);


	//For each INCLY measurement of the station
	for (auto const& ItINCLY : measINCLY)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItINCLY.getFirstObservationIndex();
		
		//Get the frame name
		(*stream).writeStringLeft(nameWidth, inclyrom.positionInTree.node->data->frame.getName());

		//get Station point
		(*stream).writeStringLeft(nameWidth, ItINCLY.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed distance
		(*stream).writeDouble(obsWidth, anglePrecision, ItINCLY.getAngle().getGonsValue());
		//get the standard deviation
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.sigmaCombined.getSignedCCValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.getAngleResidual().getSignedCCValue());

		writeReliabilityMM(index, stat);
	}
	return;
}


//------------------ Synthesis header------------------------------------------------------------------------
void TINCLWriter::writeINCLSynthesisHeader()
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
	(*stream).writeStringLeft(nameWidth, "STATION"); //plane name
	(*stream).writeString(obsResWidth, "RES_MAX"); //residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); //residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); //residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); //ecart type
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");

	(*stream) << endl;
}


//------------------ Synthesis data--------------------------------------------------------------------------
void TINCLWriter::writeINCLYResultsSynthesis(const  TINCLYROM& inclyrom)
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsResWidth = getObsResWidth();
	int					angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);
	std::string         TABs = stream->getCurrSpaceExtended(1);

	const auto& inclySummary = inclyrom.getINCLYObsSummary();

	(*stream) << TABs;

	//Name of the station linked to the frame 
	(*stream).writeStringLeft(nameWidth, inclyrom.positionInTree.node->data->frame.getName()); //Reference point
	(*stream).writeDouble(obsResWidth, angleResPrecision, inclySummary.getResMax());//residu max
	(*stream).writeDouble(obsResWidth, angleResPrecision, inclySummary.getResMin());//residu min
	(*stream).writeDouble(obsResWidth, angleResPrecision, inclySummary.getMean());//residu moy
	(*stream).writeDouble(obsResWidth, angleResPrecision, inclySummary.getVariance());//ecart type
	(*stream) << endl;


}
