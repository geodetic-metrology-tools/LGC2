#include "TEDMWriter.h"
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"

#include "TTSTNWriter.h" // getDistanceSigmaInMM, getDistanceSensibility

TEDMWriter::TEDMWriter(TAStreamFormatter& stream, bool /*hist*/) : TObservationWriter(stream), isAllfixed(false)
{}

TEDMWriter::~TEDMWriter(){}

//------------------ Synthesis header------------------------------------------------------------------------
void TEDMWriter::writeEDMSynthesisHeader()
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
	(*stream).writeStringLeft(nameWidth, "TSTN_POS"); //plane name
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

//------------------ Synthesis data--------------------------------------------------------------------------
void TEDMWriter::writeDSPTResultsSynthesis(const  TEDM& fEdm)
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	std::string         TABs = stream->getCurrSpaceExtended(1);

	const auto& dsptSummary = fEdm.getDSPTObsSummary();

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, fEdm.instrumentPos->getName());  //Reference point
	(*stream).writeDouble(obsResWidth, lengthResPrecision, dsptSummary.getResMax());//residu max
	(*stream).writeDouble(obsResWidth, lengthResPrecision, dsptSummary.getResMin());//residu min
	(*stream).writeDouble(obsResWidth, lengthResPrecision, dsptSummary.getMean());//residu moy
	(*stream).writeDouble(obsResWidth, lengthResPrecision, dsptSummary.getVariance());//ecart type
	(*stream) << endl;

}

//------------------ Result header---------------------------------------------------------------------------

void TEDMWriter::writeEDMHeader(const TEDM& fEdm)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					lengthPrecision = getLengthPrecision();
	std::string        TABs = stream->getCurrSpaceExtended(1);

	////////////////////////////////////////////////////////////
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "INSTRUMENT EDM: " + fEdm.instrument.ID);
	(*stream) << endl;
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "INST POS"); //Instrument position
	(*stream).writeStringLeft(nameWidth, fEdm.instrumentPos->getName());
	(*stream).writeString(obsWidth, "HINSTR (M)"); //instrument heigth
	(*stream).writeDouble(obsWidth, lengthPrecision, fEdm.instrument.instrHeight);
	(*stream) << endl << endl;
	///////////////////////////////////////////////////////////////////////////////////
}

void	TEDMWriter::writeDSPTResultsHeader(const int)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	std::string         TABs = stream->getCurrSpaceExtended(2);

	////////////////////////////////////////////////////////////
	//first line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POINT "); //second point's Name
	(*stream).writeString(obsWidth, "OBSERVE"); //mesured distance
	(*stream).writeString(obsResWidth, "SIGMA"); //sigma
	(*stream).writeString(obsWidth, "CALCULE"); //estimated distance
	(*stream).writeString(obsResWidth, "RESIDU"); //offset (mm)
	(*stream).writeString(obsResWidth, "SENSI"); //sensitivity
	(*stream).writeString(obsResWidth, "RES/SIG"); //offset/sigma 
	(*stream).writeString(obsWidth, "CONST"); //dist corr
	(*stream).writeString(obsResWidth, "SCONST"); //sigma of provisional dist corr
	(*stream).writeString(obsWidth, "H_PRISME"); //prism's height 	 
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeString(nameWidth, ""); //second point's Name
	(*stream).writeString(obsWidth, "(M)"); //mesured distance
	(*stream).writeString(obsResWidth, "(MM)"); //sigma
	(*stream).writeString(obsWidth, "(M)"); //estimated distance
	(*stream).writeString(obsResWidth, "(MM)"); //residu (mm)
	(*stream).writeString(obsResWidth, "(MM/CM)"); //sensitivity
	(*stream).writeString(obsResWidth, ""); //res/sigma
	(*stream).writeString(obsWidth, "(M)"); //provisional dist corr
	(*stream).writeString(obsResWidth, "(MM)"); //sigma of provisional dist corr
	(*stream).writeString(obsWidth, "(M)"); //prism's height
	(*stream) << endl;

	return;
}

//------------------ Result data---------------------------------------------------------------------------
void TEDMWriter::writeEDMResults(const TEDM& fEdm)
{
	TAStreamFormatter*	stream = getStream();
	std::string        TABs = stream->getCurrSpaceExtended(2);

	writeEDMHeader(fEdm);

   this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kDSPT), (int)fEdm.measDSPT.size());

	if(fEdm.measDSPT.size() > 0)
		//Write the measurements
		writeDSPTResultsData(fEdm.measDSPT, fEdm.instrument, fEdm.instrumentPos);
}

void TEDMWriter::writeDSPTResultsData(const std::list<TDSPT> measDSPT, const TInstrumentData::TEDM& instr, const LGCAdjustablePoint* instrPos)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision()-3, 0);
	int					lengthPrecision =	getLengthPrecision();
	std::string         TABs = stream->getCurrSpaceExtended(2);

    writeDSPTResultsHeader((int)measDSPT.size()); // write the title line for the observations
	for(auto const& ItDSPT : measDSPT)
	{
		(*stream)<<TABs;

		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItDSPT.targetPos->getName());

		//write the observed DSPT
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDSPT.getDistance());//Output value in meters [m], stored in [m]

		//write the sigma DSPT
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDSPT.target.sigmaCombined.getMMetresValue());

		//write the estimated DSPT
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDSPT.getDistance() + ItDSPT.getDistanceResidual());//Output value in meters [m], stored in [m]

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDSPT.getDistanceResidual().getMMetresValue());//Output value in meters [mm], stored in [m]

		//write the sensibility
		(*stream).writeDouble(obsResWidth, lengthResPrecision, TTSTNWriter::getDistanceSensibility<TDSPT, TInstrumentData::TEDM>(&ItDSPT, instrPos, instr));

		//write the res/sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDSPT.getDistanceResidual() / ItDSPT.target.sigmaCombined);
	
		//write the distance correction
		if (!ItDSPT.target.distCorrectionAdjustable->isFixed()){
			//write the distance cste calculated
			if (isAllfixed)
				if (!isnotanumber(ItDSPT.fAllFixedCs))
					(*stream).writeDouble(obsWidth, lengthPrecision, ItDSPT.fAllFixedCs.getMetresValue());
				else
					(*stream).writeString(obsWidth, "FIXED");
			else
				(*stream).writeDouble(obsWidth, lengthPrecision, ItDSPT.target.distCorrectionAdjustable->getEstimatedValue());

			//write the distance cste sigma )
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDSPT.target.distCorrectionAdjustable->getEstimatedPrecision().getMMetresValue());
		}
		else {
			//write the distance cste
			(*stream).writeDouble(obsWidth, lengthPrecision, ItDSPT.target.distCorrectionAdjustable->getProvisionalValue());

			//write the distance cste sigma 
			(*stream).writeString(obsResWidth, "FIXED");
		}

		//write the target height
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDSPT.target.targetHt);//Output value in meters [m], stored in [m]

		(*stream)<<endl;
	}
	(*stream)<<endl;
}

//------------------ Simu data---------------------------------------------------------------------------
void TEDMWriter::writeEDMSIMUResults(const TEDM& fEdm)
{
	TAStreamFormatter*	stream = getStream();
	std::string        TABs = stream->getCurrSpaceExtended(2);

	writeEDMHeader(fEdm);

	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kDSPT), (int)fEdm.measDSPT.size());

	if (fEdm.measDSPT.size() > 0)
		//Write the measurements
		writeDistanceResultsSummary(fEdm.getDSPTObsSummary(), TABs);

}

//------------------ Reliability header---------------------------------------------------------------------------
void	TEDMWriter::writeReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("POINT 1", "POINT 2", "", "OBSERVATION", "M", "MM");
}

//------------------ Reliability data---------------------------------------------------------------------------
void TEDMWriter::writeReliabilityData(const TEDM& fEdm , const TLGCStatistic& stat)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = std::max(getLengthResidualPrecision()-3, 0);

	//For each DSPT measurement of the station
	for(auto const& ItDspt : fEdm.measDSPT)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItDspt.getFirstObservationIndex();

		// get Stn point
		(*stream).writeStringLeft(nameWidth, fEdm.instrumentPos->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItDspt.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision,ItDspt.getDistance());
		//get the standard deviation
        (*stream).writeDouble(obsResWidth, lengthResPrecision, ItDspt.target.sigmaCombined.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDspt.getDistanceResidual().getMMetresValue());

		writeReliabilityMM(index, stat);
	}
	return;

}

