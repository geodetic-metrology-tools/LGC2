#include <MeasDef.h>
#include <LGCAdjustablePoint.h>

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
    // Set TAMeas<int>::targetPos nullptr here, since the TAMeas
    // constructor leaves an invalid reference as its value:
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
    // Set TAMeas<int>::targetPos nullptr here, since the TAMeas
    // constructor leaves an invalid reference as its value:
    targetPos = nullptr;
}

TRADI::TRADI(const LGCAdjustablePoint& point, TAngle obsVal, TLength sig) :
    station(&point),
    fAngleCnstr(obsVal),
    fResidual(NO_VALf),
    fSigmaObsVal(sig),
    TAMeas<int>(point, 0)
{}

TOBSXYZ::TOBSXYZ() :
station(nullptr),
initialValue(TPositionVector(TCoordSysFactory::ECoordSys::k3DCartesian)),
fXResidual(NO_VALf), fYResidual(NO_VALf), fZResidual(NO_VALf),
fXSigmaObsVal(NO_VALf), fYSigmaObsVal(NO_VALf), fZSigmaObsVal(NO_VALf),
TAMeas<int>(*station, 0)
{
    // Set TAMeas<int>::targetPos nullptr here, since the TAMeas
    // constructor leaves an invalid reference as its value:
    targetPos = nullptr;
}

TOBSXYZ::TOBSXYZ(const LGCAdjustablePoint& point, TPositionVector pos, TLength sigX, TLength sigY, TLength sigZ, TDataTreeIterator itTree) :
station(&point),
initialValue(pos),
fXResidual(NO_VALf), fYResidual(NO_VALf), fZResidual(NO_VALf),
fXSigmaObsVal(sigX), fYSigmaObsVal(sigY), fZSigmaObsVal(sigZ),
positionInTree(itTree),
TAMeas<int>(point, 0)
{}



#ifdef USE_SERIALIZER

void TPLR3D::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("fAllFixedCs", fAllFixedCs);
	obj.addProperty("fAllFixedHi", fAllFixedHi);
	obj.addProperty("fAllFixedRx", fAllFixedRx);
	obj.addProperty("fAllFixedRy", fAllFixedRy);
	obj.addProperty("fAllFixedV0", fAllFixedV0);
}

void TANGL::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("fAllFixedV0", fAllFixedV0);
}

void TZEND::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("fAllFixedHi", fAllFixedHi);
}

void TLINE::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("fAllFixedCs", fAllFixedCs);
	obj.addProperty("fAllFixedHi", fAllFixedHi);
}

void TECTH::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("fAllFixedV0", fAllFixedV0);
	obj.addProperty("obsHorAngle", obsHorAngle);
}

void TECDIR::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("fAllFixedV0", fAllFixedV0);
	obj.addProperty("obsHorAngle", obsHorAngle);
	obj.addProperty("obsVertAngle", obsVertAngle);
}

void TDSPT::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("fAllFixedCs", fAllFixedCs);
}

void TDVER::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("eolcomment", eolcomment);
	obj.addProperty("fDistanceCorrection", fDistanceCorrection);
	obj.addProperty("fSigmaObsVal", fSigmaObsVal);
	obj.addProperty("line", line);
	obj.addProperty("station", station);
}

void TDLEV::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("dhor", dhor);
	obj.addProperty("fAllFixedCollimation", fAllFixedCollimation);
}

void TRADI::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("eolcomment", eolcomment);
	obj.addProperty("fAngleCnstr", fAngleCnstr);
	obj.addProperty("fConstAngleVal", fConstAngleVal);
	obj.addProperty("fResidual", fResidual);
	obj.addProperty("fSigmaObsVal", fSigmaObsVal);
	obj.addProperty("line", line);
	obj.addProperty("station", station);
}

void TPdorObs::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("calaPt", calaPt);
	obj.addProperty("eolcomment", eolcomment);
	obj.addProperty("fbearing", fbearing);
	obj.addProperty("fbearingResidual", fbearingResidual);
	obj.addProperty("fDefined", fDefined);
	obj.addProperty("fIsInitialise", fIsInitialise);
	obj.addProperty("fSigmaObsVal", fSigmaObsVal);
	obj.addProperty("line", line);
	obj.addProperty("orientationPt", orientationPt);
}

void TOBSXYZ::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("eolcomment", eolcomment);
	obj.addProperty("fXResidual", fXResidual);
	obj.addProperty("fXSigmaObsVal", fXSigmaObsVal);
	obj.addProperty("fYResidual", fYResidual);
	obj.addProperty("fYSigmaObsVal", fYSigmaObsVal);
	obj.addProperty("fZResidual", fZResidual);
	obj.addProperty("fZSigmaObsVal", fZSigmaObsVal);
	obj.addProperty("initialValue", initialValue);
	obj.addProperty("line", line);
	//obj.addProperty("positionInTree", positionInTree);
	obj.addProperty("station", station);
}

void TECWS::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("line", line);
	obj.addProperty("station", station);
	obj.addProperty("wsHeight", wsHeight);
}

#endif


