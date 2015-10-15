////////////////////////////////////////////////////////////////////
// TTstnConverter
/*!
Write the TSTN instrument for an LGC output file

*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_TSTN_CONVERTER
#define SU_TSTN_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "Measurements/TEDM.h"
#include "TObservationConverter.h"
////////////////////////////////////////////////////////////////

//Class definition
class  TEDMConverter : public TObservationConverter 
{
public:
	//! Constructor
	TEDMConverter(TAStreamFormatter& stream);
	//!Destructor
	virtual ~TEDMConverter();

	/*!
		Writes all information about EDM, its definition, calculated parameters and observations which includes.
	*/
	void writeResults(const TEDM& fEdm);
	void writeResultsHeader(const int);

	void	writeReliabilityData(const TEDM& fEdm);
	void	writeReliabilityHeader();
	
};

#endif //SU_LEVEL_CONVERTER