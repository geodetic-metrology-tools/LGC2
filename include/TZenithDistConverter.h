////////////////////////////////////////////////////////////////////
// TZenithDistConverter
/*!
Write Zenithal Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_TZENDIST_CONVERTER
#define SU_TZENDIST_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TLGCProject.h"
#include "TAngleObsConverter.h"
#include "TZenithDistMeasurement.h"
#include "TheodoliteTarget.h"


//Class definition
class  TZenithDistConverter : public TAngleObsConverter  
{

public:

	TZenithDistConverter(TAStreamFormatter& stream);

	TZenithDistMeasurement* TZenithDistConverter::readObservation(TLGCProject* project, const TheodoliteTarget* currentTarget, TTheodolite* theo, int& i);

	
	void	writeResults(LSZenDistConstIter	obsIt);
	void	writeResultsHeader(bool hasOwnInstrumentHeight);
	void	writeReliabilityData(TLSCalcZenithDistObservation& obs);
	void	writeMesData(LSZenDistConstIter	obsIt);
};

#endif // SU_TZENDIST_CONVERTER
