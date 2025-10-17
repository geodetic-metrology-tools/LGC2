/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
	virtual ~TVAbstractAlgorithm() {}


	/// abstarct funtion to run the calculation
	virtual Behavior run(TLGCData& data, int fMaxIterations) = 0;


	/// Pointer to the result matrices
	std::shared_ptr<TLSResultsMatrices> resultMatrices;

};

#endif
