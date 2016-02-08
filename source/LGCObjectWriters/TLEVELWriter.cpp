#include "TLEVELWriter.h"
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include "Global.h"
#include "TLGCObsSummary.h"
#include "TAMeas.h"

TLEVELWriter::TLEVELWriter(TAStreamFormatter& stream) :TObservationWriter(stream), isAllfixed(false)
{}

TLEVELWriter::~TLEVELWriter(){}

void TLEVELWriter::writeLEVELResults(const TLEVEL& fLevel)
{
	TAStreamFormatter*	stream = getStream();
	std::string        TABs = stream->getCurrSpaceExtended(3);
	
	writeLEVELHeader(fLevel);
	writeLEVELData(fLevel);

	if(fLevel.measDLEV.size() > 0){
		//The eventual DHOR result to be written inside this method 
		writeDLEVResults(fLevel.measDLEV);
		(*stream)<<TABs;
		(*stream)<<"DLEV"<<endl;
		writeDistanceResultsSummary(fLevel.getDLEVObsSummary(), TABs);
	}
	//The DHOR result summary
	if(fLevel.hasDHOR){
		(*stream)<<TABs;
		(*stream)<<"DHOR"<<endl;
		writeDistanceResultsSummary(fLevel.getDHORObsSummary(), TABs);
	}
}

void TLEVELWriter::writeLEVELSIMUResults(const TLEVEL& fLevel)
{
	TAStreamFormatter*	stream = getStream();
	std::string        TABs = stream->getCurrSpaceExtended(3);
	
	writeLEVELHeader(fLevel);
	writeLEVELData(fLevel);

	if(fLevel.measDLEV.size() > 0){
		(*stream)<<TABs;
		(*stream)<<"DLEV"<<endl;
		writeDistanceResultsSummary(fLevel.getDLEVObsSummary(), TABs);
	}
	//The DHOR result summary
	if(fLevel.hasDHOR){
		(*stream)<<TABs;
		(*stream)<<"DHOR"<<endl;
		writeDistanceResultsSummary(fLevel.getDHORObsSummary(), TABs);
	}
}

void TLEVELWriter::writeLEVELHeader(const TLEVEL& fLevel)
{
TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	string				separator = getSeparator();
	std::string        TABs = stream->getCurrSpaceExtended(1);

	////////////////////////////////////////////////////////////
	//first line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"LEVEL INSTRUMENT: " + fLevel.instrument.ID);
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"REF POINT"); //Reference point
	(*stream).writeString(nameWidth,"X"); 
	(*stream).writeString(nameWidth,"Y");
	(*stream).writeString(nameWidth,"Z");
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
}
void TLEVELWriter::writeLEVELData(const TLEVEL& fLevel)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	std::string        TABs = stream->getCurrSpaceExtended(1);

	(*stream)<<TABs;
	//write NAME OF THE POINT ON WHICH STATION IS POSITIONED
	(*stream).writeStringLeft(nameWidth, fLevel.fMeasuredPlane->getReferencePoint()->getName());
	(*stream).writeDouble(obsWidth, obsWidth, fLevel.fMeasuredPlane->getReferencePoint()->getEstValue(0));
	(*stream).writeDouble(obsWidth, obsWidth, fLevel.fMeasuredPlane->getReferencePoint()->getEstValue(1));
	(*stream).writeDouble(obsWidth, obsWidth, fLevel.fMeasuredPlane->getReferencePoint()->getEstValue(2));
	(*stream)<<endl<<endl;
}

void TLEVELWriter::writeDLEVResults(std::vector<TDLEV> measDLEV)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision()-3, 0);
	std::string         TABs = stream->getCurrSpaceExtended(3);

	writeDLEVResultsHeader((int)measDLEV.size());

	//For each DHOR measurement of the station
	for(auto const& ItDlev : measDLEV)
	{
		(*stream)<<TABs;

		//Position of the target
		(*stream).writeStringLeft(nameWidth, ItDlev.targetPos->getName());

		//DLEV
		// mesured offset
		(*stream).writeDouble(obsWidth, lengthPrecision,ItDlev.getDistance()); 
		//sigma DIST
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDlev.target.sigmaD.getMMetresValue());
		//estimated offset
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDlev.getDistanceResidual() + ItDlev.getDistance());
		//residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDlev.getDistanceResidual().getMMetresValue());
		//residual/sima
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDlev.getDistanceResidual()/ItDlev.target.sigmaD);

		if (isAllfixed)
			if (!isnotanumber(ItDlev.fAllFixedCollimation))
				(*stream).writeDouble(obsWidth, getAnglePrecision(), ItDlev.fAllFixedCollimation.getGonsValue());
			else
				(*stream).writeString(obsWidth, "FIXED");
		else
			(*stream).writeString(obsWidth, "");

		//DHOR
		if (ItDlev.dhor)
		{
			// mesured dhor
			(*stream).writeDouble(obsWidth, lengthPrecision,ItDlev.dhor.get()->getDistance()); 
			//sigma Dhor
			(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDlev.dhor.get()->getDHORSigma().getMMetresValue());
			//estimated offset
			(*stream).writeDouble(obsWidth, lengthPrecision, ItDlev.dhor.get()->getDistanceResidual() + ItDlev.dhor.get()->getDistance());
			//residual
			(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDlev.dhor.get()->getDistanceResidual().getMMetresValue());
			//res/sigma
			(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDlev.dhor.get()->getDistanceResidual()/ItDlev.dhor.get()->getDHORSigma());
		}
		else
		{
			// mesured dhor
			(*stream).writeString(obsWidth,""); 
			//sigma Dhor
			(*stream).writeString(obsResWidth,"");
			//estimated offset
			(*stream).writeString(obsWidth, "");
			//residual
			(*stream).writeString(obsResWidth, "");
			//res/sigma
			(*stream).writeString(obsResWidth, "");
		
		}
		//Vertical offset of the staff
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDlev.target.staffHt);

		(*stream)<<endl;
	}
	(*stream)<<endl;

}
void TLEVELWriter::writeDLEVResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	std::string         TABs = stream->getCurrSpaceExtended(3);


	////////////////////////////////////////////////////////////
	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kDLEV), nOObs);
	(*stream)<<endl;
	//Second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target	
	(*stream).writeString(obsWidth,	"OBSERVE");       //DLEV: mesurement
	(*stream).writeString(obsResWidth,	"SIGMA");     //DLEV: sigma 
	(*stream).writeString(obsWidth,	"CALCULE");       //DLEV: estimation 
	(*stream).writeString(obsResWidth,	"RESIDU");    //DLEV: residual
	(*stream).writeString(obsResWidth,	"RES/SIG");   //DLEV: residual/sigma
	if (isAllfixed)
		(*stream).writeString(obsWidth, "COLLIMATION");       //DLEV: allfixed parameter (collimation angle)

	(*stream).writeString(obsWidth,	"OBSDHOR");       //DHOR: mesurement
	(*stream).writeString(obsResWidth,	"SDHOR");     //DHOR: sigma 
	(*stream).writeString(obsWidth,	"CALCDHOR");      //DHOR: estimation  
	(*stream).writeString(obsResWidth,	"RESDHOR");   //DHOR: residual
	(*stream).writeString(obsResWidth,	"RES/SIG");   //DHOR: residual/sigma
	(*stream).writeString (obsWidth,	"HStaff"); //Vertical offset of the staff
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeString(nameWidth,""); //POSITION
	(*stream).writeString(obsWidth,	"(M)");     //DLEV: mesurement
	(*stream).writeString(obsResWidth,	"(MM)");//DLEV: sigma 
	(*stream).writeString(obsWidth,	"(M)");     //DLEV: estimation 
	(*stream).writeString(obsResWidth,"(MM)");  //DLEV: residual
	(*stream).writeString(obsResWidth,"");      //DLEV: residual/sigma
	if (isAllfixed)
		(*stream).writeString(obsWidth, "GONS");       //DLEV: allfixed parameter (collimation angle)

	(*stream).writeString(obsWidth,	"(M)");     //DHOR: mesurement
	(*stream).writeString(obsResWidth,	"(MM)");//DHOR: sigma 
	(*stream).writeString(obsWidth,	"(M)");     //DHOR: estimation  
	(*stream).writeString(obsResWidth,"(MM)");  //DHOR: residual
	(*stream).writeString(obsResWidth,"");      //DHOR: residual/sigma
	(*stream).writeString(obsWidth,	"(M)"); //Vertical offset of the staff
	(*stream)<<endl;
}

//Reliability
void	TLEVELWriter::writeReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("POINT REF","POINT 2", "", "OBSERVATION", "M", "MM");
	return;
}

void TLEVELWriter::writeDHORReliabilityData(const TLEVEL& fLevel, const TLGCStatistic& stat )
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision()-3, 0);

	//For each station
	for(auto const& ItDhor : fLevel.measDLEV)
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
			(*stream).writeDouble(obsWidth, lengthPrecision,ItDhor.dhor->getDistance());
			//get the standard deviation
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDhor.dhor->getDHORSigma().getMMetresValue());
			//get the residual
			(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDhor.dhor->getDistanceResidual().getMMetresValue());

			writeReliability(index, stat);
			(*stream).setDataSpacing();
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
	int					lengthResPrecision = max(getLengthResidualPrecision()-3, 0);

	//For each DLEV measurement of the station
	for(auto const& ItDLEV : fLevel.measDLEV)
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
		(*stream).writeDouble(obsWidth, lengthPrecision,ItDLEV.getDistance());
		//get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDLEV.target.sigmaD.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDLEV.getDistanceResidual().getMMetresValue());


		writeReliability(index, stat);
		(*stream).setDataSpacing();
	}
return;
}

