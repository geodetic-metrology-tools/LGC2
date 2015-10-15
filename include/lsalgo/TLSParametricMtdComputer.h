#ifndef SU_TLS_PARAM_MTD_COMPUTER
#define SU_TLS_PARAM_MTD_COMPUTER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
#include "UEOIndices.h"
#include <string>
#include "lsalgo\TALSComputer.h"
class TLSInputMatrices;
class TLSResultsMatrices;

//! Class for a least squares computer used when there are only observation equations
class TLSParametricMtdComputer:public TALSComputer
{

public:

	//!Constructor
	TLSParametricMtdComputer();
	//!Destructor
	virtual ~TLSParametricMtdComputer();

	//!Verify the number of unknowns and run the calculation
	virtual bool computeResults(TLSInputMatrices*, TLSResultsMatrices*);

	//!Computes the results matrices for the parametric case
	virtual bool computeResultsMtrs(TLSInputMatrices*, TLSResultsMatrices*);

	//!Computes the results matrices for a free calculation
	//bool computeFreeResultsMtrs(TLSInputMatrices*, TLSResultsMatrices*);

	//!Computes the residual vector and the varaiance covariance matrices
	virtual void calcResiduAndVarCovMatrice(const TLSInputMatrices* inputMtr,TLSResultsMatrices* rm	);

private:
	int				count;
};

#endif

