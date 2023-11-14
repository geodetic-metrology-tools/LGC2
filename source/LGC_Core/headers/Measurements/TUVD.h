/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _TUVD_H_
#define _TUVD_H_

//SURVEYLIB
#include <UEOIndices.h>
//LGC
#include <TAMeas.h>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER
/*!
	\ingroup Measurements
	\brief Unit vector + distance measurement made by a camera (TCAM).
*/
class TUVD : public TAVectorMeas<TInstrumentData::TCAMD::TTarget> 
{
	public:

		TUVD(const LGCAdjustablePoint& pos, TInstrumentData::TCAMD::TTarget tgt) : 
			TAVectorMeas<TInstrumentData::TCAMD::TTarget>(pos, tgt)
		{}

		/// Returns the last LS-matrices equation index of this measurement 
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex() + 2;}

		/// Returns the last observation index of this measurement 
		inline MatrixIndex getLastObservationIndex() const {return getFirstObservationIndex() + 2;}

		/// Sets the new distance 
      void setDistance(TLength dist) { sdist = dist; }

      /// Sets the distance residual 
      void setDistanceResidual(TLength sDistRes) { sdistResidual = sDistRes; }

		/// Returns the measured distance 
      TLength getDistance() const { return sdist; }

		/// Returns the distance residual 
      TLength getDistanceResidual() const { return sdistResidual; }

#if USE_SERIALIZER
	  // Inherited via Serializable
	  virtual void serialize(ObjectSerializer &obj) const override
	  {
		  TAVectorMeas<TInstrumentData::TCAMD::TTarget>::serialize(obj);
		  obj.addProperty("sdist", sdist.getMetresValue());
		  obj.addProperty("sdistResidual", sdistResidual.getMetresValue());
	  }
#endif

	private:
		// distance of the target
      TLength sdist;

		// distance residual of the target
      TLength sdistResidual;
};
#endif