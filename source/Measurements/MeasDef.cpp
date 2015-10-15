#include "MeasDef.h"

///////////////////////////////////////////////////////////
//////Total station measurements
///////////////////////////////////////////////////////////
TPLR3D::TPLR3D(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAScalarMeas(pos, tgt),
			fFirstObservationIndex(NO_VALi){}


TDIR3D::TDIR3D(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAScalarMeas(pos, tgt),
			fFirstObservationIndex(NO_VALi){}


TLINE::TLINE(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt): 
				TAScalarMeas(pos, tgt) {}

TLINE::TLINE(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt, TReal v) : 
				TAScalarMeas(pos, tgt, v) {}

TDHOR::TDHOR(const TAdjustablePoint& pos, TInstrumentData::TLEVEL::TTarget tgt, TReal v) :
				TAScalarMeas(pos, tgt, v), dhorSigma(NO_VALf) {}

///////////////////////////////////////////////////////////
//////Offset measurements
///////////////////////////////////////////////////////////
TECTH::TECTH(const TAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr,  LGC::TAngle obsHorAngle, TReal v) :   
				stationedPoint(&stationedPoint), 
				scaleInstr(scaleInstr),
				obsHorAngle(obsHorAngle),
				measuredOffset(v),
				line(NO_VALi),
				residualOffset(NO_VALf),
				fFirstEquationIndex(NO_VALi)
				{}

///////////////////////////////////////////////////////////
//////Levelling measurements
///////////////////////////////////////////////////////////
TDVER::TDVER(const TAdjustablePoint& station, const TAdjustablePoint& target, TReal obsVal) : 
	station(&station),
	target(&target),
	fObservedValue(obsVal),
	fSigmaObsVal(0.0001), /*0.1 mm*/
	fDistanceCorrection(0.0),
	fFirstEquationIndex(NO_VALi),
	line(NO_VALi)
	{}

TDLEV::TDLEV(const TAdjustablePoint& targetPos, TInstrumentData::TLEVEL::TTarget target, TReal measValue) :
			TAScalarMeas(targetPos, target, measValue)
			{}

///////////////////////////////////////////////////////////
//////Frame measurement, COMMENTED
///////////////////////////////////////////////////////////
#if 0
TFrameMeas::TFrameMeas(const TAdjustableHelmertTransformation& observedFrame):
		fFrameMeas(&observedFrame),
		line(NO_VALi),
		fFirstEquationIndex(NO_VALi)
		{}

/// Sets the standard deviation of a rotation around X(0), Y(1) or Z(2) axis
void TFrameMeas::setRotationSigma(int axis, TReal stDev){
	assert3D(axis); 
	if(!fFrameMeas->fixedRotations[axis]){
		fSigmaRotations[axis] = stDev;
		fSigmaRotationsState[axis] = 1;
	}
	else
		throw std::logic_error("Trying to assign standard deviation to a fixed rotation element.");
}

/// Sets the standard deviation of a translation around X(0), Y(1) or Z(2) axis
void TFrameMeas::setTranslationSigma(int axis, TReal stDev){
	assert3D(axis); 
	if(!fFrameMeas->fixedTranslations[axis]){
		fSigmaTranslations[axis] = stDev;
		fSigmaTranslationsState[axis] = 1;
	}
	else
		throw std::logic_error("Trying to assign standard deviation to a fixed translation element.");
}

/// Sets the standard deviation of a scale factor
void TFrameMeas::setScaleSigma(TReal stDev){
	if(!fFrameMeas->fixedScale[0]){
		fScaleSigma = stDev;
		fSigmaRotationsState[0] = 1;
	}
	else
		throw std::logic_error("Trying to assign standard deviation to a fixed scale.");
}
///////////////////////////////////////////////////////////
//////Point measurement
///////////////////////////////////////////////////////////
TPointMeas::TPointMeas(const TAdjustablePoint& measuredPoint, TReal stDevX, TReal stDevY, TReal stDevZ):
fPointMeas(&measuredPoint),
fFirstEquationIndex(NO_VALi)
{
	fStandardDeviations[0] = stDevX;
	fStandardDeviations[1] = stDevY;
	fStandardDeviations[2] = stDevZ;
}
#endif