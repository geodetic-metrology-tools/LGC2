/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGCDEFAULTS_H_
#define _LGCDEFAULTS_H_


/*!
   \file
   \ingroup LGCProject
   Default program constants shared among the classes.
*/

//STL
#include <limits>
//SURVEYLIB
#include <Quad.h>


/*!
 *  \addtogroup ConstantsDefaults
 *  @{ 
 */

/*!@name Default values for LGC2 */
//@{

/// Default precision for output files
static const int PREC_DEF_DIGITS = 5;

/// Default fisher alpha quantile for FAUT keyword
static const TReal FAUT_DEF_ALPHA = 0.01; //  1 %
/// Default fisher beta quantile for FAUT keyword
static const TReal FAUT_DEF_BETA = 0.1; // 10 %

/// Allowed characters to separate fields in the input file.
static const char* const INPUT_SEPERATOR_CHARS = " \t";
/// Comment characters for line comments
static const char* const INPUT_COMMENT_CHARS = "%$";
/// Deactivate the oject defined in the line in front of which this character appears
static const char* const DEACTIVATION_CHAR = "#";

/// Default maximal number of iterations
static const int MAX_ITERATIONS = 80;

namespace LGC {
	///Conversion factor for the unitless vector (UVEC and UVD)
	static const TReal VECCONV = 0.001;
	static const TReal VECCONVINV = 1000;
}

/// Access coordinate axes or rotations around axes by their index
enum { X, Y, Z, H = 2 };
//@}

/*! @} End of Doxygen Groups*/


/*!
 *  \addtogroup ConstantsDefaults
 *  @{ 
 */
	static const TReal EPSILON = std::numeric_limits<TReal>::min();  //!< Smallest value of a real value, used for fuzzy comparison
	static const TReal nullLimit = 1e-15;  //!< Limit below which a number is to small and treated as zero.

/*! @} End of Doxygen Groups*/

#endif
