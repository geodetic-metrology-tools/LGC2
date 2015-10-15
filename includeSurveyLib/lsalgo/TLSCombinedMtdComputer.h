#ifndef SU_TLS_COMB_MTD_COMPUTER
#define SU_TLS_COMB_MTD_COMPUTER

#include "TALSComputer.h"

class TLSInputMatrices;
class TLSResultsMatrices;

/*!
	\ingroup CalcElements

	\brief Least squares computer used when the obseved values can not be separated from the unknowns of the model and therefore both first and second design matrices are adopted.
*/
class TLSCombinedMtdComputer:public TALSComputer
{

public:

	//!Constructor
	TLSCombinedMtdComputer();
	//!Destructor
	virtual ~TLSCombinedMtdComputer();

	//!Verify the number of unknowns and run the calculation
	virtual bool computeResults(TLSInputMatrices*, TLSResultsMatrices*);

	//!Computes the results matrices for the combined case
	virtual bool computeResultsMtrs(TLSInputMatrices*, TLSResultsMatrices*);

	//!Computes the results matrices for a free calculation
	//bool computeFreeResultsMtrs(TLSInputMatrices*, TLSResultsMatrices*);
	
	//!Computes the residual vector and the varaiance covariance matrices
	virtual void calcResiduAndVarCovMatrice(const TLSInputMatrices* inputMtr,TLSResultsMatrices* rm);

private:
	int					count;
};

#endif

