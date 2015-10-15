////////////////////////////////////////////////////////////////////
// TZenithDistConverter
/*!
Write Zenithal Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_TZENDIST_CONVERTER
#define SU_TZENDIST_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
class	TLGCProject;

#include "TAngleObsConverter.h"
#include "TLSCalcZenithDistObservation.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TZenithDistConverter : public TAngleObsConverter  
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TZenithDistConverter(	TAStreamFormatter& stream,
									const TALSCalcParameter::ELSStatus status);

			//!Destructor
			virtual  ~TZenithDistConverter();
		//@}

			bool	writeResults(LSZenDistConstIter	obsIt);

			void	writeResultsHeader(const int nbrObs);

			void	writeReliabilityData(TLSCalcZenithDistObservation& obs);

			void	writeMesData(LSZenDistConstIter	obsIt);

			int		readObservation(TAMeasurement::ECalcStatus status, 
				int i, int& numOfMeas, TLGCProject* project);

private:

	
			bool	insertZenithDistMeas(	TTheodoliteStation* newThSt,
											TZenithDistMeasurement* newZDist,
											TLGCProject* project,
											int i );
			TZenithDistConverter();

			TALSCalcParameter::ELSStatus		fStatus;
};

/*@}*/

#endif // SU_TZENDIST_CONVERTER