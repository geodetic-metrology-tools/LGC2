#ifndef SU_TLS_CNST_MTD_COMPUTER
#define SU_TLS_CNST_MTD_COMPUTER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
#include "UEOIndices.h"
#include "TALSComputer.h"
#include <string>
class TLSInputMatrices;
class TLSResultsMatrices;

/*!
	\ingroup CalcElements

	\brief Class for a least squares computer, used when there are any constraints in the model.
*/
class TLSCnstMtdComputer:public TALSComputer
{

public:

	//!Constructor
	TLSCnstMtdComputer();
	//!Destructor
	virtual ~TLSCnstMtdComputer();

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