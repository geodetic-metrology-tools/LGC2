////////////////////////////////////////////////////////////////////
// TAngleObsConverter
/*!
Base Class for converters writing Angle type Observations for an output LGC file

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_ANG_CONVERTER
#define SU_ANG_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
class	TLGCProject;
class	TTheodoliteStation;
class	TAngleConstants;

#include "TObservationConverter.h"
//#include "TALSCalcAngleObservation.h"
#include "Measurements/MeasDef.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TAngleObsConverter : public TObservationConverter  
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TAngleObsConverter(	TAStreamFormatter& stream);

			//!Destructor
			virtual  ~TAngleObsConverter();
		//@}

			//void	writeReliabilityData(TALSCalcAngleObservation& absAng);
			void	writeReliabilityData(const TAMeas<TInstrumentData::TPOLAR::TTarget,ENoValues, 0,ESingleValue, 1>& fAngle);
	
			void	writeReliabilityHeader();


protected :

			void	formatMesLine(	const long int id,
									const string namept1,
									const string namept2,
									const string namept3,
									const TAngle obs,
									const TAngle calc,
									const TAngle res,
									const TAngle sig,
									const TAngle v0,
									const string com);


			TAStreamFormatter&	readMeasurement(TSpatialPointName &tgName,
												TAngle &obsAngle, TAngle &sigma);

			bool	readColSigma(TReal& colSigma, int &i, TLGCProject* project);

			TAStreamFormatter&	readTheoStation( TTheodoliteStation& thStation);

			void	setConstant(TAngleConstants cte, TTheodoliteStation* newThSt, TLGCProject* project);

			void	readConstant( TAngleConstants &cte );

private:

			TAngleObsConverter();
};

/*@}*/

#endif // SU_ANG_CONVERTER