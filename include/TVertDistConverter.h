////////////////////////////////////////////////////////////////////
// TVertDistConverter
/*!
Write Vertical Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_TVERT_DIST_CONVERTER
#define SU_TVERT_DIST_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLengthObsConverter.h"
#include "ObservationROM.h"
#include "TVerticalDistMeasurement.h"
#include "Level.h"
#include "TLGCProject.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TVertDistConverter : public TLengthObsConverter  
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TVertDistConverter(TAStreamFormatter& stream);

			//!Destructor
			virtual  ~TVertDistConverter();
		//@}

			void	writeResults(LSVertDistConstIter	obsIt);

			void	writeResultsHeader();

			void	writeReliabilityData(TLSCalcVertDistObservation& obs);
			void	writeReliabilityHeader();

			void	writeMesData(LSVertDistConstIter	obsIt);

			VerticalDistanceROM* readObservations(int& lineNumber, TLGCProject* project);

private:
			TVerticalDistMeasurement* readObservation(const Level* level, const Staff* st1, const Staff* st2, TLGCProject* project, int& i);

};

/*@}*/

#endif // SU_TVERT_DIST_CONVERTER
