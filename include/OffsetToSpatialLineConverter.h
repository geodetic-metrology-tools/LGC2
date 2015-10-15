////////////////////////////////////////////////////////////////////
// OffsetToSpatialLineConverter
/*!

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_OFFSET_SPATIAL_LINE_CONVERTER
#define SU_OFFSET_SPATIAL_LINE_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLengthObsConverter.h"
#include "TLSCalcOffsetToSpatialLineObservation.h"

class OffsetToSpatialLineROM;

//Class definition
class  OffsetToSpatialLineConverter : public TLengthObsConverter  
{
public:

	OffsetToSpatialLineConverter(TAStreamFormatter& stream);

	OffsetToSpatialLineROM* readObservations(int& lineNumber, TLGCProject* project);
	
	void	writeResults(LSOffsetToSpaLineConstIter	obsIt);
	void	writeResultsHeader();
	void	writeInformation(const LSPosVecIter& ptOnLine, const LSFreeVecIter& unit);
	void	writeReliabilityData(TLSCalcOffsetToVerticalLineObservation& obs);
	void	writeReliabilityHeader();
	void	writeMesData(LSOffsetToSpaLineConstIter obsIt);
	
};

#endif // SU_OFFSET_SPATIAL_LINE_CONVERTER
