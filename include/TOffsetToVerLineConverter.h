////////////////////////////////////////////////////////////////////
// TOffsetToVerLineConverter
/*!
Write Offset to a Vertival Lone Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_ECVE_CONVERTER
#define SU_ECVE_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLengthObsConverter.h"
#include "TLSCalcOffsetToVerLineObservation.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TOffsetToVerLineConverter : public TLengthObsConverter  
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TOffsetToVerLineConverter(TAStreamFormatter& stream);

			//!Destructor
			virtual  ~TOffsetToVerLineConverter();
		//@}

			void	writeResults(LSOffsetToVerLineConstIter	obsIt);

			void	writeResultsHeader(const int nbrObs);
	
			void	writeReliabilityData(TLSCalcOffsetToVerLineObservation& obs);
			void	writeReliabilityHeader();

			void	writeMesData(LSOffsetToVerLineConstIter	obsIt);

			int		readObservation(int i, int& numOfMeas, TLGCProject* project);

private:

			bool	insertOffsetToVerLineMeas(	TDistMeasStation* newDistSt, TOffsetToVerLineMeasurement* newOffset,
												TLGCProject* project, int i);

			TOffsetToVerLineConverter();
};

/*@}*/

#endif // SU_ECVE_CONVERTER