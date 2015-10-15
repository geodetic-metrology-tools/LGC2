// TALSWPSMeasContribGenerator.h
//
/** Abstract Base Class for a LS contrib generator processing WPS measurements** */
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_TALSWPSMeasContribGenerator
#define SU_TALSWPSMeasContribGenerator


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <**classname**>
//using namespace std;
//
class TLSInputMatrices;
#include "TLSCalcWorkingStations.h"
#include "TLSCalcWPSStation.h"
#include "TAObsContributionsGenerator.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////



//Class definition
class  TALSWPSMeasContribGenerator : public TAObsContributionsGenerator  
{
public:
	//constants

		/// Destructor
		virtual  ~TALSWPSMeasContribGenerator();

	/**@name Member Functions */
	//@{
		/// Copy Assignment Operator 
		TALSWPSMeasContribGenerator& operator=( const TALSWPSMeasContribGenerator& );

		virtual void			setCurrentWPSStation(TLSCalcWorkingStations::CalcWPSStIterator); 

		virtual void			processWPSMeas(TLSCalcWPSStation::CalcWPSMeasIterator cWpsMeas) = 0;
	//@}

protected:
	/**@name Constructors*/
	//@{
		/// Default Constructor 
		TALSWPSMeasContribGenerator();

		/// Constructor
		TALSWPSMeasContribGenerator(TLSInputMatrices*);

		/// Copy Constructor 
		TALSWPSMeasContribGenerator(const  TALSWPSMeasContribGenerator&);

	//@}

		TLSInputMatrices*								fInputMatrices;

		TLSCalcWorkingStations::CalcWPSStIterator		fCurrentStation;

		double											fS0APrioriScaleFactor;


};


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


#endif // SU_TALSWPSMeasContribGenerator
