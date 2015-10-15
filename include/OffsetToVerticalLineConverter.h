////////////////////////////////////////////////////////////////////
// OffsetToVerticalLineConverter
/*!

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_OFFSET_VERTICAL_LINE_CONVERTER
#define SU_OFFSET_VERTICAL_LINE_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLengthObsConverter.h"
#include "OffsetToVerticalLineROM.h"
#include "TLSCalcOffsetToVerticalLineObservation.h"

//Class definition
class  OffsetToVerticalLineConverter : public TLengthObsConverter  
{

	
public:

	OffsetToVerticalLineConverter(TAStreamFormatter& stream);

	OffsetToVerticalLineROM* readObservations(int& lineNumber, TLGCProject* project);
	
	void	writeResults(LSOffsetToVerLineConstIter	obsIt);
	void	writeResultsHeader();
	void	writeInformation(const LSPosVecIter& posVec);
	void	writeReliabilityData(TLSCalcOffsetToVerticalLineObservation& obs);
	void	writeReliabilityHeader();
	void	writeMesData(LSOffsetToVerLineConstIter obsIt);
	
};

#endif // SU_OFFSET_VERTICAL_LINE_CONVERTER
