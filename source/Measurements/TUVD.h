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

		TUVD(const TAdjustablePoint& pos, TInstrumentData::TCAMD::TTarget tgt) : 
			TAVectorMeas<TInstrumentData::TCAMD::TTarget>(pos, tgt)
		{}

		/// Returns the last LS-matrices equation index of this measurement 
		inline MatrixIndex getLastEquationIndex() {return getFirstEquationIndex() + 2;}

		/// Returns the last observation index of this measurement 
		inline MatrixIndex getLastObservationIndex() {return getFirstObservationIndex() + 2;}

		/// Sets the new distance 
		void setDistance(TReal dist) {sdist = dist;}

		/// Returns the measured distance 
		TReal getDistance() const {return sdist;}

		/// Returns the distance residual 
		TReal getDistanceResidual() const {return sdistResidual;}

		/// Sets the distance residual 
		void setDistanceResidual(TReal sDistRes) {sdistResidual = sDistRes;}

	private:
		// distance of the target
		TReal sdist;

		// distance residual of the target
		TReal sdistResidual;
};
#endif