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

	virtual bool run(TLGCData& data, int fMaxIterations);

	/// Iterates to the solution
	bool	iterate2Solution(TLGCData& data,
		TLSInputMatricesFiller* matrFiller,
		TLSInputMatrices* inputMtr,
		TALSComputer* computer,
		TLSResultsMatricesExtractor* extractor,
		TLSResultsMatrices* resultMatrices,
		int fMaxIterations,
		TReal convCrit);

	/// Returns number of iterations made
	int		getNumberOfIterations() { return fNumberOfIterations; }

private:
	int		 fNumberOfIterations;
	bool	 fS0APosterioriVariances;

	void	 computeVarCovarAndReliability(TLGCData* data,
		TLSInputMatrices* inputMtr,
		TALSComputer* computer,
		TLSResultsMatricesExtractor* extractor,
		TLSResultsMatrices* resultMatrices);

};

#endif