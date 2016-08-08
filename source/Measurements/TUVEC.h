#ifndef _TUVEC_H_
#define _TUVEC_H_

#include "TAMeas.h"
#include "UEOIndices.h"

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

		/// Returns the last equation index of this measurement (UVEC introduces 2 equations). 
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex() + 1;}

		/// Returns the last observation index of this measurement (UVEC introduces 2 equations). 
		inline MatrixIndex getLastObservationIndex() const {return getFirstObservationIndex() + 1;}

};
#endif