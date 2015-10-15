#include "TCamConverter.h"
#include "Measurements/TAMeas.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"
#include	"Global.h"
#include "Measurements\TLGCObsSummary.h"

TCamConverter::TCamConverter(TAStreamFormatter& stream):TObservationConverter(stream)
{}

TCamConverter::~TCamConverter(){}


void TCamConverter::writeCAMResults(const TCAM& camera){
	TAStreamFormatter*	stream = getStream();
	std::string        TABs = stream->getCurrSpaceExtended(2);
	writeCAMHeader(camera);
	writeCAMData(camera);


	if(camera.measPLR3D.size() > 0){
			writePLRResults(camera.measPLR3D);
			TPLR3DObsSummary summary = camera.getPLR3DObsSummary();	
			(*stream)<<TABs<<"DIST"<<endl;
			writeDistanceResultsSummary(summary.distObsSum, TABs);
			(*stream)<<TABs<<"ANGL"<<endl;
			writeAngleResultsSummary(summary.anglObsSum, TABs);
			(*stream)<<TABs<<"ZEND"<<endl;
			writeAngleResultsSummary(summary.zendObsSum, TABs);
	}
	if(camera.measDIR3D.size() > 0){
		writeDIRResults(camera.measDIR3D);
		TDIR3DObsSummary summary = camera.getDIR3DObsSummary();	
		(*stream)<<TABs<<"ANGL"<<endl;
		writeAngleResultsSummary(summary.anglObsSum, TABs);
		(*stream)<<TABs<<"ZEND"<<endl;
		writeAngleResultsSummary(summary.zendObsSum, TABs);
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
	(*stream).writeString(obsResWidth,	"ICSE"); //Sigma of the instrument centering
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//third line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //Point on which is the TSTN positioned
	(*stream).writeString(obsResWidth,	"(MM)"); //Sigma of the instrument centering
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
	(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, camera.instrument.sigmaInstrCentering * LGC::M2MM);
	
	(*stream)<<endl<<endl;
}


//Results
//RESULTS
void TCamConverter::writePLRResults(const std::vector<TPLR3D>& measPLR3D)
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
	
	writePLRResultsHeader(measPLR3D.size()); // write the title line for the observations
	for(auto const& ItPLR3D: measPLR3D)
	{
		(*stream)<<TABs;
		//write TARGET ID
		(*stream).writeStringLeft(nameWidth, ItPLR3D.target.ID);
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItPLR3D.targetPos->getName());

		//write the target height
		(*stream).writeDouble(obsWidth, lengthPrecision, ItPLR3D.target.targetHt);

		//write the distance cste (TLength (M))
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
		(*stream)<<ItPLR3D.target.distCorrectionAdjustable->getProvisionalValue()<<separator;

		if(!ItPLR3D.target.distCorrectionAdjustable->isFixed()){
			//write the distance cste calculated(TLength (M))
			stream->setWidthFormat(obsWidth);
			stream->setPrecisionFormat(lengthPrecision);
			(*stream)<<ItPLR3D.target.distCorrectionAdjustable->getEstimatedValue()<<separator;

			//write the distance cste sigma (TLength (MM))
			stream->setWidthFormat(obsResWidth);
			stream->setPrecisionFormat(lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0);
			(*stream)<<ItPLR3D.target.distCorrectionAdjustable->getEstimatedPrecision()*LGC::M2MM<<separator;	//MM2M on scalar
		}
		else{
			(*stream).writeString(obsWidth, "");
			(*stream).writeString(obsResWidth, "");
		}

//DIST 
		//measured DIST value
		(*stream).writeDouble(obsWidth, lengthPrecision, ItPLR3D.getScalar());

		//write the sigma DIST
		//Output sigmadist, Output value in meters [mm], stored in [m], lower the precision because it of the m2mm conversion
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItPLR3D.target.sigmaDist * LGC::M2MM);

		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItPLR3D.getScalar() + ItPLR3D.getScalarResidual());

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItPLR3D.getScalarResidual() * LGC::M2MM);

//ANGL
		//write the observed ANGL
		stream->setAngleUnits(TAngle::kGons);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<< ItPLR3D.getAngle(EPLR3DAngles::kANGL).getSLAngle() <<(separator);	

		//write the sigma ANGL
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<< TAngle(ItPLR3D.target.sigmaAngl) <<(separator);

		//write the estimated ANGL
		stream->setAngleUnits(TAngle::kGons);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<<(ItPLR3D.getAngle(EPLR3DAngles::kANGL) + ItPLR3D.getAngleResidual(EPLR3DAngles::kANGL)).getSLAngle()<<(separator);

		//write the residual ANGL
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<<ItPLR3D.getAngleResidual(EPLR3DAngles::kANGL).getSLAngle()<<(separator);

//ZEND
		//write the observed ZEND
		stream->setAngleUnits(TAngle::kGons);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<< ItPLR3D.getAngle(EPLR3DAngles::kZEND).getSLAngle() <<(separator);	

		//write the sigma ZEND
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<< TAngle(ItPLR3D.target.sigmaAngl) <<(separator);

		//write the estimated ZEND
		stream->setAngleUnits(TAngle::kGons);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<<(ItPLR3D.getAngle(EPLR3DAngles::kZEND) + ItPLR3D.getAngleResidual(EPLR3DAngles::kZEND)).getSLAngle()<<(separator);

		//write the residual ZEND
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<<ItPLR3D.getAngleResidual(EPLR3DAngles::kZEND).getSLAngle()<<(separator);
		(*stream)<<endl;
		}
		(*stream)<<endl;
}


void TCamConverter::writeDIRResults(const std::vector<TDIR3D>& measDIR3D)
{   
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(2);
	
	writeDIRResultsHeader(measDIR3D.size()); // write the title line for the observations
	for(auto const& ItDIR3D: measDIR3D)
	{
		(*stream)<<TABs;
		//write TARGET ID
		(*stream).writeStringLeft(nameWidth, ItDIR3D.target.ID);
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItDIR3D.targetPos->getName());

		//write the target height
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDIR3D.target.targetHt);
//ANGL
		//write the observed ANGL
		stream->setAngleUnits(TAngle::kGons);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<< ItDIR3D.getAngle(EPLR3DAngles::kANGL).getSLAngle() <<(separator);	

		//write the sigma ANGL
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<< TAngle(ItDIR3D.target.sigmaAngl) <<(separator);

		//write the estimated ANGL
		stream->setAngleUnits(TAngle::kGons);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<<(ItDIR3D.getAngle(EPLR3DAngles::kANGL) + ItDIR3D.getAngleResidual(EPLR3DAngles::kANGL)).getSLAngle()<<(separator);

		//write the residual ANGL
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<<ItDIR3D.getAngleResidual(EPLR3DAngles::kANGL).getSLAngle()<<(separator);

//ZEND
		//write the observed ZEND
		stream->setAngleUnits(TAngle::kGons);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<< ItDIR3D.getAngle(EPLR3DAngles::kZEND).getSLAngle() <<(separator);	

		//write the sigma ZEND
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<< TAngle(ItDIR3D.target.sigmaAngl) <<(separator);

		//write the estimated ZEND
		stream->setAngleUnits(TAngle::kGons);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<<(ItDIR3D.getAngle(EPLR3DAngles::kZEND) + ItDIR3D.getAngleResidual(EPLR3DAngles::kZEND)).getSLAngle()<<(separator);

		//write the residual ZEND
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<<ItDIR3D.getAngleResidual(EPLR3DAngles::kZEND).getSLAngle()<<(separator);
		(*stream)<<endl;
	}
	(*stream)<<endl;
}

void TCamConverter::writePLRResultsHeader(int nOObs)
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
		this->writeObsTitle(this->getObsDescriptionEN(TALGCConverter::kPLR3D), nOObs);
		(*stream)<<endl;
		//second line
		(*stream)<<TABs;
		(*stream).writeStringLeft(nameWidth, "TRGT"); //Name of the target used
		(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
		(*stream).writeString(obsWidth,	"HEIGHT"); //target height
		(*stream).writeString(obsWidth,	"CONST"); //provisional dist corr
		(*stream).writeString(obsWidth,	"CONST CALC"); //calculated distance corr
		(*stream).writeString(obsResWidth,	"SCCONST");  //sigma distance corr

		(*stream).writeString(obsWidth,	"OBSDIST"); //mesured DIST
		(*stream).writeString(obsResWidth,	"SDIST"); //sigma DIST
		(*stream).writeString(obsWidth,	"CALCDIST"); //estimated DIST 
		(*stream).writeString(obsResWidth,	"RESDIST"); //residual


		(*stream).writeString(obsWidth,	"OBSANGL"); //mesured ANGL
		(*stream).writeString(obsResWidth,	"SANGL"); //sigma ANGL
		(*stream).writeString(obsWidth,	"CALCANGL"); //estimated ANGL 
		(*stream).writeString(obsResWidth,	"RESANGL"); //residual


		(*stream).writeString(obsWidth,	"OBSZEND"); //mesured ZEND
		(*stream).writeString(obsResWidth,	"SZEND"); //sigma ZEND
		(*stream).writeString(obsWidth,	"CALCZEND"); //estimated ZEND 
		(*stream).writeString(obsResWidth,	"RESZEND"); //residual ZZEND

		(*stream)<<endl;

		(*stream)<<TABs;
		(*stream).writeStringLeft(nameWidth,""); //TARGET ID
		(*stream).writeStringLeft(nameWidth,""); //POSITION
		(*stream).writeString(obsWidth,	"(M)"); //height of the target
		(*stream).writeString(obsWidth,	"(M)"); //provisional dist corr
		(*stream).writeString(obsWidth,	"(M)"); //calculated distance corr
		(*stream).writeString(obsResWidth,	"(MM)"); //sigma distance corr

		(*stream).writeString(obsWidth,	"(M)"); //OBSERVED VALUE
		(*stream).writeString(obsResWidth,"(MM)"); //sigma observed value
		(*stream).writeString(obsWidth,	"(M)"); //estimated DIST
		(*stream).writeString(obsResWidth,	"(MM)"); //residual


		(*stream).writeString(obsWidth,	"(GON)"); //OBSERVED VALUE
		(*stream).writeString(obsResWidth,"(CC)"); //sigma observed value
		(*stream).writeString(obsWidth,	"(GON)"); //estimated ANGL
		(*stream).writeString(obsResWidth,	"(CC)"); //residual

		(*stream).writeString(obsWidth,	"(GON)"); //OBSERVED VALUE
		(*stream).writeString(obsResWidth,"(CC)"); //sigma observed value
		(*stream).writeString(obsWidth,	"(GON)"); //estimated ZEND
		(*stream).writeString(obsResWidth,	"(CC)"); //residual

		(*stream)<<endl;
}

void TCamConverter::writeDIRResultsHeader(int nOObs)
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
	this->writeObsTitle(this->getObsDescriptionEN(TALGCConverter::kDIR3D), nOObs);
	(*stream)<<endl;
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "TRGT"); //Name of the target used
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
	(*stream).writeString(obsWidth,	"HEIGHT"); //target height

	(*stream).writeString(obsWidth,	"OBSANGL"); //mesured ANGL
	(*stream).writeString(obsResWidth,	"SANGL"); //sigma ANGL
	(*stream).writeString(obsWidth,	"CALCANGL"); //estimated ANGL 
	(*stream).writeString(obsResWidth,	"RESANGL"); //residual


	(*stream).writeString(obsWidth,	"OBSZEND"); //mesured ZEND
	(*stream).writeString(obsResWidth,	"SZEND"); //sigma ZEND
	(*stream).writeString(obsWidth,	"CALCZEND"); //estimated ZEND 
	(*stream).writeString(obsResWidth,	"RESZEND"); //residual ZZEND

	(*stream)<<endl;

	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //TARGET ID
	(*stream).writeStringLeft(nameWidth,""); //POSITION
	(*stream).writeString(obsWidth,	"(M)"); //height of the target

	(*stream).writeString(obsWidth,	"(GON)"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth,"(CC)"); //sigma observed value
	(*stream).writeString(obsWidth,	"(GON)"); //estimated DIST
	(*stream).writeString(obsResWidth,	"(CC)"); //residual

	(*stream).writeString(obsWidth,	"(GON)"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth,"(CC)"); //sigma observed value
	(*stream).writeString(obsWidth,	"(GON)"); //estimated DIST
	(*stream).writeString(obsResWidth,	"(CC)"); //residual

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
