////////////////////////////////////////////////////////////////////
// LevelStationConverter
/*!
Write Vertical Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_LEVEL_STATION_CONVERTER
#define SU_LEVEL_STATION_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLengthObsConverter.h"
#include "LevelStation.h"
#include "TLGCProject.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  LevelStationConverter : public TLengthObsConverter  
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			LevelStationConverter(TAStreamFormatter& stream);

			//@}

			void	writeResults(LSVertDistConstIter	obsIt);

			void	writeResultsHeader();

			void	writeReliabilityData(TLSCalcVertDistObservation& obs);
			void	writeReliabilityHeader();

			void	writeMesData(LSVertDistConstIter	obsIt);

			bool	readObservations(int& lineNumber, TLGCProject* project);

private:
			LevelMeasurement* readObservation(const Level* level, const Staff* staff, TLGCProject* project, int& i);

};

/*@}*/

#endif // SU_LEVEL_STATION_CONVERTER
