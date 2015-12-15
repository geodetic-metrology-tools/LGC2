#ifndef _LGCDEFAULTS_H_
#define _LGCDEFAULTS_H_
/*!
   \file
   \ingroup LGCProject
   Default program constants shared among the classes.
*/

#include <limits>

typedef double TReal;

/*!
 *  \addtogroup ConstantsDefaults
 *  @{ 
 */

/*!@name Default values for LGC2 */
//@{

static bool False = false;

/// Default precision for output files
static const int PREC_DEF_DIGITS = 6;

/// Default fisher alpha quantile for FAUT keyword
static const TReal FAUT_DEF_ALPHA = 1.0;
/// Default fisher beta quantile for FAUT keyword
static const TReal FAUT_DEF_BETA  = 10.0;

/// Allowed characters to separate fields in the input file.
static const char* const INPUT_SEPERATOR_CHARS = " \t";
/// Comment characters for line comments
static const char* const INPUT_COMMENT_CHARS = "%$#";
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
