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
#include "TLSCalcOffsetToTheoPlaneObservation.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TOffsetToTheoPlaneConverter : public TLengthObsConverter  
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TOffsetToTheoPlaneConverter(TAStreamFormatter& stream);

			//!Destructor
			virtual  ~TOffsetToTheoPlaneConverter();
		//@}

			void	writeResults(LSOffsetToTheoPlaneConstIter	obsIt);

			void	writeResultsHeader(const int nbrObs);

			void	writeReliabilityData(TLSCalcOffsetToTheoPlaneObservation& obs);
			void	writeReliabilityHeader();

			void	writeMesData(LSOffsetToTheoPlaneConstIter	obsIt);

			int		readObservation(int i, int &numOfMeas, TLGCProject* project);

private:

			bool	insertOffsetToTheoPlaneMeas(TDistMeasStation* newDistSt, TOffsetToTheoPlaneMeasurement* newOffset,
												TLGCProject* project, int i);

			TOffsetToTheoPlaneConverter();
};

/*@}*/

#endif // SU_ECTH_CONVERTER