/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_ALLFIXED
#define TLS_ALLFIXED


//SURVEYLIB
#include <Behavior.h>
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
	virtual Behavior run(TLGCData& data, int fMaxIterations);

private:
	TAllfixedParamGenerator fAllfixedGenerator; 
	TPointTransformer fPointTransformer;
};
#endif