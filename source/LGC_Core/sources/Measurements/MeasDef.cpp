#include <MeasDef.h>

///////////////////////////////////////////////////////////
//////Total station measurements
///////////////////////////////////////////////////////////
TPLR3D::TPLR3D(const LGCAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ESingleValue, 1, EPLR3DAngles, 2>(pos, tgt),
			fAllFixedCs(NO_VALf), fAllFixedHi(NO_VALf)
			{ 
				fAllFixedRx[0] = (TAngle(NO_VALf));
				fAllFixedRy[0] = (TAngle(NO_VALf));
				fAllFixedV0[0] = TAngle(NO_VALf);
				fAllFixedRx[1] = (TAngle(NO_VALf));
				fAllFixedRy[1] = (TAngle(NO_VALf));
				fAllFixedV0[1] = TAngle(NO_VALf);

}

TLINE::TLINE(const LGCAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt): 
TAScalarMeas<TInstrumentData::TPOLAR::TTarget>(pos, tgt), fAllFixedCs(NO_VALf), fAllFixedHi(NO_VALf)
    {}

TLINE::TLINE(const LGCAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt, TLength v) :
TAScalarMeas<TInstrumentData::TPOLAR::TTarget>(pos, tgt, v), fAllFixedCs(NO_VALf), fAllFixedHi(NO_VALf)
    {}



///////////////////////////////////////////////////////////
//////Offset measurements
///////////////////////////////////////////////////////////
TECTH::TECTH(const LGCAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr, TAngle obsHorAngle, TLength v) : 
	TAScalarMeas<TInstrumentData::TSCALE>(stationedPoint, scaleInstr, v),
	obsHorAngle(obsHorAngle), fAllFixedV0(TAngle(NO_VALf))
	{}

TECDIR::TECDIR(const LGCAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr, TAngle horAngle, TAngle vertAngle) :
TAScalarMeas<TInstrumentData::TSCALE>(stationedPoint, scaleInstr),
obsHorAngle(horAngle), obsVertAngle(vertAngle)
{
	fAllFixedV0[0] = (TAngle(NO_VALf));
	fAllFixedV0[1] = (TAngle(NO_VALf));
}
TECDIR::TECDIR(const LGCAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr, TAngle horAngle, TAngle vertAngle, TLength v) :
TAScalarMeas<TInstrumentData::TSCALE>(stationedPoint, scaleInstr, v),
obsHorAngle(horAngle), obsVertAngle(vertAngle)
{
	fAllFixedV0[0] = (TAngle(NO_VALf));
	fAllFixedV0[1] = (TAngle(NO_VALf));
}

TECSP::TECSP(const LGCAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr) :
    TAScalarMeas<TInstrumentData::TSCALE>(stationedPoint, scaleInstr)
{}

TECSP::TECSP(const LGCAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr, TLength v) :
    TAScalarMeas<TInstrumentData::TSCALE>(stationedPoint, scaleInstr, v)
{}

///////////////////////////////////////////////////////////
//////Levelling measurements
///////////////////////////////////////////////////////////
TDVER::TDVER(const LGCAdjustablePoint& station, const LGCAdjustablePoint& target, TLength obsVal) :
	TAScalarMeas<int>(target, 0, obsVal),
	station(&station),
	fSigmaObsVal(0.0001), /*0.1 mm*/
	fDistanceCorrection(0.0)
	{}

TDLEV::TDLEV(const LGCAdjustablePoint& targetPos, TInstrumentData::TLEVEL::TTarget target, TLength measValue) :
	TAScalarMeas<TInstrumentData::TLEVEL::TTarget>(targetPos, target, measValue),
	dhor(nullptr), fAllFixedCollimation(TAngle(NO_VALf))
	{}

TDLEV::TDHOR::TDHOR(const LGCAdjustablePoint& pos, TInstrumentData::TLEVEL::TTarget tgt, TLength v) :
    TAScalarMeas<TInstrumentData::TLEVEL::TTarget>(pos, tgt, v), dhorSigma(NO_VALf) 
    {}
///////////////////////////////////////////////////////////
//////Orientation measurements
///////////////////////////////////////////////////////////
TPdorObs::TPdorObs(const LGCAdjustablePoint& cala, const LGCAdjustablePoint& orientation, TAngle gis) :
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
{
    targetPos = nullptr;
}

void TPdorObs::Initialise(LGCAdjustablePoint& cala, LGCAdjustablePoint& ori, TAngle gis, bool hasbearing)
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
    fSigmaObsVal(1.0, TLength::EUnits::kMillimetres),
    TAMeas<int>(*station, 0)
{
    targetPos = nullptr;
}

TRADI::TRADI(const LGCAdjustablePoint& point, TAngle obsVal, TLength sig) :
    station(&point),
    fAngleCnstr(obsVal),
    fResidual(NO_VALf),
    fSigmaObsVal(sig),
    TAMeas<int>(point, 0)
{}
