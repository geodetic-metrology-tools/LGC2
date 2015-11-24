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

//------------------ Reliability header---------------------------------------------------------------------------
void	TOtherMeasurentWriter::writeDVERReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("POINT 1","POINT 2", "", "OBSERVATION", "M", "MM");
	return;
}

void	TOtherMeasurentWriter::writeORIEReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("STATION", "POINT", "", "OBSERVATION", "GON", "CC");
	return;
}

void	TOtherMeasurentWriter::writeRADIReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("POINT", "", "", "OBSERVATION", "M", "MM");
	return;
}


//------------------ Reliability data---------------------------------------------------------------------------
void	TOtherMeasurentWriter::writeDVERReliabilityData(const std::vector<TDVER>& fDVER, const TLGCStatistic& stat)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);

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
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDver.getObservedStDev().getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDver.getDistanceResidual().getMMetresValue());


		writeReliability(index, stat);
		(*stream).setDataSpacing();
	}
	return;
}

void	TOtherMeasurentWriter::writeORIEReliabilityData(const std::vector<TORIE>& fORIE, const TLGCStatistic& stat, const TAdjustablePoint& instPos)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResidualPrecision = max(getAngleResidualPrecision() - 4, 0);
	string				separator = getSeparator();


	//For each orie measurement of the station
	for (auto const& ItORIE : fORIE)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItORIE.getFirstObservationIndex();

		// get Point 1 of the line or the linenumber?
		(*stream).writeStringLeft(nameWidth, instPos.getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItORIE.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed ORIE
		(*stream).writeDouble(obsWidth, anglePrecision, ItORIE.getAngle().getGonsValue());
		//write the sigma ORIE
		(*stream).writeDouble(obsResWidth, angleResidualPrecision, ItORIE.target.sigmaAngl.getSignedCCValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, angleResidualPrecision, ItORIE.getAngleResidual().getSignedCCValue());


		writeReliability(index, stat);
		(*stream).setDataSpacing();
	}
	return;
}

void	TOtherMeasurentWriter::writeRADIReliabilityData(const std::vector<TRADI>& fRADI, const TLGCStatistic& stat)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);

	//For each ECHO measurement of the station
	for (auto const& It : fRADI)
	{
		// Observation index to take the right value in the statistic vector
		int index = It.getFirstObservationIndex();

		// get Point 1 of the line or the linenumber?
		(*stream).writeStringLeft(nameWidth, It.station->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, "");
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the bearing 
		(*stream).writeDouble(obsWidth, lengthPrecision, It.getAngleCnstr());
		//get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getObservedStDev().getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getResidual().getMMetresValue());


		writeReliability(index, stat);
		(*stream).setDataSpacing();
	}
	return;
}

//------------------ Result header---------------------------------------------------------------------------
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

void TOtherMeasurentWriter::writeORIEResultsHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);


	////////////////////////////////////////////////////////////
	//First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "STATION"); //Position of the inst
	(*stream).writeStringLeft(nameWidth, "POINT"); //Position of the scale
	(*stream).writeString(obsWidth, "OBSERVE"); //observed orie
	(*stream).writeString(obsResWidth, "SIGMA"); //sigma 
	(*stream).writeString(obsWidth, "CALCULE"); //estimated orie 
	(*stream).writeString(obsResWidth, "RESIDU"); //residual
	(*stream).writeString(obsResWidth, "RES/SIG");//residu/sigma
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); //Position of the inst 
	(*stream).writeStringLeft(nameWidth, ""); //Position of the scale
	(*stream).writeString(obsWidth, "(GON)"); //observed orie
	(*stream).writeString(obsResWidth, "(CC)"); //sigma observed value
	(*stream).writeString(obsWidth, "(GON)"); //estimated orie
	(*stream).writeString(obsResWidth, "(CC)"); //residual
	(*stream).writeString(obsResWidth, "");    //residu/sigma

	(*stream) << endl;
}

void TOtherMeasurentWriter::writePDORResultsHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);


	////////////////////////////////////////////////////////////
	//First line
	(*stream) << TABs;
	(*stream).writeString(nameWidth, "CALA POINT");
	(*stream).writeString(nameWidth, "POINT");
	(*stream).writeString(obsWidth, "OBSERVE"); //observed bearing
	(*stream).writeString(obsWidth, "CALCULE"); //estimated bearing 
	(*stream).writeString(obsResWidth, "RESIDU"); //residual
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(obsWidth, "(GON)"); //observed bearing
	(*stream).writeString(obsWidth, "(GON)"); //estimated bearing
	(*stream).writeString(obsResWidth, "(CC)"); //residual
	(*stream) << endl;
}

void TOtherMeasurentWriter::writeRADIResultsHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);


	////////////////////////////////////////////////////////////
	//First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POINT"); //Position of the ponit
	(*stream).writeString(obsWidth, "GIS"); //observed radi
	(*stream).writeString(obsResWidth, "SIGMA"); //sigma 
	(*stream).writeString(obsResWidth, "RESIDU"); //residual
	(*stream).writeString(obsResWidth, "RES/SIG");//residual/sigma
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); //Position of the inst 
	(*stream).writeString(obsWidth, "(GON)"); //observed radi
	(*stream).writeString(obsResWidth, "(MM)"); //sigma observed value
	(*stream).writeString(obsResWidth, "(MM)"); //residual
	(*stream).writeString(obsResWidth, "");    //residual/sigma

	(*stream) << endl;
}
//------------------ Result data---------------------------------------------------------------------------
void TOtherMeasurentWriter::writePDORResults(const TPdorObs& fPDOR)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					angleResPrecision = max(getAngleResidualPrecision() - 4, 0);
	int					anglePrecision = getAnglePrecision();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	//first line
	(*stream) << TABs;
	this->writeObsTitle(this->getObsDescriptionEN(TALGCObjectWriter::kPDOR), 1);
	(*stream) << endl;

	writePDORResultsHeader(); // write the title line for the observations

	(*stream) << TABs;
	//write CALA POSITION
	(*stream).writeString(nameWidth, fPDOR.calaPt->getName());
	//write ORI POSITION
	(*stream).writeString(nameWidth, fPDOR.orientationPt->getName());

	//write the observed bearing
	(*stream).writeDouble(obsWidth, anglePrecision, fPDOR.getBearing().getGonsValue());

	//write the estimated bearing
	(*stream).writeDouble(obsWidth, anglePrecision, fPDOR.getBearing().getGonsValue() + fPDOR.getAngleResidual().getGonsValue());

	//write the residual
	(*stream).writeDouble(obsResWidth, angleResPrecision, fPDOR.getAngleResidual().getSignedCCValue());
	(*stream) << endl<<endl;
	(*stream) << endl;
}

void TOtherMeasurentWriter::writeDVERResults(const std::vector<TDVER>& fDVER)
{
TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = max(getLengthResidualPrecision()-3, 0);
	int					lengthPrecision =	getLengthPrecision();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	//first line
   this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kDVER), (int)fDVER.size());
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
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDVER.getObservedStDev().getMMetresValue());//Output value in meters [mm], stored in [m]

		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDVER.getDistance() + ItDVER.getDistanceResidual());//Output value in meters [m], stored in [m]

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDVER.getDistanceResidual().getMMetresValue());//Output value in meters [mm], stored in [m]

		//write the residual/sigam
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDVER.getDistanceResidual()/ItDVER.getObservedStDev());//Output value in meters [mm], stored in [m]
		(*stream)<<endl;

		//for output residual mean and the standart deviation of the residuals
		summary.addNewResidual(ItDVER.getDistanceResidual().getMMetresValue());
	}
	(*stream)<<endl;
	
	//for output residual mean and the standart deviation of the residuals
	writeDistanceResultsSummary(summary, TABs);
}

void TOtherMeasurentWriter::writeORIEResults(const std::vector<TORIE>& fORIE, const TAdjustablePoint& instPos)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					angleResidualPrecision = max(getAngleResidualPrecision() - 4, 0);
	int					anglePrecision = getAnglePrecision();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);


	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kORIE), (int)fORIE.size());
	(*stream) << endl;

	writeORIEResultsHeader(); // write the title line for the observations

	//for output residual mean and the standart deviation of the residuals
	// directly calculate here due to the instrument absence
	TLGCObsSummary summary;

	for (auto const& ItORIE : fORIE)
	{
		(*stream) << TABs;
		//write INST POSITION
		(*stream).writeStringLeft(nameWidth, instPos.getName());
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItORIE.targetPos->getName());

		//write the observed ORIE
		(*stream).writeDouble(obsWidth, anglePrecision, ItORIE.getAngle().getGonsValue());

		//write the sigma ORIE
		(*stream).writeDouble(obsResWidth, angleResidualPrecision, ItORIE.target.sigmaAngl.getSignedCCValue());

		//write the estimated ORIE
		(*stream).writeDouble(obsWidth, anglePrecision, ItORIE.getAngle().getGonsValue() + ItORIE.getAngleResidual().getGonsValue());

		//write the residual
		(*stream).writeDouble(obsResWidth, angleResidualPrecision, ItORIE.getAngleResidual().getSignedCCValue());

		//write the resi/sigma
		(*stream).writeDouble(obsResWidth, angleResidualPrecision, ItORIE.getAngleResidual().getSignedCCValue() / ItORIE.target.sigmaAngl.getSignedCCValue());
		(*stream) << endl;

		//for output residual mean and the standart deviation of the residuals
		summary.addNewResidual(ItORIE.getAngleResidual().getSignedCCValue());
	}
	(*stream) << endl;

	//for output residual mean and the standart deviation of the residuals
	writeDistanceResultsSummary(summary, TABs);
}

void TOtherMeasurentWriter::writeRADIResults(const std::vector<TRADI>& fRADI)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);
	int					anglePrecision = getAnglePrecision();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kRADI), (int)fRADI.size());
	(*stream) << endl;

	writeRADIResultsHeader(); // write the title line for the observations

	//for output residual mean and the standart deviation of the residuals
	// directly calculate here due to the instrument absence
	TLGCObsSummary summary;

	for (auto const& It : fRADI)
	{
		(*stream) << TABs;
		//write INST POSITION
		(*stream).writeString(nameWidth, It.station->getName());

		//write the bearing
		(*stream).writeDouble(obsWidth, anglePrecision, It.getAngleCnstr().getGonsValue());

		//write the sigma 
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getObservedStDev().getMMetresValue());

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getResidual().getMMetresValue());

		//write the residual/sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getResidual() / It.getObservedStDev());
		(*stream) << endl;

		//for output residual mean and the standart deviation of the residuals
		summary.addNewResidual(It.getResidual().getMMetresValue());
	}
	(*stream) << endl;

	//for output residual mean and the standart deviation of the residuals
	writeDistanceResultsSummary(summary, TABs);
}


void TOtherMeasurentWriter::writeDVERSIMUResults(const std::vector<TDVER>& fDVER)
{
	TAStreamFormatter*	stream = getStream();
	//Third hierarchy level from local FRAME
	std::string        TABs = stream->getCurrSpaceExtended(3);

		if (!fDVER.empty()){
			(*stream) << TABs << "DVER" << endl;
			//writeDistanceResultsSummary(fDVER, TABs);
		}
}


void TOtherMeasurentWriter::writeORIESIMUResults(const TORIEROM& fOrieRom)
{
	TAStreamFormatter*	stream = getStream();
	//Third hierarchy level from local FRAME
	std::string        TABs = stream->getCurrSpaceExtended(3);

	//Write definition of ROM
	(*stream) << TABs << "ORIE" << endl;
	writeAngleResultsSummary(fOrieRom.getORIEObsSummary(), TABs);
}
