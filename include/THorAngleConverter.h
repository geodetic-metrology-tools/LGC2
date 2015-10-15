////////////////////////////////////////////////////////////////////
// THorAngleConverter
/*!
Write Horizontal Angle Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_THORANG_CONVERTER
#define SU_THORANG_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "TLGCProject.h"
#include "TAngleObsConverter.h"
#include "THorAngleMeasurement.h"
#include "TheodoliteTarget.h"


//Class definition
class THorAngleConverter : public TAngleObsConverter  
{	
public:

	THorAngleConverter(TAStreamFormatter& stream);

	THorAngleMeasurement* readObservation(TLGCProject* project, const TheodoliteTarget* currentTarget, TTheodolite* theo, int& i);

	
	void	writeResults(LSHorAngConstIter	obsIt);
	void	writeResultsHeader(bool hasOwnV0);
	void	writeMesData(LSHorAngConstIter	obsIt);

private:

};

#endif // SU_THORANG_CONVERTER
