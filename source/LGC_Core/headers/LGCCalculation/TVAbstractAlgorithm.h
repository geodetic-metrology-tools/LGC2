/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TV_ABSTRACT_ALGORITHM
#define TV_ABSTRACT_ALGORITHM


//SURVEYLIB
#include <Behavior.h>

class TLGCData;

/*!
\ingroup LGCCalculation
\brief Abstract class to run the chosen algorithm process
*/
class TVAbstractAlgorithm{

public:
	TVAbstractAlgorithm() : resultMatrices(nullptr){};

	/// This virtual base class destructor does nothing since this is an interface.
	~TVAbstractAlgorithm() {}


	/// abstarct funtion to run the calculation
	virtual Behavior run(TLGCData& data, int fMaxIterations) = 0;


	/// Pointer to the result matrices
	TLSResultsMatrices* resultMatrices;

};

#endif
