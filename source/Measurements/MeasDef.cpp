#include "MeasDef.h"

///////////////////////////////////////////////////////////
//////Total station measurements
///////////////////////////////////////////////////////////
TPLR3D::TPLR3D(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ESingleValue, 1, EPLR3DAngles, 2>(pos, tgt)
			{}

TLINE::TLINE(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt): 
				TAScalarMeas<TInstrumentData::TPOLAR::TTarget>(pos, tgt) {}

TLINE::TLINE(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt, TReal v) : 
				TAScalarMeas<TInstrumentData::TPOLAR::TTarget>(pos, tgt, v) {}

TDLEV::TDHOR::TDHOR(const TAdjustablePoint& pos, TInstrumentData::TLEVEL::TTarget tgt, TReal v) :
				TAScalarMeas<TInstrumentData::TLEVEL::TTarget>(pos, tgt, v), dhorSigma(NO_VALf) {}

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
	TAScalarMeas<int>(target, 0, obsVal),
	station(&station),
	fSigmaObsVal(0.0001), /*0.1 mm*/
	fDistanceCorrection(0.0)
	{}

TDLEV::TDLEV(const TAdjustablePoint& targetPos, TInstrumentData::TLEVEL::TTarget target, TReal measValue) :
			TAScalarMeas<TInstrumentData::TLEVEL::TTarget>(targetPos, target, measValue),
			dhor(nullptr)
	{}
