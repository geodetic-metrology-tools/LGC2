////////////////////////////////////////////////////////////////////
// EDMSpatialDistConverter
/*!
Write Spatial Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_EDM_SPA_DIST_CONVERTER
#define SU_EDM_SPA_DIST_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLGCProject.h"
#include "TLengthObsConverter.h"
#include "ObservationROM.h"
#include "TSpatialDistConverter.h"
#include "EDMTarget.h"
#include "EDM.h"

//Class definition
class  EDMSpatialDistConverter : public TLengthObsConverter
{
	
public:

	EDMSpatialDistConverter(TAStreamFormatter& stream);

	bool readObservations(int& lineNumber, TLGCProject* project);

};

/*@}*/

#endif // SU_EDM_SPA_DIST_CONVERTER
