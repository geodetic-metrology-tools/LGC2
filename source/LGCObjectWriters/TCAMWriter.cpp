#include "TCAMWriter.h"
#include "TAMeas.h"
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include "Global.h"
#include "TLGCObsSummary.h"

TCAMWriter::TCAMWriter(TAStreamFormatter& stream):TObservationWriter(stream)
{}

TCAMWriter::~TCAMWriter(){}


void TCAMWriter::writeCAMResults(const TCAM& camera){
	TAStreamFormatter*	stream = getStream();
	std::string        TABs = stream->getCurrSpaceExtended(2);
	writeCAMHeader(camera);
	writeCAMData(camera);


	if(camera.measUVD.size() > 0){
			writeUVDResults(camera.measUVD);
			TUVDObsSummary summary = camera.getUVDObsSummary();
			(*stream)<<TABs<<"XVECT"<<endl;
			writeUnitlessResultsSummary(summary.xVectorCompObsSum, TABs);	
			(*stream)<<TABs<<"YVECT"<<endl;
			writeUnitlessResultsSummary(summary.yVectorCompObsSum, TABs); 				
			(*stream)<<TABs<<"DIST"<<endl;
			writeDistanceResultsSummary(summary.distObsSum, TABs);
	}
	if(camera.measUVEC.size() > 0){
		writeUVECResults(camera.measUVEC);
		TUVECObsSummary summary = camera.getUVECObsSummary();	
		(*stream)<<TABs<<"XVECT"<<endl;
		writeUnitlessResultsSummary(summary.xVectorCompObsSum, TABs); 
		(*stream)<<TABs<<"YVECT"<<endl;
		writeUnitlessResultsSummary(summary.yVectorCompObsSum, TABs); 
	}
}

void TCAMWriter::writeCAMResultsSIMU(const TCAM& camera){
	TAStreamFormatter*	stream = getStream();
	std::string        TABs = stream->getCurrSpaceExtended(2);
	writeCAMHeader(camera);
	writeCAMData(camera);


	if(camera.measUVD.size() > 0){
		TUVDObsSummary summary = camera.getUVDObsSummary();
		(*stream)<<TABs<<"XVECT"<<endl;
		writeUnitlessResultsSummary(summary.xVectorCompObsSum, TABs);	
		(*stream)<<TABs<<"YVECT"<<endl;
		writeUnitlessResultsSummary(summary.yVectorCompObsSum, TABs); 				
		(*stream)<<TABs<<"DIST"<<endl;
		writeDistanceResultsSummary(summary.distObsSum, TABs);
	}
	if(camera.measUVEC.size() > 0){
		TUVECObsSummary summary = camera.getUVECObsSummary();	
		(*stream)<<TABs<<"XVECT"<<endl;
		writeUnitlessResultsSummary(summary.xVectorCompObsSum, TABs); 
		(*stream)<<TABs<<"YVECT"<<endl;
		writeUnitlessResultsSummary(summary.yVectorCompObsSum, TABs); 
	}
}

void TCAMWriter::writeCAMHeader(const TCAM& camera){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	string				separator = getSeparator();
	std::string        TABs = stream->getCurrSpaceExtended(1);

	////////////////////////////////////////////////////////////
	//first line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"CAMERA INSTRUMENT: " + camera.instrument.ID);
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"POSITION"); //Point on which is the CAM positioned
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
}

void TCAMWriter::writeCAMData(const TCAM& camera){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	string				separator = getSeparator();
	std::string        TABs = stream->getCurrSpaceExtended(1);

	(*stream)<<TABs;
	//write NAME OF THE POINT ON WHICH STATION IS POSITIONED
	(*stream).writeStringLeft(nameWidth, camera.instrumentPos->getName());	
	(*stream)<<endl<<endl;
}


//Results
//RESULTS
void TCAMWriter::writeUVDResults(const std::vector<TUVD>& measUVD)
{   

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();
	std::string        TABs = stream->getCurrSpaceExtended(2);
	
	//Reset the length residual precision for MM values
	int lengthResPrecision = lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0;

	writeUVDResultsHeader(measUVD.size()); // write the title line for the observations
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
		stream->setLengthUnits(TLength::kMetres); //it is unitless, decide what to use
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getX().getMetresValue());

		//write the sigma X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.target.sigmaX * LGC::M2MM); /*Unitless, delete or replace with appropriate conversion*/

		//write the estimated X vector component
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getX().getMetresValue() + ItUVD.getXCompVectorResidual());

		//write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.getXCompVectorResidual()* LGC::M2MM);/*Unitless, delete or replace with appropriate conversion*/

//Y vector component
		//write the observed Y vector component
		stream->setLengthUnits(TLength::kMetres); //it is unitless, decide what to use
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getY().getMetresValue());

		//write the sigma Y vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.target.sigmaY * LGC::M2MM); /*Unitless, delete or replace with appropriate conversion*/

		//write the estimated Y vector component
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getY().getMetresValue() + ItUVD.getYCompVectorResidual());

		//write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.getYCompVectorResidual()* LGC::M2MM);/*Unitless, delete or replace with appropriate conversion*/

//DISTANCE
		//measured DIST value
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getDistance());

		//write the sigma DIST
		//Output sigmadist, Output value in meters [mm], stored in [m], lower the precision because it of the m2mm conversion
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.target.sigmaDist.getMMetresValue());

		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getDistance() + ItUVD.getDistanceResidual());

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.getDistanceResidual() * LGC::M2MM); /*Unitless, delete or replace with appropriate conversion*/

		(*stream)<<endl;
		}
		(*stream)<<endl;
}

void TCAMWriter::writeUVECResults(const std::vector<TUVEC>& measUVEC)
{   
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(2);
	
	//Reset the length residual precision for MM values
	int lengthResPrecision = lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0;

	writeUVECResultsHeader(measUVEC.size()); // write the title line for the observations
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
		stream->setLengthUnits(TLength::kMetres); //it is unitless, decide what to use
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getX().getMetresValue());

		//write the sigma X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVEC.target.sigmaX* LGC::M2MM);/*Unitless, delete or replace with appropriate conversion*/

		//write the estimated X vector component
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getX().getMetresValue() + ItUVEC.getXCompVectorResidual());

		//write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVEC.getXCompVectorResidual()* LGC::M2MM);/*Unitless, delete or replace with appropriate conversion*/

//Y vector component
		//write the observed Y vector component
		stream->setLengthUnits(TLength::kMetres); //it is unitless, decide what to use
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getY().getMetresValue());

		//write the sigma Y vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVEC.target.sigmaY* LGC::M2MM);/*Unitless, delete or replace with appropriate conversion*/

		//write the estimated Y vector component
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getY().getMetresValue() + ItUVEC.getYCompVectorResidual());

		//write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVEC.getYCompVectorResidual()* LGC::M2MM);/*Unitless, delete or replace with appropriate conversion*/

		(*stream)<<endl;
	}
	(*stream)<<endl;
}

void TCAMWriter::writeUVDResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(2);

		////////////////////////////////////////////////////////////
		//first line
		(*stream)<<TABs;
		this->writeObsTitle(this->getObsDescriptionEN(TCAMWriter::kUVD), nOObs);
		(*stream)<<endl;
		//second line
		(*stream)<<TABs;
		(*stream).writeStringLeft(nameWidth, "TRGT"); //Name of the target used
		(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
		//(*stream).writeString(obsWidth,	"HEIGHT"); //target height
		//(*stream).writeString(obsWidth,	"CONST"); //provisional dist corr
		//(*stream).writeString(obsWidth,	"CONST CALC"); //calculated distance corr
		//(*stream).writeString(obsResWidth,	"SCCONST");  //sigma distance corr

		(*stream).writeString(obsWidth,	"OBSXV"); //mesured x vector component
		(*stream).writeString(obsResWidth,	"SXV"); //sigma x vector component
		(*stream).writeString(obsWidth,	"CALCXV"); //estimated x vector component 
		(*stream).writeString(obsResWidth,	"RESXV"); //residual


		(*stream).writeString(obsWidth,	"OBSYV"); //mesured y vector component
		(*stream).writeString(obsResWidth,	"SYV"); //sigma y vector component
		(*stream).writeString(obsWidth,	"CALCYV"); //estimated y vector component 
		(*stream).writeString(obsResWidth,	"RESYV"); //residual 

		(*stream).writeString(obsWidth,	"OBSDIST"); //mesured DIST
		(*stream).writeString(obsResWidth,	"SDIST"); //sigma DIST
		(*stream).writeString(obsWidth,	"CALCDIST"); //estimated DIST 
		(*stream).writeString(obsResWidth,	"RESDIST"); //residual

		(*stream)<<endl;

		(*stream)<<TABs;
		(*stream).writeStringLeft(nameWidth,""); //TARGET ID
		(*stream).writeStringLeft(nameWidth,""); //POSITION
	//	(*stream).writeString(obsWidth,	"(M)"); //height of the target
	//	(*stream).writeString(obsWidth,	"(M)"); //provisional dist corr
	//	(*stream).writeString(obsWidth,	"(M)"); //calculated distance corr
	//	(*stream).writeString(obsResWidth,	"(MM)"); //sigma distance corr

		(*stream).writeString(obsWidth,	"()"); //OBSERVED VALUE
		(*stream).writeString(obsResWidth,"()"); //sigma observed value
		(*stream).writeString(obsWidth,	"()"); //estimated 
		(*stream).writeString(obsResWidth,	"()"); //residual

		(*stream).writeString(obsWidth,	"()"); //OBSERVED VALUE
		(*stream).writeString(obsResWidth,"()"); //sigma observed value
		(*stream).writeString(obsWidth,	"()"); //estimated 
		(*stream).writeString(obsResWidth,	"()"); //residual
		
		(*stream).writeString(obsWidth,	"(M)"); //OBSERVED VALUE
		(*stream).writeString(obsResWidth,"(MM)"); //sigma observed value
		(*stream).writeString(obsWidth,	"(M)"); //estimated DIST
		(*stream).writeString(obsResWidth,	"(MM)"); //residual
		(*stream)<<endl;
}

void TCAMWriter::writeUVECResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(2);

	////////////////////////////////////////////////////////////
	//first line
	(*stream)<<TABs;
	this->writeObsTitle(this->getObsDescriptionEN(TALGCObjectWriter::kUVEC), nOObs);
	(*stream)<<endl;
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "TRGT"); //Name of the target used
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
	//(*stream).writeString(obsWidth,	"HEIGHT"); //target height

	(*stream).writeString(obsWidth,	"OBSXV"); //mesured x vector component
	(*stream).writeString(obsResWidth,	"SXV"); //sigma x vector component
	(*stream).writeString(obsWidth,	"CALCXV"); //estimated x vector component 
	(*stream).writeString(obsResWidth,	"RESXV"); //residual


	(*stream).writeString(obsWidth,	"OBSYV"); //mesured y vector component
	(*stream).writeString(obsResWidth,	"SYV"); //sigma y vector component
	(*stream).writeString(obsWidth,	"CALCYV"); //estimated y vector component 
	(*stream).writeString(obsResWidth,	"RESYV"); //residual 

	(*stream)<<endl;

	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //TARGET ID
	(*stream).writeStringLeft(nameWidth,""); //POSITION
	//(*stream).writeString(obsWidth,	"(M)"); //height of the target

	(*stream).writeString(obsWidth,	"()"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth,"()"); //sigma observed value
	(*stream).writeString(obsWidth,	"()"); //estimated 
	(*stream).writeString(obsResWidth,	"()"); //residual

	(*stream).writeString(obsWidth,	"()"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth,"()"); //sigma observed value
	(*stream).writeString(obsWidth,	"()"); //estimated 
	(*stream).writeString(obsResWidth,	"()"); //residual

	(*stream)<<endl;
}


//Reliability

void	TCAMWriter::writeUVECReliabilityData(const TCAM& fCam, const TLGCStatistic& stat)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	string				separator = getSeparator();

	//Reset the length residual precision for MM values
	int lengthResPrecision = lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0;

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
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUvec.getVectorValue().getX().getMetresValue());
		//get the sigma of i component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvec.target.sigmaX* LGC::M2MM);
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItUvec.getXCompVectorResidual()* LGC::M2MM);

		writeReliability(index, stat);

//------------------- 2nd obs----------------------------------------------------//
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, "");
		//get Tg point
		(*stream).writeStringLeft(nameWidth, "");
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");
		
		index = index + 1;

		//get the observed j component
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUvec.getVectorValue().getY().getMetresValue());
		//get the sigma of j component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvec.target.sigmaY* LGC::M2MM);
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvec.getYCompVectorResidual()* LGC::M2MM);

		writeReliability(index, stat);

	(*stream).setDataSpacing();
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
	int					lengthResidualPrecision = getLengthResidualPrecision();
	string				separator = getSeparator();

	//Reset the length residual precision for MM values
	int lengthResPrecision = lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0;

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
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUvd.getVectorValue().getX().getMetresValue());
		//get the sigma of i component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvd.target.sigmaX* LGC::M2MM);
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItUvd.getXCompVectorResidual()* LGC::M2MM);

		writeReliability(index, stat);

//------------------- 2nd obs----------------------------------------------------//
		index = index + 1;
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, "");
		//get Tg point
		(*stream).writeStringLeft(nameWidth, "");
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed j component
      (*stream).writeDouble(obsWidth, lengthPrecision, ItUvd.getVectorValue().getY().getMetresValue());
		//get the sigma of j component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvd.target.sigmaY* LGC::M2MM);
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItUvd.getYCompVectorResidual()* LGC::M2MM);

		writeReliability(index, stat);

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
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItUvd.getDistanceResidual()* LGC::M2MM);

		writeReliability(index, stat);

	(*stream).setDataSpacing();
	}
	return;
}


void	TCAMWriter::writeUVECReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("PIVOT","TARGET", "", "OBS_i", "[]", "[]");
	this->TObservationWriter::writeReliabilityHeader("PIVOT","TARGET", "", "OBS_j", "[]", "[]");
	return;
}

void	TCAMWriter::writeUVDReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("PIVOT","TARGET", "", "OBS_i", "[]", "[]");
	this->TObservationWriter::writeReliabilityHeader("","", "", "OBS_j", "[]", "[]");
	this->TObservationWriter::writeReliabilityHeader("","", "", "OBS_dist", "M", "MM");
	return;
}

