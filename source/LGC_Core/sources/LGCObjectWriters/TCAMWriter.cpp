#include "TCAMWriter.h"
#include <TAMeas.h>
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include <Global.h>

TCAMWriter::TCAMWriter(TAStreamFormatter& stream, bool /*hist*/) :TObservationWriter(stream)
{}

TCAMWriter::~TCAMWriter(){}

//------------------ Result header---------------------------------------------------------------------------
void TCAMWriter::writeCAMResults(const TCAM& camera){
	writeCAMHeader(camera);

	if(camera.measUVD.size() > 0)
		writeUVDResults(camera.measUVD);

	if(camera.measUVEC.size() > 0)
		writeUVECResults(camera.measUVEC);
}

void TCAMWriter::writeCAMHeader(const TCAM& camera){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	std::string				separator = getSeparator();
	std::string        TABs = stream->getCurrSpaceExtended(1);

	////////////////////////////////////////////////////////////
	//first line
	(*stream)<<endl<<TABs;
	(*stream).writeStringLeft(nameWidth,"CAMERA INSTRUMENT: " + camera.instrument.ID);
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"POSITION"); //Point on which is the CAM positioned
	(*stream).writeStringLeft(nameWidth, camera.instrumentPos->getName());
	(*stream)<<endl<<endl;
	///////////////////////////////////////////////////////////////////////////////////
}

void TCAMWriter::writeUVDResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	std::string         TABs = stream->getCurrSpaceExtended(2);

	////////////////////////////////////////////////////////////
	//first line
	(*stream) << TABs;
	this->writeObsTitle(this->getObsDescriptionFR(TCAMWriter::kUVD), nOObs);
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "TRGT"); //Name of the target used
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
	//(*stream).writeString(obsWidth,	"HEIGHT"); //target height
	//(*stream).writeString(obsWidth,	"CONST"); //provisional dist corr
	//(*stream).writeString(obsWidth,	"CONST CALC"); //calculated distance corr
	//(*stream).writeString(obsResWidth,	"SCCONST");  //sigma distance corr

	(*stream).writeString(obsWidth, "OBSXV"); //mesured x vector component
	(*stream).writeString(obsResWidth, "SXV"); //sigma x vector component
	(*stream).writeString(obsWidth, "CALCXV"); //estimated x vector component 
	(*stream).writeString(obsResWidth, "RESXV"); //residual


	(*stream).writeString(obsWidth, "OBSYV"); //mesured y vector component
	(*stream).writeString(obsResWidth, "SYV"); //sigma y vector component
	(*stream).writeString(obsWidth, "CALCYV"); //estimated y vector component 
	(*stream).writeString(obsResWidth, "RESYV"); //residual 

	(*stream).writeString(obsWidth, "OBSDIST"); //mesured DIST
	(*stream).writeString(obsResWidth, "SDIST"); //sigma DIST
	(*stream).writeString(obsWidth, "CALCDIST"); //estimated DIST 
	(*stream).writeString(obsResWidth, "RESDIST"); //residual

	(*stream) << endl;

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); //TARGET ID
	(*stream).writeStringLeft(nameWidth, ""); //POSITION
	//	(*stream).writeString(obsWidth,	"(M)"); //height of the target
	//	(*stream).writeString(obsWidth,	"(M)"); //provisional dist corr
	//	(*stream).writeString(obsWidth,	"(M)"); //calculated distance corr
	//	(*stream).writeString(obsResWidth,	"(MM)"); //sigma distance corr

	(*stream).writeString(obsWidth, "()"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth, "()"); //sigma observed value
	(*stream).writeString(obsWidth, "()"); //estimated 
	(*stream).writeString(obsResWidth, "()"); //residual

	(*stream).writeString(obsWidth, "()"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth, "()"); //sigma observed value
	(*stream).writeString(obsWidth, "()"); //estimated 
	(*stream).writeString(obsResWidth, "()"); //residual

	(*stream).writeString(obsWidth, "(M)"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth, "(MM)"); //sigma observed value
	(*stream).writeString(obsWidth, "(M)"); //estimated DIST
	(*stream).writeString(obsResWidth, "(MM)"); //residual
	(*stream) << endl;
}

void TCAMWriter::writeUVECResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	std::string         TABs = stream->getCurrSpaceExtended(2);

	////////////////////////////////////////////////////////////
	//first line
	(*stream) << TABs;
	this->writeObsTitle(this->getObsDescriptionFR(TALGCObjectWriter::kUVEC), nOObs);
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "TRGT"); //Name of the target used
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
	//(*stream).writeString(obsWidth,	"HEIGHT"); //target height

	(*stream).writeString(obsWidth, "OBSXV"); //mesured x vector component
	(*stream).writeString(obsResWidth, "SXV"); //sigma x vector component
	(*stream).writeString(obsWidth, "CALCXV"); //estimated x vector component 
	(*stream).writeString(obsResWidth, "RESXV"); //residual


	(*stream).writeString(obsWidth, "OBSYV"); //mesured y vector component
	(*stream).writeString(obsResWidth, "SYV"); //sigma y vector component
	(*stream).writeString(obsWidth, "CALCYV"); //estimated y vector component 
	(*stream).writeString(obsResWidth, "RESYV"); //residual 

	(*stream) << endl;

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); //TARGET ID
	(*stream).writeStringLeft(nameWidth, ""); //POSITION
	//(*stream).writeString(obsWidth,	"(M)"); //height of the target

	(*stream).writeString(obsWidth, "()"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth, "()"); //sigma observed value
	(*stream).writeString(obsWidth, "()"); //estimated 
	(*stream).writeString(obsResWidth, "()"); //residual

	(*stream).writeString(obsWidth, "()"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth, "()"); //sigma observed value
	(*stream).writeString(obsWidth, "()"); //estimated 
	(*stream).writeString(obsResWidth, "()"); //residual

	(*stream) << endl;
}

//------------------ Result data---------------------------------------------------------------------------
void TCAMWriter::writeUVDResults(const std::list<TUVD>& measUVD)
{   

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision()-3, 0);
	int					lengthPrecision =	getLengthPrecision();
	std::string         TABs = stream->getCurrSpaceExtended(2);

    writeUVDResultsHeader((int)measUVD.size()); // write the title line for the observations
	(*stream) << endl;
	for(auto const& ItUVD: measUVD)
	{
		(*stream)<<TABs;
		//write TARGET ID
		(*stream).writeStringLeft(nameWidth, ItUVD.target.ID);
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItUVD.targetPos->getName());

		//write the target height
		//(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.target.targetHt);

		//write the distance cste (TLength (M))
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
	//	(*stream)<<ItUVD.target.distCorrectionAdjustable->getProvisionalValue()<<separator;

//X component
		//write the observed X vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getX().getMetresValue());

		//write the sigma X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.target.sigmaX * M2MM); /*Unitless, delete or replace with appropriate conversion*/

		//write the estimated X vector component
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getX().getMetresValue() + ItUVD.getXCompVectorResidual());

		//write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.getXCompVectorResidual()* M2MM);/*Unitless, delete or replace with appropriate conversion*/

//Y vector component
		//write the observed Y vector component
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getY().getMetresValue());

		//write the sigma Y vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.target.sigmaY * M2MM); /*Unitless, delete or replace with appropriate conversion*/

		//write the estimated Y vector component
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getY().getMetresValue() + ItUVD.getYCompVectorResidual());

		//write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.getYCompVectorResidual()* M2MM);/*Unitless, delete or replace with appropriate conversion*/

//DISTANCE
		//measured DIST value
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getDistance());

		//write the sigma DIST
		//Output sigmadist, Output value in meters [mm], stored in [m], lower the precision because it of the m2mm conversion
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.target.sigmaDist.getMMetresValue());

		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getDistance() + ItUVD.getDistanceResidual());

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.getDistanceResidual().getMMetresValue()); /*Unitless, delete or replace with appropriate conversion*/

		(*stream)<<endl;
		}
		(*stream)<<endl;
}

void TCAMWriter::writeUVECResults(const std::list<TUVEC>& measUVEC)
{   
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision()-3, 0);
	int					lengthPrecision =	getLengthPrecision();
	std::string         TABs = stream->getCurrSpaceExtended(2);


   writeUVECResultsHeader((int)measUVEC.size()); // write the title line for the observations
   (*stream) << endl;
	for(auto const& ItUVEC: measUVEC)
	{
		(*stream)<<TABs;
		//write TARGET ID
		(*stream).writeStringLeft(nameWidth, ItUVEC.target.ID);
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItUVEC.targetPos->getName());

		//write the target height
		//(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.target.targetHt);


//X component
		//write the observed X vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getX());

		//write the sigma X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVEC.target.sigmaX* M2MM);/*Unitless, delete or replace with appropriate conversion*/

		//write the estimated X vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getX() + ItUVEC.getXCompVectorResidual());

		//write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVEC.getXCompVectorResidual()* M2MM);/*Unitless, delete or replace with appropriate conversion*/

//Y vector component
		//write the observed Y vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getY());

		//write the sigma Y vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVEC.target.sigmaY* M2MM);/*Unitless, delete or replace with appropriate conversion*/

		//write the estimated Y vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getY() + ItUVEC.getYCompVectorResidual());

		//write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVEC.getYCompVectorResidual()* M2MM);/*Unitless, delete or replace with appropriate conversion*/

		(*stream)<<endl;
	}
	(*stream)<<endl;

}

//------------------ Simu data---------------------------------------------------------------------------

void TCAMWriter::writeCAMResultsSIMU(const TCAM& camera){
	TAStreamFormatter*	stream = getStream();
	std::string        TABs = stream->getCurrSpaceExtended(2);
	writeCAMHeader(camera);

	if (camera.measUVD.size() > 0){
		const auto &summary = camera.getUVDObsSummary();
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kUVD) + ": XVECT", (int)camera.measUVD.size());
		writeUnitlessResultsSummary(summary.xVectorCompObsSum, TABs);
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kUVD) + ": YVECT", (int)camera.measUVD.size());
		writeUnitlessResultsSummary(summary.yVectorCompObsSum, TABs);
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kUVD) + ": DIST", (int)camera.measUVD.size());
		writeDistanceResultsSummary(summary.distObsSum, TABs);
	}
	if (camera.measUVEC.size() > 0){
		const auto &summary = camera.getUVECObsSummary();
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kUVEC) + ": XVECT", (int)camera.measUVEC.size());
		writeUnitlessResultsSummary(summary.xVectorCompObsSum, TABs);
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kUVEC) + ": YVECT", (int)camera.measUVEC.size());
		writeUnitlessResultsSummary(summary.yVectorCompObsSum, TABs);
	}
}

//------------------ Reliability header--------------------------------------------------------------------
void	TCAMWriter::writeUVECReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("PIVOT", "TARGET", "", "OBS_i", "[]", "[]");
	this->TObservationWriter::writeReliabilityHeader("PIVOT", "TARGET", "", "OBS_j", "[]", "[]");
	return;
}

void	TCAMWriter::writeUVDReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("PIVOT", "TARGET", "", "OBS_i", "[]", "[]");
	this->TObservationWriter::writeReliabilityHeader("", "", "", "OBS_j", "[]", "[]");
	this->TObservationWriter::writeReliabilityHeader("", "", "", "OBS_dist", "M", "MM");
	return;
}


//------------------ Reliability data---------------------------------------------------------------------------

void	TCAMWriter::writeUVECReliabilityData(const TCAM& fCam, const TLGCStatistic& stat)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = std::max(getLengthResidualPrecision()-3, 0);


	//For each DHOR measurement of the station
	for(auto const& ItUvec : fCam.measUVEC)
	{
//------------------- 1rst obs----------------------------------------------------//
		// Observation index to take the right value in the statistic vector
		int index = ItUvec.getFirstObservationIndex();
		
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, fCam.instrumentPos->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItUvec.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed i component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUvec.getVectorValue().getX());
		//get the sigma of i component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvec.target.sigmaX* M2MM);
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItUvec.getXCompVectorResidual()* M2MM);

		writeReliabilityMM(index, stat);

//------------------- 2nd obs----------------------------------------------------//
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, "");
		//get Tg point
		(*stream).writeStringLeft(nameWidth, "");
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");
		
		index = index + 1;

		//get the observed j component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUvec.getVectorValue().getY());
		//get the sigma of j component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvec.target.sigmaY* M2MM);
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvec.getYCompVectorResidual()* M2MM);

		writeReliabilityMM(index, stat);
	}
	return;
}

void	TCAMWriter::writeUVDReliabilityData(const TCAM& fCam, const TLGCStatistic& stat)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = std::max(getLengthResidualPrecision()-3, 0);

	//For each DHOR measurement of the station
	for(auto const& ItUvd : fCam.measUVD)
	{
//------------------- 1rst obs----------------------------------------------------//
		// Observation index to take the right value in the statistic vector
		int index = ItUvd.getFirstObservationIndex();
		
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, fCam.instrumentPos->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItUvd.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed i component
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUvd.getVectorValue().getX());
		//get the sigma of i component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvd.target.sigmaX* M2MM);
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItUvd.getXCompVectorResidual()* M2MM);

		writeReliabilityMM(index, stat);

//------------------- 2nd obs----------------------------------------------------//
		index = index + 1;
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, "");
		//get Tg point
		(*stream).writeStringLeft(nameWidth, "");
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed j component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUvd.getVectorValue().getY());
		//get the sigma of j component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvd.target.sigmaY* M2MM);
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItUvd.getYCompVectorResidual()* M2MM);

		writeReliabilityMM(index, stat);

//------------------- 3rd obs----------------------------------------------------//
		index = index + 1;
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, "");
		//get Tg point
		(*stream).writeStringLeft(nameWidth, "");
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision,ItUvd.getDistance());
		//get the sigma of dist component
		 (*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvd.target.sigmaDist.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItUvd.getDistanceResidual().getMMetresValue());

		writeReliabilityMM(index, stat);
	}
	return;
}


//------------------ Synthesis header-------------------------------------------------------------------------
void TCAMWriter::writeUVDSynthesisHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
    // int				obsWidth = getObsWidth();
    int					obsResWidth = getObsResWidth();
	std::string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	(*stream).writeStringLeft(nameWidth, "UVD"); //instrument
	(*stream) << endl;
	////////////////////////////////////////////////////////////
	//First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); //instrument
	(*stream).writeString(obsResWidth, "RES_MAX"); //residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); //residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); //residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); //ecart type
	(*stream).writeString(obsResWidth, "RES_MAX"); //residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); //residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); //residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); //ecart type
	(*stream).writeString(obsResWidth, "RES_MAX"); //residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); //residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); //residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); //ecart type
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "d(MM)");
	(*stream).writeString(obsResWidth, "d(MM)");
	(*stream).writeString(obsResWidth, "d(MM)");
	(*stream).writeString(obsResWidth, "d(MM)");
	(*stream) << endl;
}

void TCAMWriter::writeUVECSynthesisHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
    // int				obsWidth = getObsWidth();
    int					obsResWidth = getObsResWidth();
	std::string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	(*stream).writeStringLeft(nameWidth, "UVEC"); //instrument
	(*stream) << endl;
	////////////////////////////////////////////////////////////
	//First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); //instrument
	(*stream).writeString(obsResWidth, "RES_MAX"); //residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); //residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); //residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); //ecart type
	(*stream).writeString(obsResWidth, "RES_MAX"); //residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); //residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); //residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); //ecart type
	(*stream) << endl;
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "y");
	(*stream) << endl;

	(*stream) << endl;
}

//------------------ Synthesis data--------------------------------------------------------------------------
void TCAMWriter::writeUVECResultsSynthesis(const TCAM& camera)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
    // int				obsWidth = getObsWidth();
    int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
    // int				lengthPrecision = getLengthPrecision();
    std::string         TABs = stream->getCurrSpaceExtended(1);

    const auto &uvecXSummary = camera.getUVECObsSummary().xVectorCompObsSum;
    const auto &uvecYSummary = camera.getUVECObsSummary().yVectorCompObsSum;

	//X
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, camera.instrumentPos->getName()); //Reference point
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvecXSummary.getResMax());//residu max
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvecXSummary.getResMin());//residu min
	(*stream).writeDouble(obsResWidth, lengthResPrecision, uvecXSummary.getMean());//residu moy
	(*stream).writeDouble(obsResWidth, lengthResPrecision, uvecXSummary.getVariance());//ecart type
	//Y
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvecYSummary.getResMax());//residu max
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvecYSummary.getResMin());//residu min
	(*stream).writeDouble(obsResWidth, lengthResPrecision, uvecYSummary.getMean());//residu moy
	(*stream).writeDouble(obsResWidth, lengthResPrecision, uvecYSummary.getVariance());//ecart type
	(*stream) << endl;


}

void TCAMWriter::writeUVDResultsSynthesis(const TCAM& camera)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
    // int				obsWidth = getObsWidth();
    int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
    // int				lengthPrecision = getLengthPrecision();
    std::string         TABs = stream->getCurrSpaceExtended(1);

    const auto &uvdXSummary = camera.getUVDObsSummary().xVectorCompObsSum;
    const auto &uvdYSummary = camera.getUVDObsSummary().yVectorCompObsSum;
    const auto &uvdDSummary = camera.getUVDObsSummary().distObsSum;

	//X
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, camera.instrumentPos->getName()); //Reference point
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvdXSummary.getResMax());//residu max
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvdXSummary.getResMin());//residu min
	(*stream).writeDouble(obsResWidth, lengthResPrecision, uvdXSummary.getMean());//residu moy
	(*stream).writeDouble(obsResWidth, lengthResPrecision, uvdXSummary.getVariance());//ecart type
	//Y
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvdYSummary.getResMax());//residu max
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvdYSummary.getResMin());//residu min
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvdYSummary.getMean());//residu moy
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvdYSummary.getVariance());//ecart type
	//D
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvdDSummary.getResMax());//residu max
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvdDSummary.getResMin());//residu min
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvdDSummary.getMean());//residu moy
    (*stream).writeDouble(obsResWidth, lengthResPrecision, uvdDSummary.getVariance());//ecart type
	(*stream) << endl;


}