#include <LGCAdjustablePoint.h>
#include <MeasDef.h>

///////////////////////////////////////////////////////////
//////Total station measurements
///////////////////////////////////////////////////////////
TPLR3D::TPLR3D(const LGCAdjustablePoint &pos, TInstrumentData::TPOLAR::TTarget tgt) :
	TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ESingleValue, 1, EPLR3DAngles, 2>(pos, tgt), fAllFixedCs(NO_VALf), fAllFixedHi(NO_VALf), fDistSensi(NO_VALf)
{
	fAllFixedRx[0] = (TAngle(NO_VALf));
	fAllFixedRy[0] = (TAngle(NO_VALf));
	fAllFixedV0[0] = TAngle(NO_VALf);
	fAllFixedRx[1] = (TAngle(NO_VALf));
	fAllFixedRy[1] = (TAngle(NO_VALf));
	fAllFixedV0[1] = TAngle(NO_VALf);
}

TLINE::TLINE(const LGCAdjustablePoint &pos, TInstrumentData::TPOLAR::TTarget tgt) :
	TAScalarMeas<TInstrumentData::TPOLAR::TTarget>(pos, tgt), fAllFixedCs(NO_VALf), fAllFixedHi(NO_VALf), fDistSensi(NO_VALf)
{
}

TLINE::TLINE(const LGCAdjustablePoint &pos, TInstrumentData::TPOLAR::TTarget tgt, TLength v) :
	TAScalarMeas<TInstrumentData::TPOLAR::TTarget>(pos, tgt, v), fAllFixedCs(NO_VALf), fAllFixedHi(NO_VALf), fDistSensi(NO_VALf)
{
}

///////////////////////////////////////////////////////////
//////Offset measurements
///////////////////////////////////////////////////////////
TECTH::TECTH(const LGCAdjustablePoint &stationedPoint, TInstrumentData::TSCALE scaleInstr, TAngle obsHorAngle, TLength v) :
	TAScalarMeas<TInstrumentData::TSCALE>(stationedPoint, scaleInstr, v), obsHorAngle(obsHorAngle), fAllFixedV0(TAngle(NO_VALf))
{
}

TECDIR::TECDIR(const LGCAdjustablePoint &stationedPoint, TInstrumentData::TSCALE scaleInstr, TAngle horAngle, TAngle vertAngle) :
	TAScalarMeas<TInstrumentData::TSCALE>(stationedPoint, scaleInstr), obsHorAngle(horAngle), obsVertAngle(vertAngle)
{
	fAllFixedV0[0] = (TAngle(NO_VALf));
	fAllFixedV0[1] = (TAngle(NO_VALf));
}
TECDIR::TECDIR(const LGCAdjustablePoint &stationedPoint, TInstrumentData::TSCALE scaleInstr, TAngle horAngle, TAngle vertAngle, TLength v) :
	TAScalarMeas<TInstrumentData::TSCALE>(stationedPoint, scaleInstr, v), obsHorAngle(horAngle), obsVertAngle(vertAngle)
{
	fAllFixedV0[0] = (TAngle(NO_VALf));
	fAllFixedV0[1] = (TAngle(NO_VALf));
}

TECSP::TECSP(const LGCAdjustablePoint &stationedPoint, TInstrumentData::TSCALE scaleInstr) : TAScalarMeas<TInstrumentData::TSCALE>(stationedPoint, scaleInstr)
{
}

TECSP::TECSP(const LGCAdjustablePoint &stationedPoint, TInstrumentData::TSCALE scaleInstr, TLength v) :
	TAScalarMeas<TInstrumentData::TSCALE>(stationedPoint, scaleInstr, v)
{
}

///////////////////////////////////////////////////////////
//////Levelling measurements
///////////////////////////////////////////////////////////
TDVER::TDVER(const LGCAdjustablePoint &station, const LGCAdjustablePoint &target, TLength obsVal) :
	TAScalarMeas<int>(target, 0, obsVal),
	station(&station),
	fSigmaObsVal(0.0001), /*0.1 mm*/
	fDistanceCorrection(0.0)
{
}

TDLEV::TDLEV(const LGCAdjustablePoint &targetPos, TInstrumentData::TLEVEL::TTarget target, TLength measValue) :
	TAScalarMeas<TInstrumentData::TLEVEL::TTarget>(targetPos, target, measValue), dhor(nullptr), fAllFixedCollimation(TAngle(NO_VALf))
{
}

TDLEV::TDHOR::TDHOR(const LGCAdjustablePoint &pos, TInstrumentData::TLEVEL::TTarget tgt, TLength v) : TAScalarMeas<TInstrumentData::TLEVEL::TTarget>(pos, tgt, v)
{
}
///////////////////////////////////////////////////////////
//////Orientation measurements
///////////////////////////////////////////////////////////
TPdorObs::TPdorObs(const LGCAdjustablePoint &cala, const LGCAdjustablePoint &orientation, TAngle gis) :
	TAMeas<int>(orientation, 0), calaPt(&cala), orientationPt(&orientation), fSigmaObsVal(0.0000001, TAngle::EUnits::kGons), fbearing(gis), fIsInitialise(true), fDefined(true)
{
}

TPdorObs::TPdorObs() :
	calaPt(nullptr),
	orientationPt(nullptr),
	fSigmaObsVal(0.0000001, TAngle::EUnits::kGons),
	fbearing(NO_VALf),
	TAMeas<int>(*orientationPt, 0),
	fIsInitialise(false),
	fDefined(false),
	fbearingResidual(NO_VALf)
{
	// Set TAMeas<int>::targetPos nullptr here, since the TAMeas
	// constructor leaves an invalid reference as its value:
	targetPos = nullptr;
}

void TPdorObs::Initialise(LGCAdjustablePoint &cala, LGCAdjustablePoint &ori, TAngle gis, bool hasbearing)
{
	calaPt = &cala;
	orientationPt = &ori;
	fbearing = gis;
	fIsInitialise = true;
	fDefined = hasbearing;
}

TRADI::TRADI() : station(nullptr), fAngleCnstr(NO_VALf), fResidual(NO_VALf), fSigmaObsVal(1.0, TLength::EUnits::kMillimetres), TAMeas<int>(*station, 0)
{
	// Set TAMeas<int>::targetPos nullptr here, since the TAMeas
	// constructor leaves an invalid reference as its value:
	targetPos = nullptr;
}

TRADI::TRADI(const LGCAdjustablePoint &point, TAngle obsVal, TLength sig) :
	station(&point), fAngleCnstr(obsVal), fResidual(NO_VALf), fSigmaObsVal(sig), TAMeas<int>(point, 0)
{
}

TOBSXYZ::TOBSXYZ() :
	station(nullptr),
	obsValue(TPositionVector(TCoordSysFactory::ECoordSys::k3DCartesian)),
	fXResidual(NO_VALf),
	fYResidual(NO_VALf),
	fZResidual(NO_VALf),
	fXSigmaObsVal(NO_VALf),
	fYSigmaObsVal(NO_VALf),
	fZSigmaObsVal(NO_VALf),
	TAMeas<int>(*station, 0)
{
	// Set TAMeas<int>::targetPos nullptr here, since the TAMeas
	// constructor leaves an invalid reference as its value:
	targetPos = nullptr;
}

TOBSXYZ::TOBSXYZ(const LGCAdjustablePoint &point, TPositionVector pos, TLength sigX, TLength sigY, TLength sigZ, TDataTreeIterator itTree) :
	station(&point),
	obsValue(pos),
	fXResidual(NO_VALf),
	fYResidual(NO_VALf),
	fZResidual(NO_VALf),
	fXSigmaObsVal(sigX),
	fYSigmaObsVal(sigY),
	fZSigmaObsVal(sigZ),
	positionInTree(itTree),
	TAMeas<int>(point, 0)
{
}

#if USE_SERIALIZER

void TPLR3D::serialize(ObjectSerializer &obj) const
{
	TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ESingleValue, 1, EPLR3DAngles, 2>::serialize(obj);
	obj.addProperty("fAllFixedCs", fAllFixedCs.getMetresValue());
	obj.addProperty("fAllFixedHi", fAllFixedHi.getMetresValue());
	obj.addProperty("fAllFixedRx", fAllFixedRx);
	obj.addProperty("fAllFixedRy", fAllFixedRy);
	obj.addProperty("fAllFixedV0", fAllFixedV0);
	// sensibility stored unitless
	obj.addProperty("fDistSensi", fDistSensi);
}

void TANGL::serialize(ObjectSerializer &obj) const
{
	TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1>::serialize(obj);
	obj.addProperty("fAllFixedV0", fAllFixedV0.getRadiansValue());
}

void TZEND::serialize(ObjectSerializer &obj) const
{
	TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1>::serialize(obj);
	obj.addProperty("fAllFixedHi", fAllFixedHi.getMetresValue());
}

void TLINE::serialize(ObjectSerializer &obj) const
{
	TAScalarMeas<TInstrumentData::TPOLAR::TTarget>::serialize(obj);
	obj.addProperty("fAllFixedCs", fAllFixedCs.getMetresValue());
	obj.addProperty("fAllFixedHi", fAllFixedHi.getMetresValue());
	// sensibility stored unitless
	obj.addProperty("fDistSensi", fDistSensi);
}

void TECTH::serialize(ObjectSerializer &obj) const
{
	TAScalarMeas<TInstrumentData::TSCALE>::serialize(obj);
	obj.addProperty("fAllFixedV0", fAllFixedV0.getRadiansValue());
	obj.addProperty("obsHorAngle", obsHorAngle.getRadiansValue());
}

void TECDIR::serialize(ObjectSerializer &obj) const
{
	TAScalarMeas<TInstrumentData::TSCALE>::serialize(obj);
	obj.addProperty("fAllFixedV0", fAllFixedV0);
	obj.addProperty("obsHorAngle", obsHorAngle.getRadiansValue());
	obj.addProperty("obsVertAngle", obsVertAngle.getRadiansValue());
}

void TDSPT::serialize(ObjectSerializer &obj) const
{
	TAScalarMeas<TInstrumentData::TEDM::TTarget>::serialize(obj);
	obj.addProperty("fAllFixedCs", fAllFixedCs.getMetresValue());
	// sensibility stored unitless
	obj.addProperty("fDistSensi", fDistSensi);
}

void TDVER::serialize(ObjectSerializer &obj) const
{
	TAScalarMeas<int>::serialize(obj);
	obj.addProperty("eolcomment", eolcomment);
	obj.addProperty("fDistanceCorrection", fDistanceCorrection.getMetresValue());
	obj.addProperty("fSigmaObsVal", fSigmaObsVal.getMetresValue());
	obj.addProperty("line", line);
	obj.addProperty("station", station);
}

void TDLEV::TDHOR::serialize(ObjectSerializer &obj) const
{
	TAScalarMeas<TInstrumentData::TLEVEL::TTarget>::serialize(obj);
}

void TDLEV::serialize(ObjectSerializer &obj) const
{
	TAScalarMeas<TInstrumentData::TLEVEL::TTarget>::serialize(obj);
	obj.addProperty("dhor", dhor);
	obj.addProperty("fAllFixedCollimation", fAllFixedCollimation.getRadiansValue());
}

void TRADI::serialize(ObjectSerializer &obj) const
{
	TAMeas<int>::serialize(obj);
	obj.addProperty("eolcomment", eolcomment);
	obj.addProperty("fAngleCnstr", fAngleCnstr.getRadiansValue());
	obj.addProperty("fConstAngleVal", fConstAngleVal.getRadiansValue());
	obj.addProperty("fResidual", fResidual.getMetresValue());
	obj.addProperty("fSigmaObsVal", fSigmaObsVal.getMetresValue());
	obj.addProperty("line", line);
	obj.addProperty("station", station);
}

void TPdorObs::serialize(ObjectSerializer &obj) const
{
	TAMeas<int>::serialize(obj);
	if (calaPt)
		obj.addProperty("calaPt", calaPt->getName());
	obj.addProperty("eolcomment", eolcomment);
	obj.addProperty("fbearing", fbearing.getRadiansValue());
	obj.addProperty("fbearingResidual", fbearingResidual.getRadiansValue());
	obj.addProperty("fDefined", fDefined);
	obj.addProperty("fIsInitialise", fIsInitialise);
	obj.addProperty("fSigmaObsVal", fSigmaObsVal.getRadiansValue());
	obj.addProperty("line", line);
	if (orientationPt)
		obj.addProperty("orientationPt", orientationPt->getName());
}

void TOBSXYZ::serialize(ObjectSerializer &obj) const
{
	TAMeas<int>::serialize(obj);
	obj.addProperty("eolcomment", eolcomment);
	obj.addProperty("fXResidual", fXResidual.getMetresValue());
	obj.addProperty("fXSigmaObsVal", fXSigmaObsVal.getMetresValue());
	obj.addProperty("fYResidual", fYResidual.getMetresValue());
	obj.addProperty("fYSigmaObsVal", fYSigmaObsVal.getMetresValue());
	obj.addProperty("fZResidual", fZResidual.getMetresValue());
	obj.addProperty("fZSigmaObsVal", fZSigmaObsVal.getMetresValue());
	obj.addProperty("line", line);
	obj.addProperty("station", station);
	obj.addProperty("obsValue", obsValue);
}

void TECWS::serialize(ObjectSerializer &obj) const
{
	TAScalarMeas<TInstrumentData::THLSR>::serialize(obj);
	obj.addProperty("line", line);
}

void TECWI::serialize(ObjectSerializer &obj) const
{
	TAScalarMeas<TInstrumentData::TWPSR, EECWIDistances, 2, ENoValues, 0>::serialize(obj);
	obj.addProperty("line", line);
}

#endif
