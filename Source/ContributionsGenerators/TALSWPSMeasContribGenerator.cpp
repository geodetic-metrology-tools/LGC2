// TALSWPSMeasContribGenerator.cpp
//
/** Abstract Base Class for a LS contrib generator processing WPS measurements */
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
#include  "TALSWPSMeasContribGenerator.h"
////////////////////////////////////////////////////////////////


//ClassImp(TALSWPSMeasContribGenerator)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////
// Default constructor
//////////////////////
TALSWPSMeasContribGenerator::TALSWPSMeasContribGenerator()
{	// default constructor
	fInputMatrices = 0;
	fCurrentStation = 0;
	fS0APrioriScaleFactor = 1;
}

////////////////////////////////////////////////////////
// constructor setting the pointer to the input matrices
////////////////////////////////////////////////////////
TALSWPSMeasContribGenerator::TALSWPSMeasContribGenerator(TLSInputMatrices* im){

	fInputMatrices = im;
	fS0APrioriScaleFactor = fInputMatrices->getS0APrioriScaleFactor();
	fCurrentStation = 0;
}


// **This needs to be implemented**
TALSWPSMeasContribGenerator::TALSWPSMeasContribGenerator( const  TALSWPSMeasContribGenerator& original )
{	// copy constructor
	fInputMatrices = original.fInputMatrices;
	fS0APrioriScaleFactor = original.fS0APrioriScaleFactor;
	fCurrentStation = original.fCurrentStation;
}


TALSWPSMeasContribGenerator::~TALSWPSMeasContribGenerator()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

// **This needs to be implemented**
TALSWPSMeasContribGenerator&  TALSWPSMeasContribGenerator::operator=(const TALSWPSMeasContribGenerator& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		fInputMatrices = right.fInputMatrices;
		fS0APrioriScaleFactor = right.fS0APrioriScaleFactor;
		fCurrentStation = right.fCurrentStation;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////
// sets the pointer to the current WPS station
////////////////////////////////////////////////
void TALSWPSMeasContribGenerator::setCurrentWPSStation(TLSCalcWorkingStations::CalcWPSStIterator wpsSt){

	fCurrentStation = wpsSt;
}
