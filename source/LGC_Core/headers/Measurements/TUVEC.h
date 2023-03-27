/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _TUVEC_H_
#define _TUVEC_H_


//SURVEYLIB
#include <UEOIndices.h>
//LGC
#include <TAMeas.h>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER
/*!
	\ingroup Measurements
	\brief Unit vector measurement made by a camera (TCAM).
*/
class TUVEC : public TAVectorMeas<TInstrumentData::TCAMD::TTarget> 
{
	public:
		/// Constructor
		TUVEC(const LGCAdjustablePoint& pos, TInstrumentData::TCAMD::TTarget tgt) : 
			TAVectorMeas<TInstrumentData::TCAMD::TTarget>(pos, tgt)
		{}
#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(SerializerObject::SerializationHelper &obj) const override
		{ 
			TAVectorMeas<TInstrumentData::TCAMD::TTarget>::serialize(obj);
		}
#endif
		/// Returns the last equation index of this measurement (UVEC introduces 2 equations). 
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex() + 1;}

		/// Returns the last observation index of this measurement (UVEC introduces 2 equations). 
		inline MatrixIndex getLastObservationIndex() const {return getFirstObservationIndex() + 1;}

};
#endif