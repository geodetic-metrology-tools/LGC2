#ifndef SU_TLS_PARAM_MTD_COMPUTER
#define SU_TLS_PARAM_MTD_COMPUTER

#include "UEOIndices.h"
#include <string>
#include "TALSComputer.h"
class TLSInputMatrices;
class TLSResultsMatrices;

/*!
	\ingroup CalcElements

	\brief Least squares computer used when the obseved values can be separated from the unknowns of the model (parametric case), it should not be used if this condition is not fullfilled.
*/
class TLSParametricMtdComputer:public TALSComputer
{

public:

	///Constructor
	TLSParametricMtdComputer();
	///Destructor
	virtual ~TLSParametricMtdComputer();

	///Verify the number of unknowns and run the calculation
	virtual bool computeResults(TLSInputMatrices*, TLSResultsMatrices*);

	///Computes the results matrices for the parametric case
	virtual bool computeResultsMtrs(TLSInputMatrices*, TLSResultsMatrices*);

	///Computes the results matrices for a free calculation
	//bool computeFreeResultsMtrs(TLSInputMatrices*, TLSResultsMatrices*);

	///Computes the residual vector and the varaiance covariance matrices
	virtual void calcResiduAndVarCovMatrice(const TLSInputMatrices* inputMtr,TLSResultsMatrices* rm	);

private:
	int				count;
};

#endif

