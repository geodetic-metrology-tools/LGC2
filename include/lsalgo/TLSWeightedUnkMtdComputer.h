#ifndef SU_TLS_WEIGHTED_UNK_MTD_COMPUTER
#define SU_TLS_WEIGHTED_UNK_MTD_COMPUTER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
#include "UEOIndices.h"
#include "lsalgo\TALSComputer.h"
#include <string>
class TLSInputMatrices;
class TLSResultsMatrices;

//! Class for a least squares computer used when there are only observation equations
class TLSWeightedUnkMtdComputer:public TALSComputer
{

public:

	//!Constructor
	TLSWeightedUnkMtdComputer();
	//!Destructor
	virtual ~TLSWeightedUnkMtdComputer();

	//!Verify the number of unknowns and run the calculation
	virtual bool computeResults(TLSInputMatrices*, TLSResultsMatrices*);

	//!Computes the results matrices with the weihted unknowns
	virtual bool computeResultsMtrs(TLSInputMatrices*, TLSResultsMatrices*);

	//!Computes the results matrices for a free calculation
	//bool computeFreeResultsMtrs(TLSInputMatrices*, TLSResultsMatrices*);

	//!Computes the residual vector and the varaiance covariance matrices
	virtual void calcResiduAndVarCovMatrice(const TLSInputMatrices* inputMtr,TLSResultsMatrices* rm);

private:
	int		count;
};

#endif

