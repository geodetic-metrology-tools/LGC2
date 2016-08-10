#ifndef _TUVD_H_
#define _TUVD_H_

#include "TAMeas.h"
#include "UEOIndices.h"

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

	private:
		// distance of the target
      TLength sdist;

		// distance residual of the target
      TLength sdistResidual;
};
#endif