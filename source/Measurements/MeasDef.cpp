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

///////////////////////////////////////////////////////////
//////Orientation measurements
///////////////////////////////////////////////////////////
TPdorObs::TPdorObs(const TAdjustablePoint& cala, const TAdjustablePoint& orientation, TAngle gis) :
TAMeas<int>(orientation, 0),
calaPt(&cala),
orientationPt(&orientation),
fSigmaObsVal(0.0000001, TAngle::EUnits::kGons),
fbearing(gis),
fIsInitialise(true),
fDefined(true)
{}

TPdorObs::TPdorObs(): 
calaPt(nullptr),
orientationPt(nullptr),
fSigmaObsVal(0.0000001, TAngle::EUnits::kGons),
fbearing(NO_VALf),
TAMeas<int>(*orientationPt, 0),
fIsInitialise(false),
fDefined(false),
fbearingResidual(NO_VALf)
{}

void TPdorObs::Initialise(TAdjustablePoint& cala, TAdjustablePoint& ori, TAngle gis, bool hasbearing)
{
	calaPt = &cala;
	orientationPt = &ori;
	fbearing = gis;
	fIsInitialise = true;
	fDefined = hasbearing;
}

TRADI::TRADI() :
station(nullptr),
fAngleCnstr(NO_VALf),
fResidual(NO_VALf),
fSigmaObsVal(0.0),
TAMeas<int>(*station, 0)
{}

TRADI::TRADI(const TAdjustablePoint& point, TAngle obsVal, TLength sig) :
station(&point),
fAngleCnstr(obsVal),
fResidual(NO_VALf),
fSigmaObsVal(sig),
TAMeas<int>(*station, 0)
{}
