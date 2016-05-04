#include "TEDMWriter.h"
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include "TLGCObsSummary.h"

TEDMWriter::TEDMWriter(TAStreamFormatter& stream, bool hist) : TObservationWriter(stream), isAllfixed(false), writeHist(hist)
{}

TEDMWriter::~TEDMWriter(){}

//Result
void TEDMWriter::writeEDMResults(const TEDM& fEdm)
{
	TAStreamFormatter*	stream = getStream();
	std::string        TABs = stream->getCurrSpaceExtended(3);

	writeEDMHeader(fEdm);
	writeEDMData(fEdm);

   this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kDSPT), (int)fEdm.measDSPT.size());
	(*stream)<<endl;

	if(fEdm.measDSPT.size() > 0){	
		//Write the measurements
		writeDSPTResultsData( fEdm.measDSPT, fEdm.instrument, fEdm.instrumentPos);
		writeDistanceResultsSummary(fEdm.getDSPTObsSummary(),TABs);
	}

}

void TEDMWriter::writeEDMSIMUResults(const TEDM& fEdm)
{
	TAStreamFormatter*	stream = getStream();
	std::string        TABs = stream->getCurrSpaceExtended(3);

	writeEDMHeader(fEdm);
	writeEDMData(fEdm);

   this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kDSPT), (int)fEdm.measDSPT.size());

	if(fEdm.measDSPT.size() > 0){	
		//Write the measurements
		writeDistanceResultsSummary(fEdm.getDSPTObsSummary(),TABs);
		if (writeHist)
			writeHisto(fEdm.getDSPTObsSummary(), "DSTP");
	}

}

void TEDMWriter::writeEDMHeader(const TEDM& fEdm)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	std::string        TABs = stream->getCurrSpaceExtended(1);

	////////////////////////////////////////////////////////////
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"EDM INSTRUMENT: " + fEdm.instrument.ID);	
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"INST POS"); //Instrument position
	(*stream).writeString(obsWidth,	"HINSTR (M)"); //instrument heigth
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
}

void TEDMWriter::writeEDMData(const TEDM& fEdm)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					lengthPrecision =	getLengthPrecision();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	(*stream)<<TABs;
	//write NAME OF THE POINT ON WHICH STATION IS POSITIONED
	(*stream).writeStringLeft(nameWidth, fEdm.instrumentPos->getName());

	//write the instrument height
	(*stream).writeDouble(obsWidth, lengthPrecision, fEdm.instrument.instrHeight);		
	(*stream)<<endl<<endl;
}

void	TEDMWriter::writeDSPTResultsHeader(const int)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	////////////////////////////////////////////////////////////
	//first line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,	"POINT "); //second point's Name
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured distance
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated distance
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset (mm)
	(*stream).writeString(obsResWidth,	"SENSI"); //sensitivity
	(*stream).writeString(obsResWidth,	"RES/SIG"); //offset/sigma 
	(*stream).writeString(obsWidth, "CONST"); //dist corr
	(*stream).writeString(obsResWidth, "SCONST"); //sigma of provisional dist corr
	(*stream).writeString(obsWidth,	"H_PRISME"); //prism's height 	 
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	"(M)"); //mesured distance
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma
	(*stream).writeString(obsWidth,	"(M)"); //estimated distance
	(*stream).writeString(obsResWidth,	"(MM)"); //residu (mm)
	(*stream).writeString(obsResWidth,	"(MM/CM)"); //sensitivity
	(*stream).writeString(obsResWidth,	""); //res/sigma
	(*stream).writeString(obsWidth, "(M)"); //provisional dist corr
	(*stream).writeString(obsResWidth, "(MM)"); //sigma of provisional dist corr
	(*stream).writeString(obsWidth,	"(M)"); //prism's height
	(*stream)<<endl;
	
	return;
}

void TEDMWriter::writeDSPTResultsData(const std::vector<TDSPT> measDSPT,const TInstrumentData::TEDM& instr , const TAdjustablePoint* instrPos)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = max(getLengthResidualPrecision()-3, 0);
	int					lengthPrecision =	getLengthPrecision();
	std::string         TABs = stream->getCurrSpaceExtended(3);

    writeDSPTResultsHeader((int)measDSPT.size()); // write the title line for the observations
	for(auto const& ItDSPT : measDSPT)
	{
		(*stream)<<TABs;

		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItDSPT.targetPos->getName());

		//write the observed DSPT
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDSPT.getDistance());//Output value in meters [m], stored in [m]

		//write the sigma DSPT
		(*stream).writeDouble(obsResWidth, lengthResPrecision, (ItDSPT.target.sigmaDSpt + ItDSPT.target.ppmDSpt*ItDSPT.getDistance() / 1000)* M2MM);//Output value in meters [mm], stored in [m]

		//write the estimated DSPT
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDSPT.getDistance() + ItDSPT.getDistanceResidual());//Output value in meters [m], stored in [m]

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDSPT.getDistanceResidual().getMMetresValue());//Output value in meters [mm], stored in [m]

		//write the sensibility
		TReal dz= ItDSPT.targetPos->getEstValue(2) + ItDSPT.target.targetHt - instrPos->getEstValue(2) - instr.instrHeight;
		if (ItDSPT.target.distCorrectionUnknown)
		{
			(*stream).writeDouble(obsResWidth, lengthResPrecision, 10 *dz / (ItDSPT.getDistance() + ItDSPT.getDistanceResidual() + ItDSPT.target.distCorrectionAdjustable->getEstimatedValue()));
		}else{
			(*stream).writeDouble(obsResWidth, lengthResPrecision, 10 * dz / (ItDSPT.getDistance() + ItDSPT.getDistanceResidual()));
		}

		//write the res/sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDSPT.getDistanceResidual()/ItDSPT.target.sigmaDSpt);

		//write the distance correction
		if (!ItDSPT.target.distCorrectionAdjustable->isFixed()){
			//write the distance cste calculated
			if (isAllfixed)
				if (!isnotanumber(ItDSPT.fAllFixedCs))
					(*stream).writeDouble(obsWidth, lengthPrecision, ItDSPT.fAllFixedCs.getMMetresValue());
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


//Reliability
void TEDMWriter::writeReliabilityData(const TEDM& fEdm , const TLGCStatistic& stat)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision()-3, 0);

	//For each DSPT measurement of the station
	for(auto const& ItDspt : fEdm.measDSPT)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItDspt.getFirstObservationIndex();

		// get Stn point
		(*stream).writeStringLeft(nameWidth, fEdm.instrumentPos->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItDspt.target.ID);
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision,ItDspt.getDistance());
		//get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDspt.target.sigmaDSpt.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDspt.getDistanceResidual().getMMetresValue());


		writeReliability(index, stat);
		(*stream).setDataSpacing();
	}
	return;

}

void	TEDMWriter::writeReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("POINT 1","POINT 2", "", "OBSERVATION", "M", "MM");
}