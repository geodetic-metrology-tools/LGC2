#include	"TTstnConverter.h"
#include	"Measurements/TAMeas.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"
#include	"Global.h"
#include "Measurements\TLGCObsSummary.h"


TTstnConverter::TTstnConverter(TAStreamFormatter& stream): TObservationConverter(stream)
{}

TTstnConverter::~TTstnConverter(){}


void TTstnConverter::writeTSTNResults(const TTSTN& tstn){
	TAStreamFormatter*	stream = getStream();
	//Third hierarchy level from local FRAME
	std::string        TABs = stream->getCurrSpaceExtended(3);

	//Write definition of TSTN
	writeTSTNHeader(tstn);
	writeTSTNData(tstn);

	for(auto const ItRoms : tstn.roms)
	{
		//Write definition of ROM
		writeV0Header();
		writeV0Data(ItRoms);

		if(ItRoms.measANGL.size() > 0){
			writeANGLResults(ItRoms.measANGL);
			writeAngleResultsSummary(ItRoms.getANGLObsSummary(),TABs);
		}
		if(ItRoms.measZEND.size() > 0){
			writeZENDResults(ItRoms.measZEND);
			writeAngleResultsSummary(ItRoms.getZENDObsSummary(),TABs);
		}
		if(ItRoms.measDIST.size() > 0){
			writeDISTResults(ItRoms.measDIST);
			writeDistanceResultsSummary(ItRoms.getDISTObsSummary(),TABs);
		}
		if(ItRoms.measDHOR.size() > 0){
			writeDHORResults(ItRoms.measDHOR);
			writeDistanceResultsSummary(ItRoms.getDHORObsSummary(),TABs);
		}
		if(ItRoms.measPLR3D.size() > 0){
			writePLRResults(ItRoms.measPLR3D);
			TPLR3DObsSummary summary = ItRoms.getPLR3DObsSummary();	
			(*stream)<<TABs<<"DIST"<<endl;
			writeDistanceResultsSummary(summary.distObsSum,TABs);
			(*stream)<<TABs<<"ANGL"<<endl;
			writeAngleResultsSummary(summary.anglObsSum, TABs);
			(*stream)<<TABs<<"ZEND"<<endl;
			writeAngleResultsSummary(summary.zendObsSum,TABs);
		}
		if(ItRoms.measDIR3D.size() > 0){
			writeDIRResults(ItRoms.measDIR3D);
			TDIR3DObsSummary summary = ItRoms.getDIR3DObsSummary();	
			(*stream)<<TABs<<"ANGL"<<endl;
			writeAngleResultsSummary(summary.anglObsSum,TABs);
			(*stream)<<TABs<<"ZEND"<<endl;
			writeAngleResultsSummary(summary.zendObsSum,TABs);
		}
	}
}
//RESULTS
void TTstnConverter::writePLRResults(const std::vector<TPLR3D>& measPLR3D)
{   
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();
	std::string        TABs = stream->getCurrSpaceExtended(3);
	
	writePLRResultsHeader(measPLR3D.size()); // write the title line for the observations
	for(auto const& ItPLR3D: measPLR3D)
	{
		(*stream)<<TABs;
		//write TARGET ID
		(*stream).writeStringLeft(nameWidth, ItPLR3D.target.ID);
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItPLR3D.targetPos->getName());

		//write the target height
		(*stream).writeDouble(obsWidth,lengthPrecision, ItPLR3D.target.targetHt); 

		//write the distance cste (TLength (M))
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
		(*stream)<<ItPLR3D.target.distCorrectionAdjustable->getProvisionalValue()<<separator;

		if(!ItPLR3D.target.distCorrectionAdjustable->isFixed()){
			//write the distance cste calculated(TLength (M))
			stream->setWidthFormat(obsWidth);
			stream->setPrecisionFormat(lengthPrecision);
			(*stream)<<ItPLR3D.target.distCorrectionAdjustable->getEstimatedValue()<<separator;

			//write the distance cste sigma (TScalar in (MM))
			stream->setWidthFormat(obsResWidth);
			stream->setPrecisionFormat(lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0); //Output value is in [mm], lower the precision by 3
			(*stream)<<ItPLR3D.target.distCorrectionAdjustable->getEstimatedPrecision()*LGC::M2MM<<separator;	//MM2M on scalar
		}
		else{
			(*stream).writeString(obsWidth, "");
			(*stream).writeString(obsResWidth, "");
		}

//DIST 
		(*stream).writeDouble(obsWidth, lengthPrecision, ItPLR3D.getScalar()); //Write value in meters [m]


		//write the residual, output the value in [mm],  value is stored in [m], lower the precision by 3 decimal points because of the mm output
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItPLR3D.target.sigmaDist * LGC::M2MM); 

		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItPLR3D.getScalar() + ItPLR3D.getScalarResidual()); 

		//write the residual, output the value in [mm],  value is stored in [m], lower the precision by 3 decimal points because of the mm output
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
void TTstnConverter::writeDIRResults(const std::vector<TDIR3D>& measDIR3D)
{   
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();
	std::string        TABs = stream->getCurrSpaceExtended(3);
	
	writeDIRResultsHeader(measDIR3D.size()); // write the title line for the observations
	for(auto const& ItDIR3D: measDIR3D)
	{
		(*stream)<<TABs;
		//write TARGET ID
		(*stream).writeStringLeft(nameWidth, ItDIR3D.target.ID);
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItDIR3D.targetPos->getName());

		//write the target height
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDIR3D.target.targetHt); //Write value in meters [m]

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

void TTstnConverter::writeANGLResults(const std::vector<TANGL>& measANGL)
{   
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResPrecicion = getAngleResidualPrecision();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	string				separator = getSeparator();
	std::string        TABs = stream->getCurrSpaceExtended(3);



	writeANGLResultsHeader(measANGL.size()); // write the title line for the observations
	for(auto const& ItANGL : measANGL)
	{
		(*stream)<<TABs;
		//write TARGET ID
		(*stream).writeStringLeft(nameWidth, ItANGL.target.ID);
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItANGL.targetPos->getName());

		//write the sigma target centering
		//Output value is in [mm], stored in [m] lower the precision by 3
		(*stream).writeDouble( obsResWidth , lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItANGL.target.sigmaTargetCentering * LGC::M2MM); 


		//write the observed ANGL
		stream->setAngleUnits(TAngle::kGons);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<< (ItANGL.getAngle().getSLAngle()) <<(separator);	//We ge LGC::TAngle not TAngle can not format... At least make it TAngle to apply width fromat and precision format....

		//write the sigma ANGL
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<< TAngle(ItANGL.target.sigmaAngl) <<(separator);

		//write the estimated ANGL
		stream->setAngleUnits(TAngle::kGons);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<<(ItANGL.getAngle() + ItANGL.getAngleResidual()).getSLAngle()<<(separator);

		//write the residual
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(angleResPrecicion);
		(*stream)<<ItANGL.getAngleResidual().getSLAngle()<<(separator);

		//write the offset / sigma (TDouble (MM))
		//stream->setPrecisionFormat(2);
		//(*stream)<<(obsIt->getResDivSig())<<separator;

		//write the sensitivity (TReal (MM/CM))
		//(*stream)<<(obsIt->getSensitivity())<<separator;

		//write the distance
	//	stream->setLengthUnits(TLength::kMetres);
	//	stream->setWidthFormat(obsWidth);
	//	(*stream)<<(obsIt->getStTgDistance())<<separator;
		(*stream)<<endl;
	}
	(*stream)<<endl;


}

void TTstnConverter::writeZENDResults(const std::vector<TZEND>& measZEND)
{   
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResPrecicion = getAngleResidualPrecision();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);



	writeZENDResultsHeader(measZEND.size()); // write the title line for the observations
	for(auto const& ItZEND : measZEND)
	{
		(*stream)<<TABs;
		//write TARGET ID
		(*stream).writeStringLeft(nameWidth, ItZEND.target.ID);
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItZEND.targetPos->getName());

		//write the target height
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
		writeDouble(obsWidth, lengthPrecision, ItZEND.target.targetHt);

		//write the target height sigma
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItZEND.target.sigmaTargetHt * LGC::M2MM);
		//write the sigma target centering
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItZEND.target.sigmaTargetCentering * LGC::M2MM);

		//write the observed ZEND
		stream->setAngleUnits(TAngle::kGons);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<< (ItZEND.getAngle().getSLAngle()) <<(separator);	//We ge LGC::TAngle not TAngle can not format... At least make it TAngle to apply width fromat and precision format....

		//write the sigma ZEND
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<< TAngle(ItZEND.target.sigmaZenD) <<(separator);

		//write the estimated ZEND
		stream->setAngleUnits(TAngle::kGons);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(anglePrecision);
		(*stream)<<(ItZEND.getAngle() + ItZEND.getAngleResidual()).getSLAngle()<<(separator);

		//write the residual
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(angleResPrecicion);
		(*stream)<<ItZEND.getAngleResidual().getSLAngle()<<(separator);


		//write the offset / sigma (TDouble (MM))
		//stream->setPrecisionFormat(2);
		//(*stream)<<(obsIt->getResDivSig())<<separator;

		//write the sensitivity (TReal (MM/CM))
		//(*stream)<<(obsIt->getSensitivity())<<separator;

		//write the distance
	//	stream->setLengthUnits(TLength::kMetres);
	//	stream->setWidthFormat(obsWidth);
	//	(*stream)<<(obsIt->getStTgDistance())<<separator;
		(*stream)<<endl<<endl;
	}
}

void TTstnConverter::writeDISTResults(const std::vector<TLINE>& measDIST)
{   

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	writeDISTResultsHeader(measDIST.size()); // write the title line for the observations
	for(auto const& ItDIST : measDIST)
	{
		(*stream)<<TABs;
		//write TARGET ID
		(*stream).writeStringLeft(nameWidth, ItDIST.target.ID);
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItDIST.targetPos->getName());

		//write the target height
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
		(*stream).writeDouble(obsWidth, lengthPrecision > 3 ? (lengthPrecision - 3) : 0, ItDIST.target.targetHt);//Output value in meters [m], stored in [m]

		//write the target height sigma
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItDIST.target.sigmaTargetHt * LGC::M2MM);//Output value in meters [mm], stored in [m]

		//write ppm error
	    (*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItDIST.target.ppmDist * LGC::M2MM);//Output value in meters [mm], stored in [m]

		//write the sigma target centering
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItDIST.target.sigmaTargetCentering * LGC::M2MM);//Output value in meters [mm], stored in [m]


		//write the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDIST.getScalar());//Output value in meters [m], stored in [m]

		//write the sigma DIST
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItDIST.target.sigmaDist * LGC::M2MM);//Output value in meters [mm], stored in [m]

		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDIST.getScalar() + ItDIST.getScalarResidual());//Output value in meters [m], stored in [m]

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItDIST.getScalarResidual() * LGC::M2MM);//Output value in meters [mm], stored in [m]

		//write the distance cste
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0);
		(*stream)<<ItDIST.target.distCorrectionAdjustable->getProvisionalValue()<<separator;

		//write the distance cste sigma 
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItDIST.target.sigmaDCorr * LGC::M2MM);//Output value in meters [mm], stored in [m]


		if(!ItDIST.target.distCorrectionAdjustable->isFixed()){
			//write the distance cste calculated
			stream->setWidthFormat(obsWidth);
			stream->setPrecisionFormat(lengthPrecision);
			(*stream)<<ItDIST.target.distCorrectionAdjustable->getEstimatedValue()<<separator;

			//write the distance cste sigma )
			stream->setWidthFormat(obsResWidth);
			stream->setPrecisionFormat(lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0);
			(*stream)<<ItDIST.target.distCorrectionAdjustable->getEstimatedPrecision()*LGC::M2MM<<separator;	//MM2M on scalar
		}
		(*stream)<<endl;
	}
	(*stream)<<endl;
}

void TTstnConverter::writeDHORResults(const std::vector<TLINE>& measDHOR)
{   
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	writeDHORResultsHeader(measDHOR.size()); // write the title line for the observations
	for(auto const& ItDHOR : measDHOR)
	{
		(*stream)<<TABs;
		//write TARGET ID
		(*stream).writeStringLeft(nameWidth, ItDHOR.target.ID);
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItDHOR.targetPos->getName());

		//write ppm error
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItDHOR.target.ppmDist * LGC::M2MM);//Output value in meters [mm], stored in [m]

		//write the sigma target centering
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItDHOR.target.sigmaTargetCentering * LGC::M2MM);//Output value in meters [mm], stored in [m]

		//write the observed DHOR
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDHOR.getScalar());//Output value in meters [m], stored in [m]

		//write the sigma DHOR
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItDHOR.target.sigmaDist * LGC::M2MM);//Output value in meters [mm], stored in [m]

		//write the estimated DHOR
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDHOR.getScalar() + ItDHOR.getScalarResidual());//Output value in meters [m], stored in [m]

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, ItDHOR.getScalarResidual() * LGC::M2MM);//Output value in meters [mm], stored in [m]

		(*stream)<<endl;
	}
	(*stream)<<endl;
}
#if 0
void TTstnConverter::writeECTHResults(const TTSTN& fTstn)
{   
	TAStreamFormatter*	stream = getStream();
		int					nameWidth = getNameWidth();
		int					obsWidth = getObsWidth();
		int					obsResWidth = getObsResWidth();
		int					anglePrecision = getAnglePrecision();
		int					angleResPrecicion = getAngleResidualPrecision();
		int					lengthResidualPrecision = getLengthResidualPrecision();
		int					lengthPrecision =	getLengthPrecision();
		string				separator = getSeparator();

	//For each DHOR measurement of the station
	for(auto const& ItRoms : fTstn.roms)
	{
		for(auto const& ItECTH : ItRoms.measECTH)
		{

			//write Point 1
			(*stream).writeStringLeft(nameWidth, fTstn.instrumentPos->getName());

			//write Point 2
			(*stream).writeStringLeft(nameWidth, ItECTH.stationedPoint->getName());

			//write the observed offset
			stream->setLengthUnits(TLength::kMetres);
			stream->setWidthFormat(obsWidth);
			stream->setPrecisionFormat(lengthPrecision);
			(*stream)<<ItECTH.getMeasuredOffsetValue()<<(separator);
			//write the sigma
			stream->setLengthUnits(TLength::kMillimetres);
			stream->setWidthFormat(obsResWidth);
			stream->setPrecisionFormat(lengthResidualPrecision);
			(*stream)<<(separator)<<(separator);

			//write the estimated offset
			stream->setLengthUnits(TLength::kMetres);
			stream->setWidthFormat(obsWidth);
			stream->setPrecisionFormat(lengthPrecision);
			(*stream)<<(ItECTH.getMeasuredOffsetValue()+ItECTH.getMeasuredValueResidual())<<(separator);
			//write the offset (mm) after calculation
			stream->setWidthFormat(obsResWidth);
			stream->setLengthUnits(TLength::kMillimetres);
			stream->setPrecisionFormat(lengthResidualPrecision);
			(*stream)<<ItECTH.getMeasuredValueResidual()*0.001<<(separator);

			//write the offset / sigma (TDouble (MM))
			//stream->setPrecisionFormat(2);
			//(*stream)<<(obsIt->getResDivSig())<<separator;

			//write the sensitivity (TReal (MM/CM))
			//(*stream)<<(obsIt->getSensitivity())<<separator;

			//write the V0 (TAngle (gon))
			stream->setAngleUnits(TAngle::kGons);
			stream->setWidthFormat(obsResWidth);
			(*stream)<<ItRoms.v0<<separator;

			//write the distance cste (TLength (MM))
			stream->setLengthUnits(TLength::kMillimetres);
			stream->setWidthFormat(obsResWidth);
			(*stream)<<ItECTH.scaleInstr.distCorrectionValue*0.001<<separator;

			//write the distance cste sigma (TLength (MM))
			(*stream)<<ItECTH.scaleInstr.sigmaDCorr*0.001<<separator<<endl;
			(*stream).setDataSpacing();
		}
	}
	return;
}

#endif

///////////////////////////////////////////////////////////////////
//////////////HEADERS//////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void TTstnConverter::writePLRResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

		////////////////////////////////////////////////////////////
		//first line
		this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCConverter::kPLR3D), nOObs);
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
		(*stream).writeString(obsWidth,	"(GON)"); //estimated DIST
		(*stream).writeString(obsResWidth,	"(CC)"); //residual

		(*stream).writeString(obsWidth,	"(GON)"); //OBSERVED VALUE
		(*stream).writeString(obsResWidth,"(CC)"); //sigma observed value
		(*stream).writeString(obsWidth,	"(GON)"); //estimated DIST
		(*stream).writeString(obsResWidth,	"(CC)"); //residual

		(*stream)<<endl;
}

void TTstnConverter::writeDIRResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	////////////////////////////////////////////////////////////
	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCConverter::kDIR3D), nOObs);
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

void TTstnConverter::writeANGLResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	////////////////////////////////////////////////////////////
	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCConverter::kANGL), nOObs);
	(*stream)<<endl;
	//Second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "TRGT"); //Name of the target
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
	(*stream).writeString (obsResWidth,	"TCSE"); //sigma Target centering
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured ANGL
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma ANGL
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated ANGL 
	(*stream).writeString(obsResWidth,	"RESIDU"); //residual
	//(*stream).writeString(obsResWidth,	"ECART "); //sensitivity
	//(*stream).writeString(obsResWidth,	"RES/SIG"); //offset/sigma
	//(*stream).writeString(obsWidth,	"DISTANCE"); //instrument's height 
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//Third line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //TARGET ID
	(*stream).writeStringLeft(nameWidth,""); //POSITION
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma target centering
	(*stream).writeString(obsWidth,	"(GON)"); //mesured ANGL
	(*stream).writeString(obsResWidth,	"(CC)"); //sigma ANG
	(*stream).writeString(obsWidth,	"(GON)"); //estimated ANGL
	(*stream).writeString(obsResWidth,	"(CC)"); //residual
	//(*stream).writeString(obsResWidth,	"MM"); //offset/sigma
	//(*stream).writeString(obsResWidth,	""); //sensitivity
	//(*stream).writeString(obsWidth,	"(M)"); //distance
	(*stream)<<endl;
}

void TTstnConverter::writeZENDResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	////////////////////////////////////////////////////////////
	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCConverter::kZEND), nOObs);
	(*stream)<<endl;
	//Second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "TRGT"); //Name of the target
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
	(*stream).writeString(obsWidth,	"HEIGHT"); //provisional target height
	(*stream).writeString(obsResWidth,	"SIGMA H"); //sigma of the target height
	(*stream).writeString (obsResWidth,	"TCSE"); //sigma Target centering
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured ZEND
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma ZEND
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated ZEND 
	(*stream).writeString(obsResWidth,	"RESIDU"); //residual
	//(*stream).writeString(obsResWidth,	"ECART "); //sensitivity
	//(*stream).writeString(obsResWidth,	"RES/SIG"); //offset/sigma
	//(*stream).writeString(obsWidth,	" HAUTEUR D'INSTR"); //instrument's height 
	//(*stream).writeString(obsWidth,	"RALLONGE"); //extension
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//Third line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //TARGET ID
	(*stream).writeStringLeft(nameWidth,""); //POSITION
	(*stream).writeString(obsWidth,	"(M)"); //provisional target height
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma of the target height 
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma target centering
	(*stream).writeString(obsWidth,	"(GON)"); //mesured ZEND
	(*stream).writeString(obsResWidth,	"(CC)"); //sigma ZEND
	(*stream).writeString(obsWidth,	"(GON)"); //estimated ZEND
	(*stream).writeString(obsResWidth,	"(CC)"); //residual
	//(*stream).writeString(obsResWidth,	"MM"); //offset/sigma
	//(*stream).writeString(obsResWidth,	""); //sensitivity
	//(*stream).writeString(obsWidth,	"(M)"); //instrument's height 
	//(*stream).writeString(obsWidth,	"(M)"); //extension
	(*stream)<<endl;
}

void TTstnConverter::writeDISTResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	////////////////////////////////////////////////////////////
	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCConverter::kDIST), nOObs);
	(*stream)<<endl;
	//Second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "TRGT"); //Name of the target used
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
	(*stream).writeString(obsWidth,	"HEIGHT"); //target height
	(*stream).writeString(obsResWidth,	"HSIGMA"); //provisional target height
	(*stream).writeString(obsResWidth, "PPM");  //ppm error value of the measured distance
	(*stream).writeString (obsResWidth,	"TCSE"); //sigma Target centering
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured DIST
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma DIST
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated DIST 
	(*stream).writeString(obsResWidth,	"RESIDU"); //residual

	(*stream).writeString(obsWidth,	"CONST"); //provisional dist corr
	(*stream).writeString(obsResWidth,	"SCONST"); //sigma of provisional dist corr

	(*stream).writeString(obsWidth,	"CONST CALC"); //calculated distance corr
	(*stream).writeString(obsResWidth,	"SCCONST");  //sigma distance corr
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //TARGET ID
	(*stream).writeStringLeft(nameWidth,""); //POSITION
	(*stream).writeString(obsWidth,	"(M)"); //height of the target
	(*stream).writeString(obsResWidth,	"(MM)"); //SIGMA OF THE TARGET
	(*stream).writeString(obsResWidth,	"(MM)"); //PPM
	(*stream).writeString(obsResWidth,	"(MM)"); //SIGMA TGT centering
	(*stream).writeString(obsWidth,	"(M)"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth,"(MM)"); //sigma observed value
	(*stream).writeString(obsWidth,	"(M)"); //estimated DIST
	(*stream).writeString(obsResWidth,	"(MM)"); //residual

	(*stream).writeString(obsWidth,	"(M)"); //provisional dist corr
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma of provisional dist corr
	(*stream).writeString(obsWidth,	"(M)"); //calculated distance corr
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma distance corr
	(*stream)<<endl;
}

void TTstnConverter::writeDHORResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	////////////////////////////////////////////////////////////
	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCConverter::kDHOR), nOObs);
	(*stream)<<endl;
	//Second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "TRGT"); //Name of the target used
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
	(*stream).writeString(obsResWidth, "PPM");  //ppm error value of the measured distance
	(*stream).writeString (obsResWidth,	"TCSE"); //sigma Target centering
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured DIST
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma DIST
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated DIST 
	(*stream).writeString(obsResWidth,	"RESIDU"); //residual
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //TARGET ID
	(*stream).writeStringLeft(nameWidth,""); //POSITION
	(*stream).writeString(obsResWidth,	"(MM)"); //PPM
	(*stream).writeString(obsResWidth,	"(MM)"); //SIGMA TGT centering
	(*stream).writeString(obsWidth,	"(M)"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth,"(MM)"); //sigma observed value
	(*stream).writeString(obsWidth,	"(M)"); //estimated DIST
	(*stream).writeString(obsResWidth,	"(MM)"); //residual
	(*stream)<<endl;
}

#if 0
void TTstnConverter::writeECTHResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"POINT 1"); //first point's Name
	(*stream).writeStringLeft(nameWidth,	"POINT 2"); //second point's Name
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured ECTH
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma ECTH
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated ECTH
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset ECTH
	//(*stream).writeString(obsResWidth,	"RES/SIG"); //offset/sigma
	//(*stream).writeString(obsResWidth,	"SENSI"); //sensitivity
	(*stream).writeString(obsWidth,	"V0"); //estimated V0
	(*stream).writeString(obsResWidth,	"CONST"); //distance's constant 
	(*stream).writeString(obsResWidth,	"SCONST"); //sigma distance's constant 
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream).writeString(nameWidth,	""); //first point's Name
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	"ECTH"); //mesured ECTH
	(*stream).writeString(obsResWidth,	"MM"); //sigma ECTH
	(*stream).writeString(obsWidth,	"ECTH"); //estimated ECTH
	(*stream).writeString(obsResWidth,	"MM"); //offset ECTH
	//(*stream).writeString(obsResWidth,	""); //offset/sigma
	//(*stream).writeString(obsResWidth,	"BILITE"); //sensitivity
	(*stream).writeString(obsWidth,	"GON"); //estimated V0
	(*stream).writeString(obsResWidth,	"(MM)"); //distance's constant 
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma distance's constant 
	(*stream)<<endl;
	

	///////////////////////////////////////////////////////////////////////////////////
	//third line
	(*stream).writeString(nameWidth,	""); //first point's Name
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	"M"); //mesured ECTH
	(*stream).writeString(obsResWidth,	""); //sigma ECTH
	(*stream).writeString(obsWidth,	"M"); //estimated ECTH
	(*stream).writeString(obsResWidth,	""); //offset ECTH
	//(*stream).writeString(obsResWidth,	""); //offset/sigma
	//(*stream).writeString(obsResWidth,	"BILITE"); //sensitivity
	(*stream).writeString(obsWidth,	""); //estimated V0
	(*stream).writeString(obsResWidth,	""); //distance's constant 
	(*stream).writeString(obsResWidth,	""); //sigma distance's constant 
	(*stream)<<endl;
	return;
}
#endif

void TTstnConverter::writeTSTNHeader(const TTSTN& tstn){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	bool instrHeightFixed = tstn.instrumentHeightAdjustable->isFixed();
	////////////////////////////////////////////////////////////
	//first line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"TOTAL STATION " + tstn.instrument.ID);
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"POSITION"); //Point on which is the TSTN positioned
	(*stream).writeString(obsWidth,	"IH INIT"); //HEIGHT OF THE INSTRUMENT initial
	(*stream).writeString(obsWidth,	"IH FIXED"); //HEIGHT OF THE INSTRUMENT initial
	if(!instrHeightFixed)
		(*stream).writeString(obsWidth,	"CALC IH"); //HEIGHT OF THE INSTRUMENT initial

	(*stream).writeString(obsResWidth,	"IHSE"); // Standard deviation of the instrument height
	if(!instrHeightFixed)
		(*stream).writeString(obsResWidth,	"IHSE CAL"); // Standard deviation of the instrument height
	(*stream).writeString(obsResWidth,	"ICSE"); //Sigma of the instrument centering
	(*stream).writeString(obsWidth,	"ROT3D"); // indiacates if station can rotate freely

	if(tstn.rot3D){
		(*stream).writeString(obsWidth,	"rotX"); // rotation around X axis
		(*stream).writeString(obsWidth,	"rotY"); //rotation around Y axis
		(*stream).writeString(obsResWidth,	"SIGMA rX"); //rotation around Y axis
		(*stream).writeString(obsResWidth,	"SIGMA rY"); //rotation around Y axis
	}
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//third line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //Point on which is the TSTN positioned
	(*stream).writeString(obsWidth,	"(M)"); //INITIAL HEIGHT OF THE INSTRUMEN 
	(*stream).writeString(obsWidth,	""); // BOOLEAN VALUE INDICATING IF THE HEIGHT IS FIXED OR NOT
	if(!instrHeightFixed)
		(*stream).writeString(obsWidth,	"(M)"); //calculated value of the instrument height

	(*stream).writeString(obsResWidth,	"(MM)"); //Sigma instrument height

	if(!instrHeightFixed)
		(*stream).writeString(obsResWidth,	"(MM)"); //Sigma instrument height calculated

	(*stream).writeString(obsResWidth,	"(MM)"); //Sigma of the instrument centering
	(*stream).writeString(obsWidth,	""); //BOOL VALUE TELLING IF TSTN CAN ROTATE FREELY
	if(tstn.rot3D){
		(*stream).writeString(obsWidth,	"(GON)"); // rotation around X axis
		(*stream).writeString(obsWidth,	"(GON)"); //rotation around Y axis
		(*stream).writeString(obsResWidth,	"(CC)"); // rotation around X axis
		(*stream).writeString(obsResWidth,	"(CC)"); // rotation around Y axis
	}
	(*stream)<<endl;
}

void TTstnConverter::writeV0Header(){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	std::string         TABs = stream->getCurrSpaceExtended(2);

	//////////////////////////////////////
	//first line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"ROM");
	(*stream).writeString(obsWidth,	"ACST"); // Constant orientation
	(*stream).writeString(obsWidth, "V0 ANGL"); // V0 calculated angle
	(*stream).writeString(obsWidth,	"V0 SIGMA"); // V0 calculated angle
	(*stream)<<endl;
}


void TTstnConverter::writeV0Data(const TTSTN::TROM& rom){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					anglePrecision = getAnglePrecision();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(2);

	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "");

	stream->setAngleUnits(TAngle::kGons);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(anglePrecision);
	(*stream)<<rom.acst.getSLAngle()<<(separator); // Constant orientation (ACST)

	(*stream)<<rom.v0->getEstimatedValue().getSLAngle()<<(separator); // V0 calculated angle
	stream->setAngleUnits(TAngle::k100MicroGons);
	stream->setPrecisionFormat(anglePrecision);
	(*stream)<<rom.v0->getEstimatedPrecision().getSLAngle()<<(separator); // V0 estimated precision

	(*stream)<<endl<<endl;
}

void TTstnConverter::writeTSTNData(const TTSTN& tstn){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResidualPrecision = getAngleResidualPrecision();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					lengthPrecision =	getLengthPrecision();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	(*stream)<<TABs;
	//write NAME OF THE POINT ON WHICH STATION IS POSITIONED
	(*stream).writeStringLeft(nameWidth,tstn.instrumentPos->getName());

	//write INSTRUMENT HEIGHT
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);
	(*stream)<<(tstn.instrumentHeightAdjustable->getProvisionalValue())<<(separator);	//In TScalar stored in metres OK

	if(tstn.instrumentHeightAdjustable->isFixed()){
		(*stream).writeString(obsWidth, "TRUE");
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, tstn.instrument.sigmaInstrHeight * LGC::M2MM); 
	}
	else{
		(*stream).writeString(obsWidth, "FALSE"); //values stored in [m] output is in [mm], lower precision and write out
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, tstn.instrument.sigmaInstrHeight * LGC::M2MM); 
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, tstn.instrumentHeightAdjustable->getEstimatedPrecision().getValue() * LGC::M2MM); 
	}
	//Output sigma instrument centering
	(*stream).writeDouble(obsResWidth, lengthResidualPrecision > 3 ? (lengthResidualPrecision - 3) : 0, tstn.instrument.sigmaInstrCentering * LGC::M2MM); 

	stream->setAngleUnits(TAngle::kGons);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(anglePrecision);
	if(tstn.rot3D){
		(*stream).writeString(obsWidth, "TRUE");
		//write the ROTX
		(*stream)<<(tstn.rotX->getEstimatedValue().getSLAngle())<<(separator);
		//write the ROTY
		(*stream)<<(tstn.rotY->getEstimatedValue().getSLAngle())<<(separator);

		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(angleResidualPrecision);
		(*stream)<<(tstn.rotX->getEstimatedPrecision().getSLAngle())<<(separator);
		(*stream)<<(tstn.rotY->getEstimatedPrecision().getSLAngle())<<(separator);
		(*stream)<<endl;
	}
	else
		(*stream).writeString(obsWidth, "FALSE");
	
	(*stream)<<endl<<endl;
}

#if 0
//RELIABILITY
void TTstnConverter::writeReliabilityData(const TTSTN& fStn)
{

}]
#endif