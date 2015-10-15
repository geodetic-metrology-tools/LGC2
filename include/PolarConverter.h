////////////////////////////////////////////////////////////////////
// PolarConverter
/*!
Write Horizontal Angle Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_POLAR_CONVERTER
#define SU_POLAR_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "TLGCProject.h"
#include "TObservationConverter.h"
#include "PolarMeasurement.h"
#include "TheodoliteTarget.h"


//Class definition
class PolarConverter : public TObservationConverter  
{	
public:

	PolarConverter(TAStreamFormatter& stream);

	PolarMeasurement* readObservation(TLGCProject* project, const TheodoliteTarget* currentTarget, TTheodolite* theo, int& i);

	void	writeResults(LSPolarIter	obsIt);
	void	writeResultsHeader();
	void	writeMesData(LSPolarIter	obsIt);
	
	void	writeReliabilityData(TLSCalcPolarObservation& obs);
	void	writeReliabilityHeader();

private:

};

#endif // SU_POLAR_CONVERTER
