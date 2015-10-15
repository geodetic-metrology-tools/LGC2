////////////////////////////////////////////////////////////////////
// TLengthObsConverter
/*!
Base Class for converters writing Length type Observations for an output LGC file

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_LENGTH_CONVERTER
#define SU_LENGTH_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
class TDistConstants;

#include "TObservationConverter.h"
#include "TALSCalcLengthObservation.h"
// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TLengthObsConverter : public TObservationConverter  
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TLengthObsConverter(TAStreamFormatter& stream);

			//!Destructor
			virtual  ~TLengthObsConverter();
		//@}


protected :

		void	writeReliabilityData(	TALSCalcLengthObservation& obs,
										string name);

		void	formatMesLine(	const long int id,
								const string namept1,
								const string namept2,
								const string namept3,
								const TAngle gis,
								const TLength obs,
								const TLength calc,
								const TLength res,
								const TLength sig,
								const TAngle v0,
								const string com);

	
		void	readMeasurement(TLength &obsDist);
	
		void	readSigma(TLength &sigma, TLength &sigmaPpm);
	
		void	readSigma(TLength &sigma);

		bool	readColSigma(TReal& colSigma, TReal& colSigmaPpm, int &i);

		bool	readColSigma(TReal& colSigma, int &i);

		void	addConstant2List(TDistConstants &cte, TLGCProject* project);

		void	readConstant( TDistConstants &cte );

private:

			TLengthObsConverter();
};

/*@}*/

#endif // SU_LENGTH_CONVERTER
