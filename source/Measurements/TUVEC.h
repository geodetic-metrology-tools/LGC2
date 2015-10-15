#ifndef _TUVEC_H_
#define _TUVEC_H_

#include "TAMeas.h"


/// A UVEC measurement
class TUVEC : public TAVectorMeas<TInstrumentData::TCAMD::TTarget> 
{
	public:

		TUVEC(const TAdjustablePoint& pos, TInstrumentData::TCAMD::TTarget tgt) : 
			TAVectorMeas(pos, tgt),
			fFirstObservationIndex(NO_VALi)
		{}

		/*!Returns the last LS-matrices equation index of this measurement (DIR3D introduces 3 equations) */
		inline MatrixIndex getLastEquationIndex() {return getFirstEquationIndex() + 2;}

		/*! Returns the last observation index of this measurement (DIR3D introduces 2 observations, order is defined to be: theta, phi). */
		inline MatrixIndex getLastObservationIndex() {return fFirstObservationIndex + 2;}

	private:
		//An LS-matrices observatin index of a first observation in this measurement.
		MatrixIndex fFirstObservationIndex;
};
#endif