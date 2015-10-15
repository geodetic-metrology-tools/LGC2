// TALSComputer.h 
// abstract base class for a least squares computer 
//

#ifndef SU_VLS_COMPUTER
#define SU_VLS_COMPUTER

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
#include <math.h>
#include <string>
#include <vector>
#include <Eigen/LU>

#include "TSparseMatrix.h"
#include "UEOIndices.h"
#include "TLSResultsMatrices.h"
#include "TLSInputMatrices.h"


struct limits{double s0PostUpLimit; double s0PostLoLimit;};

/*!
	\ingroup CalcElements

	\brief Abstract Base Class. Defines the methods common to all least squares computers.
*/
class TALSComputer{

public:
	//!Destructor
	virtual ~TALSComputer();

	//!Verify the number of unknowns and run the calculation
	virtual bool computeResults(TLSInputMatrices*, TLSResultsMatrices*) = 0;

	//!Computes the results matrices
	virtual bool computeResultsMtrs(TLSInputMatrices*, TLSResultsMatrices*) = 0;

	//!Computes the results matrices for a free calculation
	//bool computeFreeResultsMtrs(TLSInputMatrices*, TLSResultsMatrices*);

	//!Computes the residual vector and the varaiance covariance matrices
	virtual void calcResiduAndVarCovMatrice(const TLSInputMatrices* inputMtr, TLSResultsMatrices* rm) = 0;

	//!Calculate the hypothesis testing limits for the sigma zero a posteriori
	struct limits	calcSigmaZeroLimits(const int nbObs, const int nbUnk, const double sigmaZero2);
	
	/*! Access to eventual error */
	std::string		getError() const { return fError; }

protected:
	///Constructor
	TALSComputer();

	std::string			fError;		/*!< errors during calculation */
	
};

#endif

