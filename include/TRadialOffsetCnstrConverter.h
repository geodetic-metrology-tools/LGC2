////////////////////////////////////////////////////////////////////
// TRadialOffsetCnstrConverter
/*!
Write Radial Offset Constraints for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_RADI_CONVERTER
#define SU_RADI_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TObservationConverter.h"
#include "TLSCalcRadialOffsetCnstrObservation.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TRadialOffsetCnstrConverter : public TObservationConverter
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TRadialOffsetCnstrConverter(TAStreamFormatter& stream);

			//!Destructor
			virtual  ~TRadialOffsetCnstrConverter();
		//@}

			void	writeResults(LSRadOffCnstrConstIter	obsIt);

			void	writeResultsHeader();

			void	writeReliabilityData(TLSCalcRadialOffsetCnstrObservation& obs);
			void	writeReliabilityHeader();

			void	writeMesData(LSRadOffCnstrConstIter obsIt);

			void	writeRadData(LSRadOffCnstrConstIter obsIt);

			int		readObservation(int i, TLGCProject* project);

private:
			void	formatRadLine(	const long int id,
									const string namept1,
									const string namept2,
									const string namept3, 
									const TAngle bear,
									const TLength res, 
									const TLength sig, 
									const string com);

			void	formatMesLine(	const long int id,
									const string namept1,
									const string namept2,
									const string namept3, 
									const TAngle bear,
									const TLength res, 
									const TLength sig, 
									const string com);

			bool	readColSigma(TReal& colSigma, int &i);
			void	readSigma(TLength &sigma);

			TRadialOffsetCnstrConverter();
};

/*@}*/

#endif // SU_RADI_CONVERTER
