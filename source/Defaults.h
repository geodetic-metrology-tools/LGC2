#ifndef _LGCDEFAULTS_H_
#define _LGCDEFAULTS_H_

typedef double TReal;

/*!
 *  \addtogroup Constants
 *  @{
 */

/// Default behavior for SIMU: ignore observations in input file
static const bool SIM_DEF_IGNORE_OBS = false;

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

/*! @} End of Doxygen Groups*/

#endif
