////////////////////////////////////////////////////////////////////
// TOffsetToSpaLineConverter
/*!
Write Offset to a SPatial Line Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_ECSP_CONVERTER
#define SU_ECSP_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLengthObsConverter.h"
#include "TLSCalcOffsetToSpaLineObservation.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TOffsetToSpaLineConverter : public TLengthObsConverter  
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TOffsetToSpaLineConverter(TAStreamFormatter& stream);

			//!Destructor
			virtual  ~TOffsetToSpaLineConverter();
		//@}

			void	writeResults(LSOffsetToSpaLineConstIter	obsIt);

			void	writeResultsHeader(const int nbrObs);
	
			void	writeReliabilityData(TLSCalcOffsetToSpaLineObservation& obs);
			void	writeReliabilityHeader();

			void	writeMesData(LSOffsetToSpaLineConstIter	obsIt);

			int		readObservation(int i, int &numOfMeas, TLGCProject* project);

private:

			bool	insertOffsetToSpaLineMeas(	TDistMeasStation* newDistSt, TOffsetToSpaLineMeasurement* newOffset,
												TLGCProject* project, int i);

			TOffsetToSpaLineConverter();
};

/*@}*/

#endif // SU_ECSP_CONVERTER