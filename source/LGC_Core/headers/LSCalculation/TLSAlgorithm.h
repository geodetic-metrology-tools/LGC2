/*
© Copyright CERN 2000-2023. All rights reserved. This software is released under a CERN proprietary software license.
Any permission to use it shall be granted in writing. Request shall be addressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_ALGORITHM
#define TLS_ALGORITHM

//STL
#include <random>
//SURVEYLIB
#include <TALSComputer.h>
#include <Behavior.h>
//LGC
#include <TVAbstractAlgorithm.h>
#include <TLSResultsMatricesExtractor.h>
#include <TPointTransformer.h>
#include <TLSConsistencyCheck.h>

class TLSInputMatricesFiller;
class TLSResultsMatrices;

/*!
\ingroup LSAlgorithm
\brief Class to run the least square process
*/
class TLSAlgorithm : public TVAbstractAlgorithm
{

public:
	TLSAlgorithm(TLGCData& data);

	/// Run the calculation 
	Behavior run(TLGCData& data, int fMaxIterations) override;

	/// Return number of iterations made
	int		getNumberOfIterations() { return fNumberOfIterations; }

	/// Random numbers generator
	std::ranlux48 engine;

protected:
	/// Pointer to the result matrix extractor
	std::shared_ptr<TLSResultsMatricesExtractor> fExtractor;

	/// Number of equations
	int		 fNumberOfIterations;

	/// Flag if S a posteriori variance exists
	bool	 fS0APosterioriVariances;


	// for FRAS stress test
	void manipulate_ECWS_measurements(TLGCData* data);

	/// Iterate to the solution
	Behavior iterate2Solution(TLGCData& data,
								TLSInputMatricesFiller* matrFiller,
								TLSInputMatrices* inputMtr,
								TALSComputer* computer,
								int fMaxIterations,
								TReal convCrit);

	/// Compute the covariance matrix, the residual vector and the reliabilities
	bool computeVarCovarAndReliability(TLGCData* data, TLSInputMatrices* inputMtr, TALSComputer* computer);

private:

	/// Allow transformations between the usual reference frame
	TPointTransformer fPointTransformer;

};

#endif
