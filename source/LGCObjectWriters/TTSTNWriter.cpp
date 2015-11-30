#include "TTSTNWriter.h"
#include "TAMeas.h"
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include "Global.h"
#include "TLGCObsSummary.h"
#include "TAdjustablePoint.h"

TTSTNWriter::TTSTNWriter(TAStreamFormatter& stream): TObservationWriter(stream)
{}

TTSTNWriter::~TTSTNWriter(){}


void TTSTNWriter::writeTSTNResults(const TTSTN& tstn){
	TAStreamFormatter*	stream = getStream();
	//Third hierarchy level from local FRAME
	std::string        TABs = stream->getCurrSpaceExtended(3);

	//Write definition of TSTN
	writeTSTNHeader(tstn);
	writeTSTNData(tstn);

	for(auto const ItRoms : tstn.roms)
	{
		const TAngle& V0 = ItRoms.v0->getEstimatedValue();

		//Write definition of ROM
		writeV0Header();
		writeV0Data(ItRoms);

		if(ItRoms.measANGL.size() > 0){
			writeANGLResults(ItRoms.measANGL, tstn.instrumentPos, V0);
			writeAngleResultsSummary(ItRoms.getANGLObsSummary(),TABs);
		}
		if(ItRoms.measZEND.size() > 0){
			writeZENDResults(ItRoms.measZEND, tstn.instrumentPos);
			writeAngleResultsSummary(ItRoms.getZENDObsSummary(),TABs);
		}
		if(ItRoms.measDIST.size() > 0){
			writeDISTResults(ItRoms.measDIST, tstn.instrument, tstn.instrumentPos);
			writeDistanceResultsSummary(ItRoms.getDISTObsSummary(),TABs);
		}
		if(ItRoms.measDHOR.size() > 0){
			writeDHORResults(ItRoms.measDHOR);
			writeDistanceResultsSummary(ItRoms.getDHORObsSummary(),TABs);
		}
		if(ItRoms.measECTH.size() > 0){
			writeECTHResults(ItRoms.measECTH, tstn.instrumentPos);
			writeDistanceResultsSummary(ItRoms.getECTHObsSummary(),TABs);
		}
		if (ItRoms.measECSP.size() > 0){
			writeECSPResults(ItRoms.measECSP, tstn.instrumentPos);
			writeDistanceResultsSummary(ItRoms.getECSPObsSummary(), TABs);
		}
		if(ItRoms.measPLR3D.size() > 0){
			writePLRResults(ItRoms.measPLR3D, tstn.instrument, tstn.instrumentPos, V0);
			TPOLARObsSummary summary = ItRoms.getPLR3DObsSummary();		
			(*stream)<<TABs<<"ANGL"<<endl;
			writeAngleResultsSummary(summary.anglObsSum, TABs);
			(*stream)<<TABs<<"ZEND"<<endl;
			writeAngleResultsSummary(summary.zendObsSum,TABs);
			(*stream)<<TABs<<"DIST"<<endl;
			writeDistanceResultsSummary(summary.distObsSum,TABs);
		}
	}
}

void TTSTNWriter::writeTSTNResultsSIMU(const TTSTN& tstn){
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
			(*stream)<<TABs<<"ANGL"<<endl;
			writeAngleResultsSummary(ItRoms.getANGLObsSummary(),TABs);
		}
		if(ItRoms.measZEND.size() > 0){
			(*stream)<<TABs<<"ZEND"<<endl;
			writeAngleResultsSummary(ItRoms.getZENDObsSummary(),TABs);
		}
		if(ItRoms.measDIST.size() > 0){
			(*stream)<<TABs<<"DIST"<<endl;
			writeDistanceResultsSummary(ItRoms.getDISTObsSummary(),TABs);
		}
		if(ItRoms.measDHOR.size() > 0){
			(*stream)<<TABs<<"DHOR"<<endl;
			writeDistanceResultsSummary(ItRoms.getDHORObsSummary(),TABs);
		}
		if(ItRoms.measECTH.size() > 0){
			(*stream)<<TABs<<"ECTH"<<endl;
			writeDistanceResultsSummary(ItRoms.getECTHObsSummary(),TABs);
		}
		if (ItRoms.measECSP.size() > 0){
			(*stream) << TABs << "ECSP" << endl;
			writeDistanceResultsSummary(ItRoms.getECSPObsSummary(), TABs);
		}

		if(ItRoms.measPLR3D.size() > 0){
			TPOLARObsSummary summary = ItRoms.getPLR3DObsSummary();	
			(*stream)<<TABs<<"DIST"<<endl;
			writeDistanceResultsSummary(summary.distObsSum,TABs);
			(*stream)<<TABs<<"ANGL"<<endl;
			writeAngleResultsSummary(summary.anglObsSum, TABs);
			(*stream)<<TABs<<"ZEND"<<endl;
			writeAngleResultsSummary(summary.zendObsSum,TABs);
		}
	}
}

//RESULTS
void TTSTNWriter::writePLRResults(const std::vector<TPLR3D>& measPLR3D, const TInstrumentData::TPOLAR& instr , const TAdjustablePoint* instrPos, const TAngle& V)
{   
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResPrecision = max(getAngleResidualPrecision() - 4, 0);
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);
	std::string         TABs = stream->getCurrSpaceExtended(3);

   writePLRResultsHeader((int)measPLR3D.size()); // write the title line for the observations
	for(auto const& ItPLR3D: measPLR3D)
	{
		(*stream)<<TABs;
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItPLR3D.targetPos->getName());


//ANGL
		//write the observed ANGL
		(*stream).writeDouble(obsWidth, anglePrecision, ItPLR3D.getAngle(EPLR3DAngles::kANGL).getGonsValue());

		//write the sigma ANGL
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItPLR3D.target.sigmaAngl.getSignedCCValue());

		//write the estimated ANGL
		(*stream).writeDouble(obsWidth, anglePrecision, (ItPLR3D.getAngle(EPLR3DAngles::kANGL) + ItPLR3D.getAngleResidual(EPLR3DAngles::kANGL) + V).getGonsValue());

		//write the residual ANGL
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItPLR3D.getAngleResidual(EPLR3DAngles::kANGL).getSignedCCValue());

		//write the offset		
		TReal dist =sqrt( pow2(ItPLR3D.targetPos->getEstValue(0) - instrPos->getEstValue(0))+
			pow2(ItPLR3D.targetPos->getEstValue(1) - instrPos->getEstValue(1))+
			pow2(ItPLR3D.targetPos->getEstValue(2) - instrPos->getEstValue(2)));
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItPLR3D.getAngleResidual(EPLR3DAngles::kANGL).getRadiansValue()*dist*M2MM);

		//write the residual/sigma ANGL
		(*stream).writeDouble(obsResWidth, 2, (ItPLR3D.getAngleResidual(EPLR3DAngles::kANGL).getRadiansValue() / ItPLR3D.target.sigmaAngl.getRadiansValue()));

//ZEND
		//write the observed ZEND
		(*stream).writeDouble(obsWidth, anglePrecision, ItPLR3D.getAngle(EPLR3DAngles::kZEND).getGonsValue());

		//write the sigma ZEND
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItPLR3D.target.sigmaZenD.getSignedCCValue());

		//write the estimated ZEND
		(*stream).writeDouble(obsWidth, anglePrecision, (ItPLR3D.getAngle(EPLR3DAngles::kZEND) + ItPLR3D.getAngleResidual(EPLR3DAngles::kZEND)).getGonsValue());

		//write the residual ZEND
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItPLR3D.getAngleResidual(EPLR3DAngles::kZEND).getSignedCCValue());

		//write the offset	
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItPLR3D.getAngleResidual(EPLR3DAngles::kZEND).getRadiansValue()*dist*M2MM);

		//write the residual/sigma ZEND
		(*stream).writeDouble(obsResWidth, 2, (ItPLR3D.getAngleResidual(EPLR3DAngles::kZEND).getRadiansValue() / TAngle(ItPLR3D.target.sigmaAngl).getRadiansValue()));

//DIST 
		//write the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision, ItPLR3D.getDistance()); //Write value in meters [m]

		//write the sigma, output the value in [mm],  value is stored in [m], lower the precision by 3 decimal points because of the mm output
		(*stream).writeDouble(obsResWidth, lengthResPrecision, (ItPLR3D.target.sigmaDist + ItPLR3D.target.ppmDist*ItPLR3D.getDistance()/1000)*M2MM); 

		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItPLR3D.getDistance() + ItPLR3D.getDistanceResidual()); 

		//write the residual, output the value in [mm],  value is stored in [m], lower the precision by 3 decimal points because of the mm output
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItPLR3D.getDistanceResidual().getMMetresValue()); 

		//write the sensibility		
		TReal dz= ItPLR3D.targetPos->getEstValue(2) + ItPLR3D.target.targetHt - instrPos->getEstValue(2) - instr.instrHeight;
		if (ItPLR3D.target.distCorrectionUnknown)
		{
			(*stream).writeDouble(obsResWidth, lengthResPrecision, 10 *dz / (ItPLR3D.getDistance() + ItPLR3D.getDistanceResidual() + ItPLR3D.target.distCorrectionAdjustable->getEstimatedValue()));
		}else{
			(*stream).writeDouble(obsResWidth, lengthResPrecision, 10 * dz / (ItPLR3D.getDistance() + ItPLR3D.getDistanceResidual()));
		}

		//write the residual/sigma
		(*stream).writeDouble(obsResWidth, 2, (ItPLR3D.getDistanceResidual()) / (ItPLR3D.target.sigmaDist));

//OTHER INFO
		

		if(!ItPLR3D.target.distCorrectionAdjustable->isFixed()){
			//write the distance cste calculated(TLength (M))
			(*stream).writeDouble(obsWidth, lengthPrecision, ItPLR3D.target.distCorrectionAdjustable->getEstimatedValue());

			//write the distance cste sigma (in (MM))
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItPLR3D.target.distCorrectionAdjustable->getEstimatedPrecision().getMMetresValue());
		}
		else{
			//write the distance cste (TLength (M))
			(*stream).writeDouble(obsWidth, lengthPrecision, ItPLR3D.target.distCorrectionAdjustable->getProvisionalValue());
			(*stream).writeString(obsResWidth, "FIXED");
		}

		//write TARGET ID
		(*stream).writeString(nameWidth, ItPLR3D.target.ID);
		//write the target height
		(*stream).writeDouble(obsWidth,lengthPrecision, ItPLR3D.target.targetHt);	

		(*stream)<<endl;
		}
	(*stream)<<endl;
}

void TTSTNWriter::writeANGLResults(const std::vector<TANGL>& measANGL, const TAdjustablePoint* instrPos, const TAngle& V)
{   
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResPrecision = max(getAngleResidualPrecision() - 4, 0);
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);
	std::string         TABs = stream->getCurrSpaceExtended(3);

	writeANGLResultsHeader((int)measANGL.size()); // write the title line for the observations
	for(auto const& ItANGL : measANGL)
	{
		(*stream)<<TABs;
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItANGL.targetPos->getName());

		//write the observed ANGL
		(*stream).writeDouble(obsWidth, anglePrecision, (ItANGL.getAngle()).getGonsValue());

		//write the sigma ANGL
		(*stream).writeDouble(obsResWidth, angleResPrecision, (ItANGL.target.sigmaAngl).getSignedCCValue());

		//write the estimated ANGL
		(*stream).writeDouble(obsWidth, anglePrecision, (ItANGL.getAngle() + ItANGL.getAngleResidual() + V).getGonsValue());

		//write the residual
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItANGL.getAngleResidual().getSignedCCValue()); 
		
		//write the offset	
		TReal dist =sqrt( pow2(ItANGL.targetPos->getEstValue(0) - instrPos->getEstValue(0))+
			pow2(ItANGL.targetPos->getEstValue(1) - instrPos->getEstValue(1))+
			pow2(ItANGL.targetPos->getEstValue(2) - instrPos->getEstValue(2)));
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItANGL.getAngleResidual().getRadiansValue()*dist*M2MM);
		
		//write the residual/sigma
		(*stream).writeDouble(obsResWidth, 2, (ItANGL.getAngleResidual().getRadiansValue() / ItANGL.target.sigmaAngl.getRadiansValue())); 
		
		//write TARGET ID
		(*stream).writeString(nameWidth, ItANGL.target.ID);
		
		//write the target height
		(*stream).writeDouble(obsWidth, lengthPrecision, ItANGL.target.targetHt);
		(*stream)<<endl;
	}
	(*stream)<<endl;
}

void TTSTNWriter::writeZENDResults(const std::vector<TZEND>& measZEND, const TAdjustablePoint* instrPos)
{   
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResPrecision = max(getAngleResidualPrecision() - 4, 0);
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);
	std::string         TABs = stream->getCurrSpaceExtended(3);

	writeZENDResultsHeader((int)measZEND.size()); // write the title line for the observations
	for(auto const& ItZEND : measZEND)
	{
		(*stream)<<TABs;
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItZEND.targetPos->getName());

		//write the observed ZEND
		(*stream).writeDouble(obsWidth, anglePrecision, ItZEND.getAngle().getGonsValue());

		//write the sigma ZEND
		(*stream).writeDouble(obsResWidth, angleResPrecision, (ItZEND.target.sigmaZenD).getSignedCCValue());

		//write the estimated ZEND
		(*stream).writeDouble(obsWidth, anglePrecision, (ItZEND.getAngle() + ItZEND.getAngleResidual()).getGonsValue());

		//write the residual
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItZEND.getAngleResidual().getSignedCCValue());

		//write the offset	
		TReal dist =sqrt( pow2(ItZEND.targetPos->getEstValue(0) - instrPos->getEstValue(0))+
			pow2(ItZEND.targetPos->getEstValue(1) - instrPos->getEstValue(1))+
			pow2(ItZEND.targetPos->getEstValue(2) - instrPos->getEstValue(2)));
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItZEND.getAngleResidual().getRadiansValue()*dist*M2MM);

		//write the residual/sigma
		(*stream).writeDouble(obsResWidth, angleResPrecision, (ItZEND.getAngleResidual().getRadiansValue() / ItZEND.target.sigmaZenD.getRadiansValue()));
		
		//write TARGET ID
		(*stream).writeString(nameWidth, ItZEND.target.ID);
		
		//write the target height
		(*stream).writeDouble(obsWidth, lengthPrecision, ItZEND.target.targetHt);
		(*stream)<<endl;
	}
	(*stream)<<endl<<endl;
}

void TTSTNWriter::writeDISTResults(const std::vector<TLINE>& measDIST,const TInstrumentData::TPOLAR& instr , const TAdjustablePoint* instrPos)
{   

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);
	int					lengthPrecision =	getLengthPrecision();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	writeDISTResultsHeader((int)measDIST.size()); // write the title line for the observations
	for(auto const& ItDIST : measDIST)
	{
		(*stream)<<TABs;
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItDIST.targetPos->getName());

		//write the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDIST.getDistance());

		//write the sigma DIST
		(*stream).writeDouble(obsResWidth, lengthResPrecision, (ItDIST.target.sigmaDist +ItDIST.target.ppmDist*ItDIST.getDistance()/1000)* M2MM);

		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDIST.getDistance() + ItDIST.getDistanceResidual());

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDIST.getDistanceResidual().getMMetresValue());
		
		//write the sensibility	
		TReal dz= ItDIST.targetPos->getEstValue(2) + ItDIST.target.targetHt - instrPos->getEstValue(2) - instr.instrHeight;
		if (ItDIST.target.distCorrectionUnknown)
		{
			(*stream).writeDouble(obsResWidth, lengthResPrecision, 10 *dz / (ItDIST.getDistance() + ItDIST.getDistanceResidual() + ItDIST.target.distCorrectionAdjustable->getEstimatedValue()));
		}else{
			(*stream).writeDouble(obsResWidth, lengthResPrecision, 10 * dz / (ItDIST.getDistance() + ItDIST.getDistanceResidual()));
		}

		//write the residual/sigma
		(*stream).writeDouble(obsResWidth, 2, (ItDIST.getDistanceResidual() / ItDIST.target.sigmaDist));

		if(!ItDIST.target.distCorrectionAdjustable->isFixed()){
			//write the distance cste calculated
			(*stream).writeDouble(obsWidth, lengthPrecision, ItDIST.target.distCorrectionAdjustable->getEstimatedValue());

			//write the distance cste sigma )
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDIST.target.distCorrectionAdjustable->getEstimatedPrecision().getMMetresValue());
		} else {
			//write the distance cste
			(*stream).writeDouble(obsWidth, lengthPrecision, ItDIST.target.distCorrectionAdjustable->getProvisionalValue());

			//write the distance cste sigma 
			(*stream).writeString(obsResWidth, "FIXED");	
		}
		
		//write TARGET ID
		(*stream).writeString(nameWidth, ItDIST.target.ID);
		//write the target height
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDIST.target.targetHt);

		(*stream)<<endl;
	}
	(*stream)<<endl;
}

void TTSTNWriter::writeDHORResults(const std::vector<TLINE>& measDHOR)
{   
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);
	int					lengthPrecision =	getLengthPrecision();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	writeDHORResultsHeader((int)measDHOR.size()); // write the title line for the observations
	for(auto const& ItDHOR : measDHOR)
	{
		(*stream)<<TABs;

		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItDHOR.targetPos->getName());

		//write the observed DHOR
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDHOR.getDistance());

		//write the sigma DHOR
		(*stream).writeDouble(obsResWidth, lengthResPrecision, (ItDHOR.target.sigmaDist + ItDHOR.target.ppmDist*ItDHOR.getDistance()/1000)*M2MM);//Output value in meters [mm], stored in [m]

		//write the estimated DHOR
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDHOR.getDistance() + ItDHOR.getDistanceResidual());

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDHOR.getDistanceResidual().getMMetresValue());

		//write the residual/SIGMA
		(*stream).writeDouble(obsResWidth, 2, ItDHOR.getDistanceResidual() /ItDHOR.target.sigmaDist );

		//write TARGET ID
		(*stream).writeString(nameWidth, ItDHOR.target.ID);
		//write the target height
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDHOR.target.targetHt);

		(*stream)<<endl;
	}
	(*stream)<<endl;
}

void TTSTNWriter::writeECTHResults(const std::vector<TECTH>& measECTH, const TAdjustablePoint* instrPos)
{   
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);
	int					lengthPrecision =	getLengthPrecision();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	writeECTHResultsHeader((int)measECTH.size());

	//For each DHOR measurement of the station
	for(auto const& ItECTH : measECTH)
	{
		(*stream) << TABs;

		//write Point
		(*stream).writeStringLeft(nameWidth, ItECTH.targetPos->getName());

		//write the observed offset
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECTH.getDistance());
		
		//write the sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECTH.target.sigmaD.getMMetresValue());

		//write the estimated offset
		(*stream).writeDouble(obsWidth, lengthPrecision, (ItECTH.getDistance() + ItECTH.getDistanceResidual()));
		
		//write the offset (mm) after calculation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECTH.getDistanceResidual().getMMetresValue());

		//write the offset / sigma (TDouble (MM))
		(*stream).writeDouble(obsResWidth, 2, (ItECTH.getDistanceResidual() / ItECTH.target.sigmaD));

		//write azimut
		(*stream).writeDouble(obsWidth, anglePrecision, ItECTH.obsHorAngle.getGonsValue());


		(*stream) << endl;
	}
	(*stream) << endl;

}

void TTSTNWriter::writeECSPResults(const std::vector<TECSP>& measECSP, const TAdjustablePoint* instrPos)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);
	int					lengthPrecision = getLengthPrecision();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	writeECSPResultsHeader((int)measECSP.size());

	//For each DHOR measurement of the station
	for (auto const& ItECSP : measECSP)
	{
		(*stream) << TABs;

		//write Point
		(*stream).writeStringLeft(nameWidth, ItECSP.targetPos->getName());

		//write the observed offset
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECSP.getDistance());

		//write the sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECSP.target.sigmaD.getMMetresValue());

		//write the estimated offset
		(*stream).writeDouble(obsWidth, lengthPrecision, (ItECSP.getDistance() + ItECSP.getDistanceResidual()));

		//write the offset (mm) after calculation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECSP.getDistanceResidual().getMMetresValue());

		//write the offset / sigma (TDouble (MM))
		(*stream).writeDouble(obsResWidth, 2, (ItECSP.getDistanceResidual() / ItECSP.target.sigmaD));

		//write angle
		(*stream).writeDouble(obsWidth, anglePrecision, ItECSP.obsHorAngle.getGonsValue());
		(*stream).writeDouble(obsWidth, anglePrecision, ItECSP.obsVertAngle.getGonsValue());

		(*stream) << endl;
	}
	(*stream) << endl;

}


///////////////////////////////////////////////////////////////////
//////////////HEADERS//////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void TTSTNWriter::writePLRResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

		////////////////////////////////////////////////////////////
		//first line
		this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kPLR3D), nOObs);
		(*stream)<<endl;
		//second line
		(*stream)<<TABs;
		(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
		
		(*stream).writeString(obsWidth,	"OBSANGL"); //mesured ANGL
		(*stream).writeString(obsResWidth,	"SANGL"); //sigma ANGL
		(*stream).writeString(obsWidth,	"CALCANGL"); //estimated ANGL 
		(*stream).writeString(obsResWidth,	"RESANGL"); //residual
		(*stream).writeString(obsResWidth,	"ECART"); //ecart
		(*stream).writeString(obsResWidth,	"RES"); //residual/sigma


		(*stream).writeString(obsWidth,	"OBSZEND"); //mesured ZEND
		(*stream).writeString(obsResWidth,	"SZEND"); //sigma ZEND
		(*stream).writeString(obsWidth,	"CALCZEND"); //estimated ZEND 
		(*stream).writeString(obsResWidth,	"RESZEND"); //residual ZZEND
		(*stream).writeString(obsResWidth,	"ECART"); //ecart
		(*stream).writeString(obsResWidth,	"RES"); //residual/sigma

		(*stream).writeString(obsWidth,	"OBSDIST"); //mesured DIST
		(*stream).writeString(obsResWidth,	"SDIST"); //sigma DIST
		(*stream).writeString(obsWidth,	"CALCDIST"); //estimated DIST 
		(*stream).writeString(obsResWidth,	"RESDIST"); //residual
		(*stream).writeString(obsResWidth,	"SENSI"); //sensibilite
		(*stream).writeString(obsResWidth,	"RES"); //residual/sigma
	
		(*stream).writeString(obsWidth,	"CONST"); //provisional dist corr
		(*stream).writeString(obsResWidth,	"SCONST");  //sigma distance corr
		(*stream).writeString(nameWidth, "TRGT"); //Name of the target used
		(*stream).writeString(obsWidth,	"H_TRGT"); //target height

		(*stream)<<endl;

		(*stream)<<TABs;
		(*stream).writeStringLeft(nameWidth,""); //POSITION

		(*stream).writeString(obsWidth,	"(GON)"); //OBSERVED VALUE
		(*stream).writeString(obsResWidth,"(CC)"); //sigma observed value
		(*stream).writeString(obsWidth,	"(GON)"); //estimated DIST
		(*stream).writeString(obsResWidth,	"(CC)"); //residual
		(*stream).writeString(obsResWidth,	"(MM)"); //ecart
		(*stream).writeString(obsResWidth,	"/SIG"); //residual/sigma

		(*stream).writeString(obsWidth,	"(GON)"); //OBSERVED VALUE
		(*stream).writeString(obsResWidth,"(CC)"); //sigma observed value
		(*stream).writeString(obsWidth,	"(GON)"); //estimated DIST
		(*stream).writeString(obsResWidth,	"(CC)"); //residual
		(*stream).writeString(obsResWidth,	"(MM)"); //ecart
		(*stream).writeString(obsResWidth,	"/SIG"); //residual/sigma

		(*stream).writeString(obsWidth,	"(M)"); //OBSERVED VALUE
		(*stream).writeString(obsResWidth,"(MM)"); //sigma observed value
		(*stream).writeString(obsWidth,	"(M)"); //estimated DIST
		(*stream).writeString(obsResWidth,	"(MM)"); //residual
		(*stream).writeString(obsResWidth,	"(MM/CM)"); //sensibilite
		(*stream).writeString(obsResWidth,	"/SIG"); //residual/sigma

		(*stream).writeString(obsWidth,	"(M)"); //distance corr
		(*stream).writeString(obsResWidth,	"(MM)"); //sigma distance corr
		(*stream).writeString(nameWidth,""); //TARGET ID
		(*stream).writeString(obsWidth,	"(M)"); //height of the target
		(*stream)<<endl;
}

void TTSTNWriter::writeANGLResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	////////////////////////////////////////////////////////////
	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kANGL), nOObs);
	(*stream)<<endl;
	//Second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured ANGL
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma ANGL
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated ANGL 
	(*stream).writeString(obsResWidth,	"RESIDU"); //residual
	(*stream).writeString(obsResWidth,	"ECART"); //ecart
	(*stream).writeString(obsResWidth,	"RES"); //residual/sigma
	(*stream).writeString(nameWidth, "TRGT"); //Name of the target
	(*stream).writeString(obsWidth,	"H_TGT"); //provisional target height
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//Third line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //POSITION
	(*stream).writeString(obsWidth,	"(GON)"); //mesured ANGL
	(*stream).writeString(obsResWidth,	"(CC)"); //sigma ANG
	(*stream).writeString(obsWidth,	"(GON)"); //estimated ANGL
	(*stream).writeString(obsResWidth,	"(CC)"); //residual
	(*stream).writeString(obsResWidth,	"(MM)"); //ecart
	(*stream).writeString(obsResWidth,	"/SIG"); //residual/sigma
	(*stream).writeString(nameWidth,""); //TARGET ID
	(*stream).writeString(obsWidth,	"(M)"); //provisional target height
	(*stream)<<endl;
}

void TTSTNWriter::writeZENDResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	////////////////////////////////////////////////////////////
	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kZEND), nOObs);
	(*stream)<<endl;
	//Second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured ZEND
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma ZEND
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated ZEND 
	(*stream).writeString(obsResWidth,	"RESIDU"); //residual
	(*stream).writeString(obsResWidth,	"ECART"); //ecart
	(*stream).writeString(obsResWidth,	"RES"); //residual/sigma
	(*stream).writeString(nameWidth, "TRGT"); //Name of the target
	(*stream).writeString(obsWidth,	"H_TGT"); //provisional target height
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//Third line
	(*stream)<<TABs;
	
	(*stream).writeStringLeft(nameWidth,""); //POSITION
	(*stream).writeString(obsWidth,	"(GON)"); //mesured ZEND
	(*stream).writeString(obsResWidth,	"(CC)"); //sigma ZEND
	(*stream).writeString(obsWidth,	"(GON)"); //estimated ZEND
	(*stream).writeString(obsResWidth,	"(CC)"); //residual
	(*stream).writeString(obsResWidth,	"(MM)"); //ecart
	(*stream).writeString(obsResWidth,	"/SIG"); //residual/sigma
	(*stream).writeString(nameWidth,""); //TARGET ID
	(*stream).writeString(obsWidth,	"(M)"); //provisional target height
	(*stream)<<endl;
}

void TTSTNWriter::writeDISTResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	////////////////////////////////////////////////////////////
	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kDIST), nOObs);
	(*stream)<<endl;
	//Second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured DIST
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma DIST
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated DIST 
	(*stream).writeString(obsResWidth,	"RESIDU"); //residual
	(*stream).writeString(obsResWidth,	"SENSI"); //sensibilite
	(*stream).writeString(obsResWidth,	"RES"); //residual/sigma
	(*stream).writeString(obsWidth,	"CONST"); //dist corr
	(*stream).writeString(obsResWidth,	"SCONST"); //sigma of provisional dist corr
	(*stream).writeString(nameWidth, "TRGT"); //Name of the target
	(*stream).writeString(obsWidth,	"H_TGT"); //provisional target height
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //POSITION
	(*stream).writeString(obsWidth,	"(M)"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth,"(MM)"); //sigma observed value
	(*stream).writeString(obsWidth,	"(M)"); //estimated DIST
	(*stream).writeString(obsResWidth,	"(MM)"); //residual
	(*stream).writeString(obsResWidth,	"(MM/CM)"); //ecart
	(*stream).writeString(obsResWidth,	"/SIG"); //residual/sigma
	(*stream).writeString(obsWidth,	"(M)"); //provisional dist corr
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma of provisional dist corr
	(*stream).writeString(nameWidth,""); //TARGET ID
	(*stream).writeString(obsWidth,	"(M)"); //provisional target height
	(*stream)<<endl;
}

void TTSTNWriter::writeDHORResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	////////////////////////////////////////////////////////////
	//first line
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kDHOR), nOObs);
	(*stream)<<endl;
	//Second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the target
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured DHOR
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma DHOR
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated DHOR 
	(*stream).writeString(obsResWidth,	"RESIDU"); //residual
	//(*stream).writeString(obsResWidth,	"ECART"); //ecart
	(*stream).writeString(obsResWidth,	"RES/SIG"); //residual/sigma
	(*stream).writeString(nameWidth, "TRGT"); //Name of the target
	(*stream).writeString(obsWidth,	"H_TGT"); //provisional target height
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //POSITION
	(*stream).writeString(obsWidth,	"(M)"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth,"(MM)"); //sigma observed value
	(*stream).writeString(obsWidth,	"(M)"); //estimated DIST
	(*stream).writeString(obsResWidth,	"(MM)"); //residual
	//(*stream).writeString(obsResWidth,	"ECART"); //ecart
	(*stream).writeString(obsResWidth,	""); //residual/sigma
	(*stream).writeStringLeft(nameWidth,""); //TARGET ID
	(*stream).writeString(obsWidth,	"(M)"); //target height
	(*stream)<<endl;
}

void TTSTNWriter::writeECTHResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	//summuray
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kECTH), nOObs);
	(*stream)<<endl;

	
	////////////////////////////////////////////////////////////
	//first line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POINT"); //second point's Name
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured ECTH
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma ECTH
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated ECTH
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset ECTH
	(*stream).writeString(obsResWidth,	"RES/SIG"); //offset/sigma
	(*stream).writeString(obsWidth, "AZIMUT"); //azimut
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); //station
	(*stream).writeString(obsWidth,	"(M)"); //mesured ECTH
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma ECTH
	(*stream).writeString(obsWidth,	"(M)"); //estimated ECTH
	(*stream).writeString(obsResWidth,	"(MM)"); //offset ECTH
	(*stream).writeString(obsResWidth,	""); //offset/sigma
	(*stream).writeString(obsWidth, "(GON)"); //azimut
	(*stream)<<endl;

}

void TTSTNWriter::writeECSPResultsHeader(int nOObs)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(3);

	//summuray
	this->writeObsTitle(TABs + this->getObsDescriptionEN(TALGCObjectWriter::kECSP), nOObs);
	(*stream) << endl;


	////////////////////////////////////////////////////////////
	//first line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POINT"); //second point's Name
	(*stream).writeString(obsWidth, "OBSERVE"); //mesured ECTH
	(*stream).writeString(obsResWidth, "SIGMA"); //sigma ECTH
	(*stream).writeString(obsWidth, "CALCULE"); //estimated ECTH
	(*stream).writeString(obsResWidth, "RESIDU"); //offset ECTH
	(*stream).writeString(obsResWidth, "RES/SIG"); //offset/sigma
	(*stream).writeString(obsWidth, "AZIMUT"); //azimut
	(*stream).writeString(obsWidth, "ZEND"); //zenithal distance
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); //station
	(*stream).writeString(obsWidth, "(M)"); //mesured ECTH
	(*stream).writeString(obsResWidth, "(MM)"); //sigma ECTH
	(*stream).writeString(obsWidth, "(M)"); //estimated ECTH
	(*stream).writeString(obsResWidth, "(MM)"); //offset ECTH
	(*stream).writeString(obsResWidth, ""); //offset/sigma
	(*stream).writeString(obsWidth, "(GON)"); //azimut
	(*stream).writeString(obsWidth, "(GON)"); //zenithal distance
	(*stream) << endl;

}

void TTSTNWriter::writeTSTNHeader(const TTSTN& tstn){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	////////////////////////////////////////////////////////////
	//first line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"TOTAL STATION INSTRUMENT: " + tstn.instrument.ID);
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"POSITION"); //Point on which is the TSTN positioned
	(*stream).writeString(obsWidth,	"H_INSTR"); //HEIGHT OF THE INSTRUMENT initial
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
	(*stream).writeString(obsWidth,	""); //BOOL VALUE TELLING IF TSTN CAN ROTATE FREELY
	
	if(tstn.rot3D){
		(*stream).writeString(obsWidth,	"(GON)"); // rotation around X axis
		(*stream).writeString(obsWidth,	"(GON)"); //rotation around Y axis
		(*stream).writeString(obsResWidth,	"(CC)"); // rotation around X axis
		(*stream).writeString(obsResWidth,	"(CC)"); // rotation around Y axis
	}
	(*stream)<<endl;
}

void TTSTNWriter::writeV0Header(){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	std::string         TABs = stream->getCurrSpaceExtended(2);

	//////////////////////////////////////
	//first line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"ROM");
	(*stream).writeString(obsWidth,	"ACST"); // Constant orientation
	(*stream).writeString(obsWidth, "V0"); // V0 calculated angle
	(*stream).writeString(obsWidth,	"SV0"); // V0 calculated angle
	(*stream)<<endl;
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"");
	(*stream).writeString(obsWidth,	"GON"); // Constant orientation
	(*stream).writeString(obsWidth, "GON"); // V0 calculated angle
	(*stream).writeString(obsWidth,	"CC"); // V0 calculated angle
	(*stream)<<endl;
}


void TTSTNWriter::writeV0Data(const TTSTN::TROM& rom){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					anglePrecision = getAnglePrecision();
	std::string         TABs = stream->getCurrSpaceExtended(2);

	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "");

	(*stream).writeDouble(obsWidth, anglePrecision, rom.acst.getGonsValue()); // Constant orientation (ACST)
	(*stream).writeDouble(obsWidth, anglePrecision, rom.v0->getEstimatedValue().getGonsValue()); // V0 calculated angle
	(*stream).writeDouble(obsWidth, anglePrecision, rom.v0->getEstimatedPrecision().getSignedCCValue()); // V0 estimated precision

	(*stream)<<endl<<endl;
}

void TTSTNWriter::writeTSTNData(const TTSTN& tstn){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResPrecision = max(getAngleResidualPrecision()-4, 0);
	int					lengthPrecision = max(getLengthPrecision()-3, 0);
	std::string         TABs = stream->getCurrSpaceExtended(3);

	(*stream)<<TABs;
	//write NAME OF THE POINT ON WHICH STATION IS POSITIONED
	(*stream).writeStringLeft(nameWidth,tstn.instrumentPos->getName());

	//write INSTRUMENT HEIGHT
	if(tstn.instrumentHeightAdjustable->isFixed()){
		(*stream).writeDouble(obsWidth, lengthPrecision, tstn.instrument.instrHeight); 
	}
	else{
		(*stream).writeDouble(obsResWidth, lengthPrecision, tstn.instrumentHeightAdjustable->getEstimatedValue()); 
	}


	if(tstn.rot3D){
		(*stream).writeString(obsWidth, "TRUE");
		//write the ROTX
		(*stream).writeDouble(obsWidth, anglePrecision, tstn.rotX->getEstimatedValue().getGonsValue());
		//write the ROTY
		(*stream).writeDouble(obsWidth, anglePrecision, tstn.rotY->getEstimatedValue().getGonsValue());


		(*stream).writeDouble(obsResWidth, angleResPrecision, tstn.rotX->getEstimatedPrecision().getSignedCCValue());
		(*stream).writeDouble(obsResWidth, angleResPrecision, tstn.rotY->getEstimatedPrecision().getSignedCCValue());
		(*stream)<<endl;
	}
	else
		(*stream).writeString(obsWidth, "FALSE");
	
	(*stream)<<endl<<endl;
}


//RELIABILITY
void	TTSTNWriter::writeANGLReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("STATION","TARGET", "", "OBSERVATION", "GONS", "CC");
	return;
}

void	TTSTNWriter::writeZENDReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("STATION","TARGET", "", "OBSERVATION", "GONS", "CC");
	return;
}

void	TTSTNWriter::writeDISTReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("STATION","TARGET", "", "OBSERVATION", "M", "MM");
	return;
}

void	TTSTNWriter::writePLRReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("STATION","TARGET", "", "OBSERVATION", "GONS", "CC");
	this->TObservationWriter::writeReliabilityHeader("","", "", "OBSERVATION", "GONS", "CC");
	this->TObservationWriter::writeReliabilityHeader("","", "", "OBSERVATION", "M", "MM");
	return;
}

void	TTSTNWriter::writeDHORReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("STATION","TARGET", "", "OBSERVATION", "M", "MM");
	return;
}

void	TTSTNWriter::writeECTHReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("STATION","TARGET", "", "OBSERVATION", "M", "MM");
	return;
}

void	TTSTNWriter::writeECSPReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("STATION", "TARGET", "", "OBSERVATION", "M", "MM");
	return;
}

void	TTSTNWriter::writeANGLReliabilityData(const TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TANGL>& measANGL)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResPrecision = max(getAngleResidualPrecision() - 4, 0);

	for(auto const& ItANGL : measANGL)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItANGL.getFirstObservationIndex();
		
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, tstn.instrumentPos->getName());
		
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItANGL.targetPos->getName());
		
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//write the observed ANGL
		(*stream).writeDouble(obsWidth, anglePrecision, ItANGL.getAngle().getGonsValue());

		//write the sigma ANGL
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItANGL.target.sigmaAngl.getSignedCCValue());

		//write the residual
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItANGL.getAngleResidual().getSignedCCValue());

		writeReliability(index, stat);
		(*stream).setDataSpacing();
	}
	return;
}

void	TTSTNWriter::writeZENDReliabilityData(const  TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TZEND>& measZEND)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResPrecision = max(getAngleResidualPrecision() - 4, 0);

	for(auto const& ItZEND : measZEND)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItZEND.getFirstObservationIndex();
		
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, tstn.instrumentPos->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItZEND.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//write the observed ZEND
		(*stream).writeDouble(obsWidth, anglePrecision, ItZEND.getAngle().getGonsValue());

		//write the sigma ZEND
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItZEND.target.sigmaAngl.getSignedCCValue());

		//write the residual
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItZEND.getAngleResidual().getSignedCCValue());

		writeReliability(index, stat);
		(*stream).setDataSpacing();
	}
	return;
}

void	TTSTNWriter::writeDISTReliabilityData(const  TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TLINE>& measDIST)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);

	for(auto const& ItDist : measDIST)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItDist.getFirstObservationIndex();
		
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, tstn.instrumentPos->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItDist.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision,ItDist.getDistance());
		//get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDist.target.sigmaDist.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDist.getDistanceResidual().getMMetresValue());

		writeReliability(index, stat);
		(*stream).setDataSpacing();
	}
	return;	
}

void	TTSTNWriter::writePLRReliabilityData(const TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TPLR3D>& measPLR3D)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);
	int					anglePrecision = getAnglePrecision();
	int					angleResPrecision = max(getAngleResidualPrecision() - 4, 0);

	for(auto const& ItPLR : measPLR3D)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItPLR.getFirstObservationIndex();

//----------------------- ANGL ------------------------------------//
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, tstn.instrumentPos->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItPLR.target.ID);
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//write the observed ANGL
		(*stream).writeDouble(obsWidth, anglePrecision, ItPLR.getAngle(EPLR3DAngles::kANGL).getGonsValue());
		//write the sigma ANGL
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItPLR.target.sigmaAngl.getSignedCCValue());
		//write the residual
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItPLR.getAngleResidual(EPLR3DAngles::kANGL).getSignedCCValue());

		writeReliability(index, stat);
//----------------------- ZEND ------------------------------------//
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, "");
		//get Tg point
		(*stream).writeStringLeft(nameWidth, "");
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//write the observed ZEND
		(*stream).writeDouble(obsWidth, anglePrecision, ItPLR.getAngle(EPLR3DAngles::kZEND).getGonsValue());
		//write the sigma ZEND
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItPLR.target.sigmaZenD.getSignedCCValue());
		//write the residual
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItPLR.getAngleResidual(EPLR3DAngles::kZEND).getSignedCCValue());

		writeReliability(index+1, stat);
//----------------------- DIST ------------------------------------//
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, "");
		//get Tg point
		(*stream).writeStringLeft(nameWidth, "");
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed 3rd equation
		(*stream).writeDouble(obsWidth, lengthPrecision,ItPLR.getDistance(ESingleValue::kValue));
		//get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItPLR.target.sigmaDist.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItPLR.getDistanceResidual(ESingleValue::kValue).getMMetresValue());

		writeReliability(index+2, stat);
		(*stream).setDataSpacing();
	}
	return;	
}

void	TTSTNWriter::writeDHORReliabilityData(const  TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TLINE>& measDHOR)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);

	for(auto const& ItDhor : measDHOR)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItDhor.getFirstObservationIndex();
		
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, tstn.instrumentPos->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItDhor.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed DHOR
		(*stream).writeDouble(obsWidth, lengthPrecision,ItDhor.getDistance());
		//get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDhor.target.sigmaDist.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItDhor.getDistanceResidual().getMMetresValue());

		writeReliability(index, stat);
		(*stream).setDataSpacing();
	}
	return;	
}

void	TTSTNWriter::writeECTHReliabilityData(const TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TECTH>& measECTH)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision()-3, 0);

	for(auto const& ItEcth : measECTH)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItEcth.getFirstObservationIndex();
		
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, tstn.instrumentPos->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItEcth.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision,ItEcth.getDistance());
		//get the sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItEcth.target.sigmaD.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItEcth.getDistanceResidual().getMMetresValue());

		writeReliability(index, stat);
		(*stream).setDataSpacing();
	}
	return;
}

void	TTSTNWriter::writeECSPReliabilityData(const TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TECSP>& measECSP)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = max(getLengthResidualPrecision() - 3, 0);

	for (auto const& ItEcsp : measECSP)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItEcsp.getFirstObservationIndex();

		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, tstn.instrumentPos->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItEcsp.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItEcsp.getDistance());
		//get the sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItEcsp.target.sigmaD.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItEcsp.getDistanceResidual().getMMetresValue());

		writeReliability(index, stat);
		(*stream).setDataSpacing();
	}
	return;
}

