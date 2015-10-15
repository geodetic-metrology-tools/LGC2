// TALS2DOffMeasContribGenerator.h
//
//! Abstract base class for all LS 2D Offset Measurement contribution generators
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_TALS2DOffMeasContribGenerator
#define SU_TALS2DOffMeasContribGenerator


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <**classname**>
//using namespace std;
//
class  TLSInputMatrices;
#include "TLSCalcWorkingStations.h"
#include "TAObsContributionsGenerator.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////



//Class definition
class  TALS2DOffMeasContribGenerator : public TAObsContributionsGenerator  
{
public:

	/**Calculates the contributions of a 2D offset measurement and puts them in the matrices
	@param iterOM an iterator pointing to the 2D offset measurement to be processed*/
	virtual	void	process2DOffsetMeas(TLSCalc2DOffsetROM::Calc2DOffsetIterator iterOM) = 0;

	///Destructor
	virtual ~TALS2DOffMeasContribGenerator();

	//!Sets the current dist meas station
	/*!@param iterSt an iterator pointing to the station of the next 2D offset roms to be processed*/
	virtual void	setCurrentDistStation(TLSCalcWorkingStations::CalcDistStIterator iterSt);

	//!Sets the current 2D offset rom
	/*!@param iterORom an iterator pointing to the rom of the next 2D offset measurements to be processed*/
	virtual void	setCurrent2DOffsetROM(TLSCalcDistMeasStation::Calc2DOffsetROMIterator iterORom);




protected:

	///Default constructor
	TALS2DOffMeasContribGenerator();
	/**Constructor
	@param im a pointer to the LS input matrices (for the generator to know where the put the contributions)*/
	explicit TALS2DOffMeasContribGenerator(TLSInputMatrices* im);
	///Copy constructor
	TALS2DOffMeasContribGenerator(const TALS2DOffMeasContribGenerator& source);

	TLSInputMatrices *									fInputMatrices;
	TLSCalcWorkingStations::CalcDistStIterator			fCurrentStation;
	TLSCalcDistMeasStation::Calc2DOffsetROMIterator		fCurrentROM;

	double												fS0APrioriScaleFactor;

	
};

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////
#endif // SU_TALS2DOffMeasContribGenerator
