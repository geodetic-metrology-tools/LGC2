////////////////////////////////////////////////////////////////////
// THorDistConverter
/*!
Write Horizontal Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_THOR_DIST_CONVERTER
#define SU_THOR_DIST_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLengthObsConverter.h"
#include "TLSCalcHorDistObservation.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  THorDistConverter : public TLengthObsConverter  
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			THorDistConverter(TAStreamFormatter& stream);

			//!Destructor
			virtual  ~THorDistConverter();
		//@}

			void	writeResults(LSHorDistConstIter	obsIt);

			void	writeResultsHeader(const int nbrObs);

			void	writeReliabilityData(TLSCalcHorDistObservation& obs);
			void	writeReliabilityHeader();

			void	writeMesData(LSHorDistConstIter	obsIt);

			int		readObservation(int i, int &numOfMeas, TLGCProject* project);


private:

			bool	insertHorDistMeas(	TDistMeasStation* newDistSt, THorizontalDistMeas* newHDist,
										TLGCProject* project, int i );
			THorDistConverter();
};

/*@}*/

#endif // SU_THOR_DIST_CONVERTER