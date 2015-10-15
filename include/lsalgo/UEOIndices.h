//
// Definition of the structure holding the 3 indices : 
// unknown index, equation index and observation index
//

#ifndef SU_UEOINDICES
#define SU_UEOINDICES

#include  "TVNumericValue.h"

//matrix index type
typedef int MatrixIndex;
//matrix element type
typedef TReal MatrixElmt;


///structure for unknowns, equations and observations indicies
struct UEOIndices{
	MatrixIndex UIndex; // number of unknowns
	MatrixIndex EIndex; // number of equations
	MatrixIndex OIndex;	// number of observations
};

#endif
