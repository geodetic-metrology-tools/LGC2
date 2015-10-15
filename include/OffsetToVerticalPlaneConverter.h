////////////////////////////////////////////////////////////////////
// OffsetToVerticalPlaneConverter
/*!

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_OFFSET_LINE_PLANE_CONVERTER
#define SU_OFFSET_LINE_PLANE_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLengthObsConverter.h"
#include "ObservationROM.h"
#include "TLSCalcOffsetToVerticalLineObservation.h"
#include "TLSCalcOffsetToVerPlaneObservation.h"

//Class definition
class  OffsetToVerticalPlaneConverter : public TLengthObsConverter  
{

	
public:

	OffsetToVerticalPlaneConverter(TAStreamFormatter& stream);

	OffsetToLineOrPlaneROM* readObservations(int& lineNumber, TLGCProject* project);
	TOffsetToLineOrPlaneMeasurement* readObservation(TLGCProject* project, const Scale* currentScale, void* unused, int& i);
		
	void	writeResults(LSOffsetToVerPlaneConstIter	obsIt);
	void	writeResultsHeader();
	void	writeInformation(const LSFreeVecIter& posVec);
	void	writeReliabilityData(TLSCalcOffsetToVerPlaneObservation& obs);
	void	writeReliabilityHeader();
	void	writeMesData(LSOffsetToVerPlaneConstIter obsIt);	
};

#endif // SU_OFFSET_LINE_PLANE_CONVERTER
