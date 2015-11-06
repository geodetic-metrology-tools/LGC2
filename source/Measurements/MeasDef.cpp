#include "MeasDef.h"

///////////////////////////////////////////////////////////
//////Total station measurements
///////////////////////////////////////////////////////////
TPLR3D::TPLR3D(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ESingleValue, 1, EPLR3DAngles, 2>(pos, tgt)
			{}

TLINE::TLINE(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt): 
				TAScalarMeas<TInstrumentData::TPOLAR::TTarget>(pos, tgt) {}

TLINE::TLINE(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt, TLength v) :
				TAScalarMeas<TInstrumentData::TPOLAR::TTarget>(pos, tgt, v) {}

TDLEV::TDHOR::TDHOR(const TAdjustablePoint& pos, TInstrumentData::TLEVEL::TTarget tgt, TLength v) :
				TAScalarMeas<TInstrumentData::TLEVEL::TTarget>(pos, tgt, v), dhorSigma(NO_VALf) {}

///////////////////////////////////////////////////////////
//////Offset measurements
///////////////////////////////////////////////////////////
TECTH::TECTH(const TAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr, TAngle obsHorAngle, TLength v) : 
	TAScalarMeas<TInstrumentData::TSCALE>(stationedPoint, scaleInstr, v),
	obsHorAngle(obsHorAngle)
	{}

///////////////////////////////////////////////////////////
//////Levelling measurements
///////////////////////////////////////////////////////////
TDVER::TDVER(const TAdjustablePoint& station, const TAdjustablePoint& target, TLength obsVal) :
	TAScalarMeas<int>(target, 0, obsVal),
	station(&station),
	fSigmaObsVal(0.0001), /*0.1 mm*/
	fDistanceCorrection(0.0)
	{}

TDLEV::TDLEV(const TAdjustablePoint& targetPos, TInstrumentData::TLEVEL::TTarget target, TLength measValue) :
			TAScalarMeas<TInstrumentData::TLEVEL::TTarget>(targetPos, target, measValue),
			dhor(nullptr)
	{}
