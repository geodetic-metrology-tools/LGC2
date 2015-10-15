////////////////////////////////////////////////////////////////////
// TOffsetToTheoPlaneConverter
/*!
Write Offset to a Theodolite Plane Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_ECTH_CONVERTER
#define SU_ECTH_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLengthObsConverter.h"
#include "TOffsetToTheoPlaneMeasurement.h"
#include "TLSCalcOffsetToTheoPlaneObservation.h"
#include "Scale.h"
#include "TTheodolite.h"
#include "TAngle.h"
#include "TLength.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TOffsetToTheoPlaneConverter : public TLengthObsConverter  
{

	
public:

	TOffsetToTheoPlaneConverter(TAStreamFormatter& stream);

	TOffsetToTheoPlaneMeasurement* readObservation(TLGCProject* project, const Scale* currentScale, TAngle* angle, int& i);


	void	writeResults(LSOffsetToTheoPlaneConstIter	obsIt);

	void	writeResultsHeader(bool hasOwnV0);

	void	writeReliabilityData(TLSCalcOffsetToTheoPlaneObservation& obs);
	void	writeReliabilityHeader();

	void	writeMesData(LSOffsetToTheoPlaneConstIter	obsIt);
};

/*@}*/

#endif // SU_ECTH_CONVERTER
