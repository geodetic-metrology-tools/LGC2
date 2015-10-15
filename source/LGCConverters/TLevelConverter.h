////////////////////////////////////////////////////////////////////
// TLevelConverter
/*!
Write the TSTN instrument for an LGC output file

*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_LEVEL_CONVERTER
#define SU_LEVEL_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "Measurements/TLevel.h"
#include "TObservationConverter.h"
////////////////////////////////////////////////////////////////

//Class definition
class  TLevelConverter : public TObservationConverter 
{
public:
	//! Constructor
	TLevelConverter(TAStreamFormatter& stream);
	//!Destructor
	virtual ~TLevelConverter();

	void	writeReliabilityData(const TLEVEL& fLevel);
	
	void	writeReliabilityHeader();
	
};

#endif //SU_LEVEL_CONVERTER