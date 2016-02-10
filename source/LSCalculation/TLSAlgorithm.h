#ifndef TLS_ALGORITHM
#define TLS_ALGORITHM

#include "TALSComputer.h"
#include "TVAbractAlgorithm.h"

class TLGCData;
class TLSInputMatrices;
class TLSInputMatricesFiller;
class TALSComputer;
class TLSResultsMatricesExtractor;
class TLSResultsMatrices;

/*!
\ingroup LSAlgorithm
\brief Class to run the least square process
*/
class TLSAlgorithm : public TVAbractAlgorithm
{

public:
	TLSAlgorithm();

	//run the calculation
	virtual bool run(TLGCData& data, int fMaxIterations);

	/// Iterates to the solution
	virtual bool	iterate2Solution(TLGCData& data,
		TLSInputMatricesFiller* matrFiller,
		TLSInputMatrices* inputMtr,
		TALSComputer* computer,
		TLSResultsMatricesExtractor* extractor,
		TLSResultsMatrices* resultMatrices,
		int fMaxIterations,
		TReal convCrit);

	/// Returns number of iterations made
	int		getNumberOfIterations() { return fNumberOfIterations; }

protected:
	/// number of equations
	int		 fNumberOfIterations;

	/// flag if S a posteriori variance exists
	bool	 fS0APosterioriVariances;

	///compute the covariance matrix,the residual vector and the reliabilities
	void	 computeVarCovarAndReliability(TLGCData* data,
		TLSInputMatrices* inputMtr,
		TALSComputer* computer,
		TLSResultsMatricesExtractor* extractor,
		TLSResultsMatrices* resultMatrices);

};

#endif