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
#include "lsalgo/TLSResultsMatrices.h"
#include "lsalgo/TLSInputMatrices.h"


struct limits{double s0PostUpLimit; double s0PostLoLimit;};

//! Abstract Base Class. Defines the methods common to all least squares computers
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

	//!Computes the statistic vector
	virtual void calcStatisticVector(TReal alpha, TReal beta, const TLSInputMatrices* inputMtr, TLSResultsMatrices* rm);

	//!Calculate the hypothesis testing limits for the sigma zero a posteriori
	struct limits	calcSigmaZeroLimits(const int nbObs, const int nbUnk, const double sigmaZero2);
	virtual void    calcOverall(TVector* Z);
	
	/*! Access to eventual error */
	std::string		getError() const { return fError; }

	/*! Access to statistic vectors */
	TVector*		getZi(){return fZ;}
	TVector*		getWi(){return fW;} 
	TVector*		getTi(){return fT;} 
	TVector*		getDelty(){return fDelty;}
	TVector*		getNabla(){return fNablaValue;}
	TVector*		getGi(){return fGValue;}
	TReal			getOVERALL(){return fOverall;}

	bool		getAreDetermined(){return fAreDetermined;} 
	bool		getWToCompute(){return fWToCompute; }
	bool		getGToCompute(){return fGToCompute;}
	bool		getDeltaComputed(){return fDeltaComputed;}

	void clearVectors();
	void initialiseStatVector(const TLSInputMatrices* im);

protected:
	///Constructor
	TALSComputer();
	TALSComputer(const TALSComputer& source);
	

	std::string			fError;		/*!< errors during calculation */
	bool fS0APosterioriVariances;

	TVector*		fZ; /*!< local reliability statistic */
	TVector*		fW; /*!< Gross-error detection statistic */
	TVector*		fT; /*!< Level of ease a gross error of size NABLA can be detected */
	TVector*		fDelty; /*!< Maximum effect of an undetected gross-error */
	TVector*		fNablaValue; /*!< Value for greatest undetected error */
	TVector*		fGValue; /*!< Value for estimation of the gross-error made with the statistic wi */
	TReal			fOverall; /*!< global network reliability */

	bool		fAreDetermined; /*!< indicates if the error detection params can be computed */
	bool		fWToCompute; /*!< Indicates if the w param is to compute: false if simulation */
	bool		fGToCompute; /*!< indicates if G needs to be computed */
	bool		fDeltaComputed; /*!< indicates that the parameters for statistic tests have been rightly chosen */

	TReal alpha;
	TReal beta;
	
};

#endif

