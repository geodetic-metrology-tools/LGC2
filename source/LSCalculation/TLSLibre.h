#ifndef TLS_LIBR
#define TLS_LIBR

#include "TLSAlgorithm.h"
#include "TPointTransformer.h"
#include "TLibrCnstrGenerator.h"


/*!
\ingroup LibrCalculation

\brief Class responsible for running the calculation in the LIBR mode.
*/
class TLSLibre : public TLSAlgorithm
{
public:

	/// Constructor
	TLSLibre(TLGCData& data, int maxIter);

	///run the calculation
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

private: 
	TLibrCnstrGenerator fLibrCnstrGenerator;
	TPointTransformer fPointTransformer;
};
#endif