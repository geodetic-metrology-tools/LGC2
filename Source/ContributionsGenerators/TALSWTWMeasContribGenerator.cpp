// TALSWTWMeasContribGenerator.cpp
//
/** Abstract Base Class for a LS contrib generator processing wire to wire distance measurements */
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include "TLSInputMatrices.h"
#include  "TALSWTWMeasContribGenerator.h"
////////////////////////////////////////////////////////////////


//ClassImp(TALSWTWMeasContribGenerator)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////
// Default constructor
//////////////////////
TALSWTWMeasContribGenerator::TALSWTWMeasContribGenerator()
{	// default constructor
	fInputMatrices = 0;
	fCurrentStation = 0;
	fS0APrioriScaleFactor = 1;
}

////////////////////////////////////////////////////////
// constructor setting the pointer to the input matrices
////////////////////////////////////////////////////////
TALSWTWMeasContribGenerator::TALSWTWMeasContribGenerator(TLSInputMatrices* im){

	fInputMatrices = im;
	fS0APrioriScaleFactor = fInputMatrices->getS0APrioriScaleFactor();
	fCurrentStation = 0;
}


// **This needs to be implemented**
TALSWTWMeasContribGenerator::TALSWTWMeasContribGenerator( const  TALSWTWMeasContribGenerator& original )
{	// copy constructor
	fInputMatrices = original.fInputMatrices;
	fS0APrioriScaleFactor = original.fS0APrioriScaleFactor;
	fCurrentStation = original.fCurrentStation;
}


TALSWTWMeasContribGenerator::~TALSWTWMeasContribGenerator()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////
// sets the pointer to the current WTW station
////////////////////////////////////////////////
void TALSWTWMeasContribGenerator::setCurrentWTWStation(TLSCalcWorkingStations::CalcWTWStIterator wtwSt){

	fCurrentStation = wtwSt;
}
