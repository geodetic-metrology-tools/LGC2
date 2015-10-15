#ifndef _TUVD_H_
#define _TUVD_H_

#include "TAMeas.h"


/// A UVD measurement
class TUVD : public TAVectorMeas<TInstrumentData::TCAMD::TTarget> 
{
	public:

		TUVD(const TAdjustablePoint& pos, TInstrumentData::TCAMD::TTarget tgt) : 
			TAVectorMeas(pos, tgt),
			fFirstObservationIndex(NO_VALi)
		{}

		/*!Returns the last LS-matrices equation index of this measurement */
		inline MatrixIndex getLastEquationIndex() {return getFirstEquationIndex() + 3;}

		/*! Returns the last observation index of this measurement */
		inline MatrixIndex getLastObservationIndex() {return fFirstObservationIndex + 3;}

		/*! Sets the new distance */
		void setDistance(TReal dist) {sdist = dist;}

		/*! Retrieves the measured distance */
		TReal getDistance() const {return sdist;}

		/*! Retrieves the distance residual */
		TReal getDistanceResidual() const {return sdist;}

		/*! Sets the distance residual */
		void setDistanceResidual(TReal sDistRes) {sdistResidual = sDistRes;}



	private:
		//An LS-matrices observatin index of a first observation in this measurement.
		MatrixIndex fFirstObservationIndex;

		// distance to the target
		TReal sdist;

		// distance to the target
		TReal sdistResidual;
};
#endif