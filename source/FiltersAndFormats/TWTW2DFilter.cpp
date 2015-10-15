// TWTW2DFilter.h
//
// Concrete Class of the TWTWFilter PABC.
// Handles the IO of an TWireToWireDistMeas object for the case
// where there are both a horizontal and a vertical distance measurements
//
// Patterns:
// This class is a Singleton.
// Works as a Strategy of a TLGCTStream object. 
//
// Copyright 2000, CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// Other forward declarations
#include "TLGCTStream.h"
#include "TWireToWireDistMeas.h"

#include "TWTW2DFilter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TWTW2DFilter)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
TWTW2DFilter *TWTW2DFilter::fFilter = 0;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TWTW2DFilter::TWTW2DFilter()
{}

TWTW2DFilter::~TWTW2DFilter()
{}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////
// returns the single instance of this class
////////////////////////////////////////////
TWTW2DFilter *TWTW2DFilter::instance()
{
	if( fFilter == 0 )
	{
		fFilter = new TWTW2DFilter();
	}

	return fFilter;
}


////////////////////////////////////////////////
// inputs a TWTWMeasurement from the QTextStream
////////////////////////////////////////////////
void TWTW2DFilter::input(TLGCTStream &iStream, TWireToWireDistMeas &WTW) const{
	

	TSpatialPointName w1End1Name, w1End2Name, w2End1Name, w2End2Name;
	TLength w1Pos(LITERAL(0.0)), w2Pos(LITERAL(0.0)), obsHDist(LITERAL(0.0)), hSigma(LITERAL(0.0)), obsVDist(LITERAL(0.0)), vSigma(LITERAL(0.0));
	
	//get the first wire's first end point's name
	iStream >> w1End1Name;

	//get the first wire's second end point's name
	iStream >> w1End2Name;

	//get the measurement's position along the first wire (from first end on)
	iStream >> w1Pos;

	//get the second wire's first end point's name
	iStream >> w2End1Name;

	//get the second wire's second end point's name
	iStream >> w2End2Name;

	//get the measurement's position along the second wire (from first end on)
	iStream >> w2Pos;

	//get the horizontal distance
	iStream >> obsHDist;

	//get the horizontal distance's precision
	iStream >> hSigma;
	hSigma *= LITERAL(0.001);

	//possobly read the vertical distance
	if (!iStream.atEnd()){
		iStream >> obsVDist;
	}

	//possibly read the vertical distance's precision
	if (!iStream.atEnd()){
		iStream >> vSigma;
		vSigma *= LITERAL(0.001);
	}

	TWireToWireDistMeas* newWTW = new TWireToWireDistMeas
		(w1End1Name, w1End2Name, w1Pos, w2End1Name, w2End2Name, w2Pos, obsHDist, hSigma, obsVDist, vSigma);
	newWTW->activateHDist();
	newWTW->activateVDist();

	WTW = *newWTW;

	delete newWTW;

	return;
}
