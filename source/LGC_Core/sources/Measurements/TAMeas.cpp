#include "LGC_Core/headers/Measurements/TAMeas.h"

#include <LGCAdjustablePoint.h>

#ifdef USE_SERIALIZER
template<typename TTarget>
void TAMeas<TTarget>::serialize(SerializerObject::SerializationHelper &obj) const
{
	TStatusObject::serialize(obj);
	obj.addProperty("eolcomment", eolcomment);
	obj.addProperty("fFirstEquationIndex", fFirstEquationIndex);
	obj.addProperty("fFirstObservationIndex", fFirstObservationIndex);
	obj.addProperty("line", line);
	obj.addProperty("measCounter", measCounter);
	obj.addProperty("measId", measId);
	obj.addProperty("target", target);
	obj.addProperty("targetPos", targetPos);
}
// Explicit template instantiation
template class TAMeas<TInstrumentData::TPOLAR::TTarget>;
template class TAMeas<TInstrumentData::TCAMD::TTarget>;
template class TAMeas<TInstrumentData::TEDM::TTarget>;
template class TAMeas<TInstrumentData::TLEVEL::TTarget>;
template class TAMeas<TInstrumentData::TSCALE>;
template class TAMeas<TInstrumentData::TINCL>;
template class TAMeas<TInstrumentData::THLSR>;
template class TAMeas<int>;

template<typename TTarget>
void TAVectorMeas<TTarget>::serialize(SerializerObject::SerializationHelper &obj) const
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
template class TAVectorMeas<int>;
#endif
