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
#include "TLSCalcVertDistObservation.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TVertDistConverter : public TLengthObsConverter  
{

	
public:
			enum	VertDistMode {kDVER, kDLEV};

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TVertDistConverter(TAStreamFormatter& stream);

			//!Destructor
			virtual  ~TVertDistConverter();
		//@}

			void	writeResults(LSVertDistConstIter	obsIt);

			void	writeResultsHeader(const int nbrObs);

			void	writeReliabilityData(TLSCalcVertDistObservation& obs);
			void	writeReliabilityHeader();

			void	writeMesData(LSVertDistConstIter	obsIt);

			int		readObservation(int i, int& numOfMeas, TLGCProject* project, VertDistMode mode);

private:
			void	readVerticalDistStation(TVerticalDistROM& levelROM);

			bool	insertVertDistMeas( TVerticalDistROM* newVDRom, TVerticalDistMeasurement* newVDist,
										TLGCProject* project, VertDistMode mode, int i );

			TVertDistConverter();
};

/*@}*/

#endif // SU_TVERT_DIST_CONVERTER