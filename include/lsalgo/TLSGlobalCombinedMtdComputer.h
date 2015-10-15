#ifndef SU_TLS_GLOB_COMB_MTD_COMPUTER
#define SU_TLS_GLOB_COMB_MTD_COMPUTER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
#include "UEOIndices.h"
#include <string>
class TLSInputMatrices;
class TLSResultsMatrices;
class TLSResultsMatricesExtractor;

//! Class for a least squares computer used when there are only observation equations
class TLSGlobalCombinedMtdComputer//:
//public TALSMtdComputer
{

public:

	//!Constructor
	TLSGlobalCombinedMtdComputer();
	//!Destructor
	~TLSGlobalCombinedMtdComputer();


	bool computeResults(TLSInputMatrices*, TLSResultsMatrices*);

	//!Computes the results matrices
	/*!@param im a pointer to the LS input matrices*/
	bool computeResultsMtrs(TLSInputMatrices*, TLSResultsMatrices*);

	//void calcResiduAndVarCovMatrice(//LSCalcDataSet& calcDS,
	//												const TLSInputMatrices* inputMtr,
	//												TLSResultsMatrices* rm,
	//												TLSResultsMatricesExtractor* /*,
	//												TLSParametricMtdComputer* */);

	//!Computes the results matrices for a free calculation
	/*!\param im a pointer to the LS input matrices*/
	//bool computeFreeResultsMtrs(TLSInputMatrices*, TLSResultsMatrices*);

	/*! Access to eventual error */
	std::string		getError() const { return fError; }

private:

	std::string			fError;		/*!< errors during calculation */
	int					count;
};

#endif

