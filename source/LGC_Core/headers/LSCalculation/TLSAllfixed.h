#ifndef TLS_ALLFIXED
#define TLS_ALLFIXED

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

//LGC
#include <TLSAlgorithm.h>
#include <TPointTransformer.h>
#include <TAllfixedParamGenerator.h>


/*!
\ingroup LSAlgorithm
\brief Class responsible for running the calculation in the allfixed mode.
*/
class TLSAllfixed : public TLSAlgorithm{
public:

	/// Constructor
	TLSAllfixed(TLGCData& data, int maxIter);

	/// Run the calculation
	virtual bool run(TLGCData& data, int fMaxIterations);

private:
	TAllfixedParamGenerator fAllfixedGenerator; 
	TPointTransformer fPointTransformer;
};
#endif