////////////////////////////////////////////////////////////////////
// THorDistConverter
/*!
Write Horizontal Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_THOR_DIST_CONVERTER
#define SU_THOR_DIST_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLengthObsConverter.h"
#include "THorizontalDistMeas.h"
#include "TTheodolite.h"
#include "TheodoliteTarget.h"
#include "TLSCalcHorDistObservation.h"

//Class definition
class  THorDistConverter : public TLengthObsConverter  
{

	
public:
	THorDistConverter(TAStreamFormatter& stream);

	THorizontalDistMeas<TheodoliteTarget>* readObservation(TLGCProject* project, const TheodoliteTarget* currentTarget, TTheodolite* theo, int& i);

	void	writeResults(LSHorDistConstIter	obsIt);

	void	writeResultsHeader();

	void	writeReliabilityData(TLSCalcHorDistObservation<TheodoliteTarget>& obs);
	void	writeReliabilityHeader();

	void	writeMesData(LSHorDistConstIter	obsIt);

};

#endif // SU_THOR_DIST_CONVERTER
