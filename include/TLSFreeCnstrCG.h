//TLSFreeCnstrCG.h : header file
//
/*! Base Class for a LS contrib generator
processing constraints in a free survey network

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////



#ifndef SU_FREE_CNSTR_CC
#define SU_FREE_CNSTR_CG


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
class	TLSInputMatrices;
class	TLSConstraintIdentifier;
#include "LSCalcDataSet.h"
#include "TLSCalcPosVectorParam.h"

////////////////////
// Class declaration
////////////////////
class TLSFreeCnstrCG 
{

public :
	//!Default constructor
	TLSFreeCnstrCG();

	//!Destructor
	virtual ~TLSFreeCnstrCG();

	//!Calculates the contributions of a offset (LIBR) constraints and puts them in the matrices
	/*!\param ptIter an iterator pointing to the point which has a constraint to be processed*/
	virtual	bool	processFreeCnstr(const LSCalcDataSet& lsds, TLSInputMatrices& matrices, const TLSConstraintIdentifier& cnstr);



private :

	TReal		getRxCalcValue(LSPosVecConstIter ptIt, const TLSConstraintIdentifier& cnstr);	

	TReal		getRyCalcValue(LSPosVecConstIter ptIt, const TLSConstraintIdentifier& cnstr);	

	TReal		getRzCalcValue(LSPosVecConstIter ptIt, const TLSConstraintIdentifier& cnstr);	

	TReal		getScaleCalcValue(LSPosVecConstIter ptIt, const TLSConstraintIdentifier& cnstr);




	
};
#endif

