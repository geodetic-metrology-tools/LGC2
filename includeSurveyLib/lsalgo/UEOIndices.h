#ifndef SU_UEOINDICES
#define SU_UEOINDICES

#include  "TVNumericValue.h"

//matrix index type
typedef int MatrixIndex;
//matrix element type
typedef TReal MatrixElmt;

/*!
	\ingroup CalcElements
	\brief Structure holding the 3 indices used in LS process: unknown, equation and observation index.
*/
struct UEOIndices{
	MatrixIndex UIndex; // number of unknowns
	MatrixIndex EIndex; // number of equations
	MatrixIndex OIndex;	// number of observations
};

#endif
