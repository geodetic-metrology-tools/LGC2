#include "TCamConverter.h"
#include "Measurements/TAMeas.h"
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include "Global.h"
#include "Measurements\TLGCObsSummary.h"

TCamConverter::TCamConverter(TAStreamFormatter& stream):TObservationConverter(stream)
{}

TCamConverter::~TCamConverter(){}


void TCamConverter::writeCAMResults(const TCAM& camera){
	TAStreamFormatter*	stream = getStream();
	std::string        TABs = stream->getCurrSpaceExtended(2);
	writeCAMHeader(camera);
	writeCAMData(camera);


	if(camera.measUVD.size() > 0){
			writeUVDResults(camera.measUVD);
			TUVDObsSummary summary = camera.getUVDObsSummary();	
			(*stream)<<TABs<<"DIST"<<endl;
			writeDistanceResultsSummary(summary.distObsSum, TABs);
			(*stream)<<TABs<<"XVECT"<<endl;
			writeAngleResultsSummary(summary.xVectorCompObsSum, TABs);
			(*stream)<<TABs<<"YVECT"<<endl;
			writeAngleResultsSummary(summary.yVectorCompObsSum, TABs);
	}
	if(camera.measUVEC.size() > 0){
		writeUVECResults(camera.measUVEC);
		TUVECObsSummary summary = camera.getUVECObsSummary();	
		(*stream)<<TABs<<"XVECT"<<endl;
		writeAngleResultsSummary(summary.xVectorCompObsSum, TABs);
		(*stream)<<TABs<<"YVECT"<<endl;
		writeAngleResultsSummary(summary.yVectorCompObsSum, TABs);
	}
}

void TCamConverter::writeCAMHeader(const TCAM& camera){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string        TABs = stream->getCurrSpaceExtended(1);

	////////////////////////////////////////////////////////////
	//first line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"CAMERA " + camera.instrument.ID);
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"POSITION"); //Point on which is the CAM positioned
	//(*stream).writeString(obsResWidth,	"ICSE"); //Sigma of the instrument centering
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//third line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //Point on which is the CAMERA is positioned
	//(*stream).writeString(obsResWidth,	"(MM)"); //Sigma of the instrument centering
	(*stream)<<endl;
}

void TCamConverter::writeCAMData(const TCAM& camera){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	string				separator = getSeparator();
	std::string        TABs = stream->getCurrSpaceExtended(1);

	(*stream)<<TABs;
	//write NAME OF THE POINT ON WHICH STATION IS POSITIONED
	(*stream).writeStringLeft(nameWidth, camera.instrumentPos->getName());

	//write the instrument centering
    //Output sigma instrument centering, Output value in meters [mm], stored in [m], lower the precision because it of the m2mm conversion
//	(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, camera.instrument.sigmaInstrCentering * LGC::M2MM);
	
	(*stream)<<endl<<endl;
}


//Results
//RESULTS
void TCamConverter::writeUVDResults(const std::vector<TUVD>& measUVD)
{   

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();
	std::string        TABs = stream->getCurrSpaceExtended(2);
	
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

//DISTANCE
		//measured DIST value
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getDistance());

		//write the sigma DIST
		//Output sigmadist, Output value in meters [mm], stored in [m], lower the precision because it of the m2mm conversion
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItUVD.target.sigmaDist * LGC::M2MM);

		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getDistance() + ItUVD.getDistanceResidual());

		//write the residual
		//(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0,ItUVD.getDistanceResidual() * LGC::M2MM);

//X component
		//write the observed X vector component
		stream->setLengthUnits(TLength::kMetres); //it is unitless, decide what to use
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getX().getValue());

		//write the sigma X vector component
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision, ItUVD.target.sigmaX);

		//write the estimated X vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getX().getValue() + ItUVD.getXCompVectorResidual());

		//write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision, ItUVD.getXCompVectorResidual());

//Y vector component
		//write the observed Y vector component
		stream->setLengthUnits(TLength::kMetres); //it is unitless, decide what to use
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getY().getValue());

		//write the sigma Y vector component
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision, ItUVD.target.sigmaY);

		//write the estimated Y vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getY().getValue() + ItUVD.getYCompVectorResidual());

		//write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision, ItUVD.getYCompVectorResidual());

		(*stream)<<endl;
		}
		(*stream)<<endl;
}


void TCamConverter::writeUVECResults(const std::vector<TUVEC>& measUVEC)
{   
#if 1
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					anglePrecision = getAnglePrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(2);
	
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
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getX().getValue());

		//write the sigma X vector component
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision, ItUVEC.target.sigmaX);

		//write the estimated X vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getX().getValue() + ItUVEC.getXCompVectorResidual());

		//write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision, ItUVEC.getXCompVectorResidual());

//Y vector component
		//write the observed Y vector component
		stream->setLengthUnits(TLength::kMetres); //it is unitless, decide what to use
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getY().getValue());

		//write the sigma Y vector component
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision, ItUVEC.target.sigmaY);

		//write the estimated Y vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getY().getValue() + ItUVEC.getYCompVectorResidual());

		//write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision, ItUVEC.getYCompVectorResidual());
	}
	(*stream)<<endl;
#endif
}

void TCamConverter::writeUVDResultsHeader(int nOObs)
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
		this->writeObsTitle(this->getObsDescriptionEN(TALGCConverter::kUVD), nOObs);
		(*stream)<<endl;
		//second line
		(*stream)<<TABs;
		(*stream).writeStringLeft(nameWidth, "TRGT"); //Name of the target used
		(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
		//(*stream).writeString(obsWidth,	"HEIGHT"); //target height
		//(*stream).writeString(obsWidth,	"CONST"); //provisional dist corr
		//(*stream).writeString(obsWidth,	"CONST CALC"); //calculated distance corr
		//(*stream).writeString(obsResWidth,	"SCCONST");  //sigma distance corr

		(*stream).writeString(obsWidth,	"OBSDIST"); //mesured DIST
		(*stream).writeString(obsResWidth,	"SDIST"); //sigma DIST
		(*stream).writeString(obsWidth,	"CALCDIST"); //estimated DIST 
	//	(*stream).writeString(obsResWidth,	"RESDIST"); //residual

		(*stream).writeString(obsWidth,	"XVECOB"); //mesured x vector component
		(*stream).writeString(obsResWidth,	"SXVEC"); //sigma x vector component
		(*stream).writeString(obsWidth,	"XVECCAL"); //estimated x vector component 
		(*stream).writeString(obsResWidth,	"RESXV"); //residual


		(*stream).writeString(obsWidth,	"YVECOB"); //mesured y vector component
		(*stream).writeString(obsResWidth,	"SYVEC"); //sigma y vector component
		(*stream).writeString(obsWidth,	"YVECCAL"); //estimated y vector component 
		(*stream).writeString(obsResWidth,	"RESYV"); //residual 

		(*stream)<<endl;

		(*stream)<<TABs;
		(*stream).writeStringLeft(nameWidth,""); //TARGET ID
		(*stream).writeStringLeft(nameWidth,""); //POSITION
	//	(*stream).writeString(obsWidth,	"(M)"); //height of the target
	//	(*stream).writeString(obsWidth,	"(M)"); //provisional dist corr
	//	(*stream).writeString(obsWidth,	"(M)"); //calculated distance corr
	//	(*stream).writeString(obsResWidth,	"(MM)"); //sigma distance corr

		(*stream).writeString(obsWidth,	"(M)"); //OBSERVED VALUE
		(*stream).writeString(obsResWidth,"(MM)"); //sigma observed value
		(*stream).writeString(obsWidth,	"(M)"); //estimated DIST
	//	(*stream).writeString(obsResWidth,	"(MM)"); //residual


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

void TCamConverter::writeUVECResultsHeader(int nOObs)
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
	this->writeObsTitle(this->getObsDescriptionEN(TALGCConverter::kUVEC), nOObs);
	(*stream)<<endl;
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "TRGT"); //Name of the target used
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
	//(*stream).writeString(obsWidth,	"HEIGHT"); //target height

	(*stream).writeString(obsWidth,	"XVECOB"); //mesured x vector component
	(*stream).writeString(obsResWidth,	"SXVEC"); //sigma x vector component
	(*stream).writeString(obsWidth,	"XVECCAL"); //estimated x vector component 
	(*stream).writeString(obsResWidth,	"RESXV"); //residual


	(*stream).writeString(obsWidth,	"YVECOB"); //mesured y vector component
	(*stream).writeString(obsResWidth,	"SYVEC"); //sigma y vector component
	(*stream).writeString(obsWidth,	"YVECCAL"); //estimated y vector component 
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

#if 0
//Reliability
void	TCamConverter::writeReliabilityData(const TCAM& fCam)
{
	
}

void	TCamConverter::writeReliabilityHeader()
{}
#endif
