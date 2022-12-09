#include "TLEVELWriter.h"
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include <Global.h>
#include <TAMeas.h>

TLEVELWriter::TLEVELWriter(TAStreamFormatter& stream, bool /*hist*/) : TObservationWriter(stream), isAllfixed(false)
{}

TLEVELWriter::~TLEVELWriter(){}

//------------------ Result header---------------------------------------------------------------------------
void TLEVELWriter::writeLEVELHeader(const TLEVEL& fLevel)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	std::string				separator = getSeparator();
	int					lengthPrecision = getLengthPrecision();
	std::string        TABs = stream->getCurrSpaceExtended(1);
	int					obsWidth = getObsWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	////////////////////////////////////////////////////////////
	//first line
	(*stream) << endl << TABs;
	(*stream).writeStringLeft(nameWidth, "LEVEL INSTRUMENT: " + fLevel.instrument.ID);
	(*stream) << endl;
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "REF POINT"); //Reference point
	(*stream).writeStringLeft(nameWidth, fLevel.fMeasuredPlane->getReferencePoint()->getName());
	(*stream).writeStringLeft(11, "X (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, fLevel.fMeasuredPlane->getReferencePoint()->getEstValue(0));
	(*stream).writeStringLeft(11, "Y (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, fLevel.fMeasuredPlane->getReferencePoint()->getEstValue(1));
	(*stream).writeStringLeft(11, "Z (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, fLevel.fMeasuredPlane->getReferencePoint()->getEstValue(2));
	(*stream) << endl;
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "PLANE");
	(*stream).writeStringLeft(11, "HDIST (MM)");
	(*stream).writeDouble(obsWidth, lengthResPrecision, fLevel.fMeasuredPlane->getRefPtDistEstimatedValue().getMMetresValue());
	(*stream).writeStringLeft(11, "SHDIST (MM)");
	(*stream).writeDouble(obsWidth, lengthResPrecision, fLevel.fMeasuredPlane->getRefPDistEstimatedPrecision().getMMetresValue());
	(*stream) << endl << endl;
	///////////////////////////////////////////////////////////////////////////////////
}

void TLEVELWriter::writeDLEVResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					obsIdWidth = getObsIdWidth();
	std::string         TABs = stream->getCurrSpaceExtended(2);


	////////////////////////////////////////////////////////////
	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kDLEV), nOObs);
	//Second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target	
	(*stream).writeString(obsWidth, "OBSERVE");       //DLEV: mesurement
	(*stream).writeString(obsResWidth, "SIGMA");     //DLEV: sigma 
	(*stream).writeString(obsWidth, "CALCULE");       //DLEV: estimation 
	(*stream).writeString(obsResWidth, "RESIDU");    //DLEV: residual
	(*stream).writeString(obsResWidth, "RES/SIG");   //DLEV: residual/sigma
	(*stream).writeString(obsWidth, "COLLIMATION");   //DLEV: collimation angle
	(*stream).writeString(obsResWidth, "SCOLL");         //DLEV: sigma of collimation angle

	(*stream).writeString(obsWidth, "DHOR");          //DHOR: mesurement
	(*stream).writeString(obsResWidth, "DSE");        //DHOR: sigma 
	(*stream).writeString(obsWidth, "CALCDHOR");      //DHOR: estimation  
	(*stream).writeString(obsResWidth, "RESDHOR");    //DHOR: residual
	(*stream).writeString(obsResWidth, "RES/SIG");    //DHOR: residual/sigma

	(*stream).writeString(obsWidth, "CONST");         //DHOR: Distance correction
	(*stream).writeString(obsResWidth, "SCONST");     //DHOR: 1-Sigma precision for the distance correction 
	(*stream).writeString(obsResWidth, "OBSE");       //DHOR: 1-sigma precision for the vertical distance observation 
	(*stream).writeString(obsResWidth, "PPM");        //DHOR: ppm value for the distance correction 
	(*stream).writeString(obsWidth, "H_TRGT");         //DHOR: Vertical offset of the staff
	(*stream).writeString(obsResWidth, "THSE");       //DHOR: 1-sigma precision for the vertical offset of the staff

	if (obsIdWidth != 0) (*stream).writeString(obsIdWidth, "ID"); // Observation identifier

	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeString(nameWidth, ""); //POSITION
	(*stream).writeString(obsWidth, "(M)");     //DLEV: mesurement
	(*stream).writeString(obsResWidth, "(MM)");//DLEV: sigma 
	(*stream).writeString(obsWidth, "(M)");     //DLEV: estimation 
	(*stream).writeString(obsResWidth, "(MM)");  //DLEV: residual
	(*stream).writeString(obsResWidth, "");      //DLEV: residual/sigma
	(*stream).writeString(obsWidth, "GONS");    //DLEV: collimation angle
	(*stream).writeString(obsResWidth, "CC");      //DLEV: sigma collimation angle

	(*stream).writeString(obsWidth, "(M)");     //DHOR: mesurement
	(*stream).writeString(obsResWidth, "(MM)"); //DHOR: sigma 
	(*stream).writeString(obsWidth, "(M)");     //DHOR: estimation  
	(*stream).writeString(obsResWidth, "(MM)"); //DHOR: residual
	(*stream).writeString(obsResWidth, "");     //DHOR: residual/sigma

	(*stream).writeString(obsWidth, "(M)");     //DHOR: Distance correction
	(*stream).writeString(obsResWidth, "(MM)"); //DHOR: 1-Sigma precision for the distance correction 
	(*stream).writeString(obsResWidth, "(MM)"); //DHOR: 1-sigma precision for the vertical distance observation 
	(*stream).writeString(obsResWidth, "(MM/KM)");     //DHOR: ppm value for the distance correction 
	(*stream).writeString(obsWidth, "(M)");     //DHOR: Vertical offset of the staff
	(*stream).writeString(obsResWidth, "(MM)"); //DHOR: 1-sigma precision for the vertical offset of the staff


	(*stream) << endl;
}

//------------------ Result data---------------------------------------------------------------------------
void TLEVELWriter::writeLEVELResults(const TLEVEL& fLevel)
{
	writeLEVELHeader(fLevel);

	if (fLevel.measDLEV.size() > 0)
		//The eventual DHOR result to be written inside this method 
		writeDLEVResults(fLevel.measDLEV, fLevel.instrument);
}

void TLEVELWriter::writeDLEVResults(std::list<TDLEV> measDLEV, const TInstrumentData::TLEVEL& instr)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					obsIdWidth = getObsIdWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	std::string         TABs = stream->getCurrSpaceExtended(2);

	writeDLEVResultsHeader((int)measDLEV.size());

	//For each DHOR measurement of the station
	for (auto const& ItDlev : measDLEV)
	{
		(*stream) << TABs;

		//Position of the target
		(*stream).writeStringLeft(nameWidth, ItDlev.targetPos->getName());

		//DLEV
		// mesured offset
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDlev.getDistance());
		//sigma DIST
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDlev.target.sigmaCombinedDist.getMMetresValue());
		//estimated offset
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDlev.getDistanceResidual() + ItDlev.getDistance());
		//residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDlev.getDistanceResidual().getMMetresValue());
		//residual/sima
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDlev.getDistanceResidual().getMetresValue() / ItDlev.target.sigmaCombinedDist.getMetresValue());

		//Collimation angle
		if (!instr.collAngleAdjustable->isFixed())
		{
			if (isAllfixed)
				if (!isnotanumber(ItDlev.fAllFixedCollimation))
					(*stream).writeDouble(obsWidth, getAnglePrecision(), ItDlev.fAllFixedCollimation.getGonsValue());
				else
					(*stream).writeString(obsWidth, "FIXED");
			else
				(*stream).writeDouble(obsWidth, getAnglePrecision(), instr.collAngleAdjustable->getEstimatedValue().getGonsValue());

			(*stream).writeDouble(obsResWidth, getAngleResidualPrecision(), instr.collAngleAdjustable->getEstimatedPrecision().getSignedCCValue());
		}
		else
		{
			(*stream).writeDouble(obsWidth, getAnglePrecision(), instr.collAngleAdjustable->getProvisionalValue().getGonsValue());
			(*stream).writeString(obsResWidth, "FIXED");
		}

		//DHOR
		if (ItDlev.dhor)
		{
			// mesured dhor
			(*stream).writeDouble(obsWidth, lengthPrecision, ItDlev.dhor.get()->getDistance());
			//sigma Dhor
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDlev.dhor.get()->getDHORSigma().getMMetresValue());
			//estimated offset
			(*stream).writeDouble(obsWidth, lengthPrecision, ItDlev.dhor.get()->getDistanceResidual() + ItDlev.dhor.get()->getDistance());
			//residual
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDlev.dhor.get()->getDistanceResidual().getMMetresValue());
			//res/sigma
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDlev.dhor.get()->getDistanceResidual() / ItDlev.dhor.get()->getDHORSigma());
		}
		else
		{
			// mesured dhor
			(*stream).writeString(obsWidth, "");
			//sigma Dhor
			(*stream).writeString(obsResWidth, "");
			//estimated offset
			(*stream).writeString(obsWidth, "");
			//residual
			(*stream).writeString(obsResWidth, "");
			//res/sigma
			(*stream).writeString(obsResWidth, "");

		}

		(*stream).writeDouble(obsWidth, lengthPrecision, ItDlev.target.distCorrectionValue.getMetresValue());   //DHOR: Distance correction
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDlev.target.sigmaDCorr.getMMetresValue());     //DHOR: 1-Sigma precision for the distance correction 
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDlev.target.sigmaD.getMMetresValue());         //DHOR: 1-sigma precision for the vertical distance observation 
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDlev.target.ppmD.getMMetresValue());           //DHOR: ppm value for the distance correction 
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDlev.target.staffHt.getMetresValue());               //DHOR: Vertical offset of the staff
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDlev.target.sigmaStaffHt.getMMetresValue());   //DHOR: 1-sigma precision for the vertical offset of the staff

		// Write the observation identifier
		(*stream).writeString(obsIdWidth, ItDlev.obsID);
		(*stream) << endl;
	}
	(*stream) << endl;

}

//------------------ Simu data---------------------------------------------------------------------------
void TLEVELWriter::writeLEVELSIMUResults(const TLEVEL& fLevel)
{
	TAStreamFormatter*	stream = getStream();
	std::string        TABs = stream->getCurrSpaceExtended(2);

	writeLEVELHeader(fLevel);

	if (fLevel.measDLEV.size() > 0){
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kDLEV), (int)fLevel.measDLEV.size());
		writeDistanceResultsSummary(fLevel.getDLEVObsSummary(), TABs);
	}
	//The DHOR result summary
	if (fLevel.hasDHOR){
		(*stream) << TABs;
		(*stream) << "DHOR" << endl;
		writeDistanceResultsSummary(fLevel.getDHORObsSummary(), TABs);
	}
}

//------------------ Reliability header---------------------------------------------------------------------------
void	TLEVELWriter::writeReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("POINT REF", "POINT 2", "", "OBSERVATION", "M", "MM");
	return;
}


//------------------ Reliability data---------------------------------------------------------------------------
void TLEVELWriter::writeDHORReliabilityData(const TLEVEL& fLevel, const TLGCStatistic& stat)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	//For each station
	for (auto const& ItDhor : fLevel.measDLEV)
	{
		//if we have a dhor measurements, we write the data
		if (ItDhor.dhor)
		{
			// Observation index to take the right value in the statistic vector
			int index = ItDhor.dhor->getFirstObservationIndex();

			// get reference point to the plane
			(*stream).writeStringLeft(nameWidth, fLevel.fMeasuredPlane->getReferencePoint()->getName());
			//get Tg point
			(*stream).writeStringLeft(nameWidth, ItDhor.targetPos->getName());
			// get Point 3
			(*stream).writeStringLeft(nameWidth, "");

			//get the observed distance
			(*stream).writeDouble(obsWidth, lengthPrecision, ItDhor.dhor->getDistance());
			//get the standard deviation
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDhor.dhor->getDHORSigma().getMMetresValue());
			//get the residual
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDhor.dhor->getDistanceResidual().getMMetresValue());

			writeReliabilityMM(index, stat);
		}

	}
	return;
}

void TLEVELWriter::writeDLEVReliabilityData(const TLEVEL& fLevel, const TLGCStatistic& stat)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	//For each DLEV measurement of the station
	for (auto const& ItDLEV : fLevel.measDLEV)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItDLEV.getFirstObservationIndex();

		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, fLevel.fMeasuredPlane->getReferencePoint()->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItDLEV.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDLEV.getDistance());
		//get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDLEV.target.sigmaCombinedDist.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDLEV.getDistanceResidual().getMMetresValue());

		writeReliabilityMM(index, stat);
	}
	return;
}


//------------------ Synthesis header-------------------------------------------------------------------------
void TLEVELWriter::writeLEVELSynthesisHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
    // int				obsWidth = getObsWidth();
    int					obsResWidth = getObsResWidth();
	std::string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);


	////////////////////////////////////////////////////////////
	//First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "DLEV_PLANE"); //plane name
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
void TLEVELWriter::writeLEVELResultsSynthesis(const TLEVEL& fLevel)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
    // int				obsWidth = getObsWidth();
    int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
    // int				lengthPrecision = getLengthPrecision();
    std::string         TABs = stream->getCurrSpaceExtended(1);

    const auto &dlevSummary = fLevel.getDLEVObsSummary();

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, fLevel.fMeasuredPlane->getReferencePoint()->getName()); //Reference point
    (*stream).writeDouble(obsResWidth, lengthResPrecision, dlevSummary.getResMax());//residu max
    (*stream).writeDouble(obsResWidth, lengthResPrecision, dlevSummary.getResMin());//residu min
	(*stream).writeDouble(obsResWidth, lengthResPrecision, dlevSummary.getMean());//residu moy
	(*stream).writeDouble(obsResWidth, lengthResPrecision, dlevSummary.getStdev()); // ecart type
	(*stream) << endl;

}

void TLEVELWriter::writeLEVELResultsSynthesis(std::list<const TLGCObsSummary*> &dlevsum)
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	std::string         TABs = stream->getCurrSpaceExtended(1);

	for (auto const& ItDLEV : dlevsum) {
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, ItDLEV->getObsText()); //Reference point
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDLEV->getResMax());//residu max
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDLEV->getResMin());//residu min
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDLEV->getMean());//residu moy
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDLEV->getStdev()); // ecart type
		(*stream) << endl;
	}
}