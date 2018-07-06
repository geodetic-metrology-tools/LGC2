/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_ALGORITHM
#define TLS_ALGORITHM


//SURVEYLIB
#include <TALSComputer.h>
#include <Behavior.h>
//LGC
#include <TVAbractAlgorithm.h>
#include <TLSResultsMatricesExtractor.h>
#include <TLibrCnstrGenerator.h>

class TLSInputMatricesFiller;
class TLSResultsMatrices;

/*!
\ingroup LSAlgorithm
\brief Class to run the least square process
*/
class TLSAlgorithm : public TVAbractAlgorithm
{

public:
	TLSAlgorithm(TLGCData& data);

	/// Run the calculation
	virtual Behavior run(TLGCData& data, int fMaxIterations);

	/// Iterate to the solution
	virtual Behavior	iterate2Solution(TLGCData& data,
		TLSInputMatricesFiller* matrFiller,
		TLSInputMatrices* inputMtr,
		TALSComputer* computer,
		int fMaxIterations,
		TReal convCrit);

	/// Return number of iterations made
	int		getNumberOfIterations() { return fNumberOfIterations; }


protected:
	/// Pointer to the result matrix extractor
	std::shared_ptr<TLSResultsMatricesExtractor> extractor;

	/// Number of equations
	int		 fNumberOfIterations;

	/// Flag if S a posteriori variance exists
	bool	 fS0APosterioriVariances;

	/// Compute the covariance matrix,the residual vector and the reliabilities
	void	 computeVarCovarAndReliability(TLGCData* data,
		TLSInputMatrices* inputMtr,
		TALSComputer* computer);

private:
	/// Contribution generator for the free network
	TLibrCnstrGenerator fLibrCnstrGenerator;

	/// Allow transformations between the usual reference frame
	TPointTransformer fPointTransformer;

};

#endif
