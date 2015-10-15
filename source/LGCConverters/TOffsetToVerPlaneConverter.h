////////////////////////////////////////////////////////////////////
// TOffsetToVerPlaneConverter
/*!
Write Offset to a Vertical Plane Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_ECHO_CONVERTER
#define SU_ECHO_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLengthObsConverter.h"
#include "TLSCalcOffsetToVerPlaneObservation.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TOffsetToVerPlaneConverter : public TLengthObsConverter  
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TOffsetToVerPlaneConverter(TAStreamFormatter& stream);

			//!Destructor
			virtual  ~TOffsetToVerPlaneConverter();
		//@}

			void	writeResults(LSOffsetToVerPlaneConstIter	obsIt);

			void	writeResultsHeader(const int nbrObs);

			void	writeReliabilityData(TLSCalcOffsetToVerPlaneObservation& obs);
			void	writeReliabilityHeader();

			void	writeMesData(LSOffsetToVerPlaneConstIter	obsIt);

			int		readObservation(int i, int &numOfMeas, TLGCProject* project);

private:

			bool	insertOffsetToVerPlaneMeas(	TDistMeasStation* newDistSt, TOffsetToVerPlaneMeasurement* newOffset,
												TLGCProject* project, int i);

			TOffsetToVerPlaneConverter();
};

/*@}*/

#endif // SU_ECHO_CONVERTER