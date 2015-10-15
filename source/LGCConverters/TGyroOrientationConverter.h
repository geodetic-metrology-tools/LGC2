////////////////////////////////////////////////////////////////////
// TGyroOrientationConverter
/*!
Write Gyro Orientation Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_GYRO_CONVERTER
#define SU_GYRO_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations

#include "TAngleObsConverter.h"
#include "TLSCalcGyroOrientationObservation.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TGyroOrientationConverter : public TAngleObsConverter  
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TGyroOrientationConverter(TAStreamFormatter& stream);

			//!Destructor
			virtual  ~TGyroOrientationConverter();
		//@}

			void	writeResults(LSGyroOrieConstIter	obsIt);

			void	writeResultsHeader(const int nbrObs);

			void	writeMesData(LSGyroOrieConstIter	obsIt);

			int		readObservation(int i, int& numOfMeas, TLGCProject* project);

private:

			bool	insertGyroOrieMeas(TTheodoliteStation* newThSt,
									   THorAngleMeasurement* newOrie,
									   TLGCProject* project,
									   int i );

			TGyroOrientationConverter();
};

/*@}*/

#endif // SU_GYRO_CONVERTER