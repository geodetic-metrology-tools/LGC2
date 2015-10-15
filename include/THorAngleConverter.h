////////////////////////////////////////////////////////////////////
// THorAngleConverter
/*!
Write Horizontal Angle Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_THORANG_CONVERTER
#define SU_THORANG_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations

#include "TAngleObsConverter.h"
#include "TLSCalcHorAngleObservation.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup ProjectFramework

@{*/


//Class definition
class  THorAngleConverter : public TAngleObsConverter  
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			THorAngleConverter(TAStreamFormatter& stream);

			//!Destructor
			virtual  ~THorAngleConverter();
		//@}

			void	writeResults(LSHorAngConstIter	obsIt);

			void	writeResultsHeader(const int nbrObs);

			void	writeMesData(LSHorAngConstIter	obsIt);

			int		readObservation(int i, int &numOfMeas, TLGCProject* projet);


private:

			bool insertHorAngleMeas(TTheodoliteStation* newThSt,
									THorAngleMeasurement* newHAng,
									TLGCProject* project,
									int i,
									bool firstStation);



			THorAngleConverter();
};

/*@}*/

#endif // SU_THORANG_CONVERTER