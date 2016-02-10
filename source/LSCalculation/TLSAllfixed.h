#ifndef TLS_ALLFIXED
#define TLS_ALLFIXED

#include "TLSAlgorithm.h"
#include "TPointTransformer.h"
#include "TAllfixedParamGenerator.h"


/*!
\ingroup AllfixedCalculation

\brief Class responsible for running the calculation in the allfixed mode.
*/
class TLSAllfixed : public TLSAlgorithm{
public:

	/// Constructor
	TLSAllfixed(TLGCData& data, int maxIter);

	//run the calculation
	virtual bool run(TLGCData& data, int fMaxIterations);

private:
	TAllfixedParamGenerator fAllfixedGenerator; 
	TPointTransformer fPointTransformer;
};
#endif