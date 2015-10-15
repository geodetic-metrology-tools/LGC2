// TWTWHFilter.h
//
// Concrete Class of the TWTWFilter PABC.
// Handles the IO of an TWireToWireDistMeas object for the case
// where there is only a horizontal distance
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

#include "TWTWHFilter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TWTWHFilter)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
TWTWHFilter *TWTWHFilter::fFilter = 0;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TWTWHFilter::TWTWHFilter()
{}

TWTWHFilter::~TWTWHFilter()
{}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////
// returns the single instance of this class
////////////////////////////////////////////
TWTWHFilter *TWTWHFilter::instance()
{
	if( fFilter == 0 )
	{
		fFilter = new TWTWHFilter();
	}

	return fFilter;
}


////////////////////////////////////////////////
// inputs a TWTWMeasurement from the QTextStream
////////////////////////////////////////////////
void TWTWHFilter::input(TLGCTStream &iStream, TWireToWireDistMeas &WTW) const{
	

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


	//possibly get the horizontal distance
	if (!iStream.atEnd())
		iStream >> obsHDist;

	//possibly read the horizontal distance's precision
	if (!iStream.atEnd()){
		iStream >> hSigma;
		hSigma *= LITERAL(0.001);
	}

	TWireToWireDistMeas* newWTW = new TWireToWireDistMeas
		(w1End1Name, w1End2Name, w1Pos, w2End1Name, w2End2Name, w2Pos, obsHDist, hSigma, obsVDist, vSigma);
	newWTW->activateHDist();
	newWTW->disactivateVDist();

	WTW = *newWTW;

	delete newWTW;

	return;
}
