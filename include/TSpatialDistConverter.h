////////////////////////////////////////////////////////////////////
// TSpatialDistConverter
/*!
Write Spatial Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_TSPA_DIST_CONVERTER
#define SU_TSPA_DIST_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLengthObsConverter.h"
#include "TLSCalcSpatialDistObservation.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TSpatialDistConverter : public TLengthObsConverter  
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TSpatialDistConverter(	TAStreamFormatter& stream,
									const TALSCalcParameter::ELSStatus status);

			//!Destructor
			virtual  ~TSpatialDistConverter();
		//@}

			void	writeResults(LSSpaDistConstIter obsIt);

			void	writeResultsHeader(const int nbrObs);

			void	writeReliabilityData(TLSCalcSpatialDistObservation& obs);
			void	writeReliabilityHeader();

			void	writeMesData(LSSpaDistConstIter obsIt);

			int	readObservation(TAMeasurement::ECalcStatus status, int i,
								int& numOfMeas, TLGCProject* project);


private:

			bool	insertSpatialDistMeas(	TDistMeasStation* newDistSt, TSpatialDistMeasurement* newSDist,
											TLGCProject* project,int i );
			TSpatialDistConverter();

			TALSCalcParameter::ELSStatus	fStatus;
};

/*@}*/

#endif // SU_TSPA_DIST_CONVERTER