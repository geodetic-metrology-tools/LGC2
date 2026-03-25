// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "LGC_Core/headers/Measurements/TAMeas.h"

#include <LGCAdjustablePoint.h>

#include "LGC_Core/headers/Measurements/MeasDef.h"

#if USE_SERIALIZER
template<typename TTarget>
void TAMeas<TTarget>::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("eolcomment", eolcomment);
	obj.addProperty("fMeasIdx", fMeasIdx);
	obj.addProperty("line", line);
	obj.addProperty("obsID", obsID);
	obj.addProperty("measCounter", measCounter);
	obj.addProperty("measId", measId);
	obj.addProperty("target", target);
	if (targetPos)
		obj.addProperty("targetPos", targetPos->getName());
}
// Explicit template instantiation
template class TAMeas<TInstrumentData::TPOLAR::TTarget>;
template class TAMeas<TInstrumentData::TCAMD::TTarget>;
template class TAMeas<TInstrumentData::TEDM::TTarget>;
template class TAMeas<TInstrumentData::TLEVEL::TTarget>;
template class TAMeas<TInstrumentData::TSCALE>;
template class TAMeas<TInstrumentData::TINCL>;
template class TAMeas<TInstrumentData::THLSR>;
template class TAMeas<TInstrumentData::TWPSR>;
template class TAMeas<int>;

template<typename TTarget, typename TEnumDistance, int numDistances, typename TEnumAngle, int numAngles>
void TAScalarMeas<TTarget, TEnumDistance, numDistances, TEnumAngle, numAngles>::serialize(ObjectSerializer &obj) const
{
	TAMeas<TTarget>::serialize(obj);
	obj.addProperty("angles", angles);
	obj.addProperty("anglesResiduals", anglesResiduals);
	obj.addProperty("distances", distances);
	obj.addProperty("distancesResiduals", distancesResiduals);
}
// Explicit template instantiation
template class TAScalarMeas<TInstrumentData::TPOLAR::TTarget>;
template class TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1>;
template class TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ESingleValue, 1, EPLR3DAngles, 2>;
template class TAScalarMeas<TInstrumentData::TCAMD::TTarget>;
template class TAScalarMeas<TInstrumentData::TEDM::TTarget>;
template class TAScalarMeas<TInstrumentData::TLEVEL::TTarget>;
template class TAScalarMeas<TInstrumentData::TSCALE>;
template class TAScalarMeas<TInstrumentData::TINCL>;
template class TAScalarMeas<TInstrumentData::TINCL, ENoValues, 0, ESingleValue, 1>;
template class TAScalarMeas<TInstrumentData::THLSR>;
template class TAScalarMeas<TInstrumentData::TWPSR, EECWIDistances, 2, ENoValues, 0>;
template class TAScalarMeas<int>;

template<typename TTarget>
void TAVectorMeas<TTarget>::serialize(ObjectSerializer &obj) const
{
	TAMeas<TTarget>::serialize(obj);
	obj.addProperty("vector", vector);
	obj.addProperty("XcompResidual", XcompResidual);
	obj.addProperty("YcompResidual", YcompResidual);
}
// Explicit template instantiation
template class TAVectorMeas<TInstrumentData::TPOLAR::TTarget>;
template class TAVectorMeas<TInstrumentData::TCAMD::TTarget>;
template class TAVectorMeas<TInstrumentData::TEDM::TTarget>;
template class TAVectorMeas<TInstrumentData::TLEVEL::TTarget>;
template class TAVectorMeas<TInstrumentData::TSCALE>;
template class TAVectorMeas<TInstrumentData::TINCL>;
template class TAVectorMeas<TInstrumentData::THLSR>;
template class TAVectorMeas<TInstrumentData::TWPSR>;
template class TAVectorMeas<int>;
#endif
