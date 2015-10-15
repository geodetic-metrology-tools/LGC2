// TWPSTFilter.h
//
// Concrete Class of the TWPSFilter PABC.
// Handles the IO of an TWPSMeasurement object for the case
// where there is only a transverse offset
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
//#include "TLGCTStream.h"
#include "TAStreamFormatter.h"
#include "TWPSMeasurement.h"

#include "TWPSTFilter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TWPSTFilter)



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
TWPSTFilter *TWPSTFilter::fFilter = 0;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TWPSTFilter::TWPSTFilter()
{}

TWPSTFilter::~TWPSTFilter()
{}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////
// returns the single instance of this class
////////////////////////////////////////////
TWPSTFilter *TWPSTFilter::instance()
{
	if( fFilter == 0 )
	{
		fFilter = new TWPSTFilter();
	}

	return fFilter;
}


////////////////////////////////////////////////
// inputs a TWPSMeasurement from the QTextStream
////////////////////////////////////////////////
//void TWPSTFilter::input(TLGCTStream &iStream, TWPSMeasurement &wps) const
void TWPSTFilter::input(TAStreamFormatter& iStream, TWPSMeasurement &wps) const
{
	TSpatialPointName end1Name, end2Name, setupName;
	TLength obsTOffset, tSigma(LITERAL(0.0)), obsVOffset(LITERAL(0.0)), vSigma(LITERAL(0.0));
	
	//get the wire's first end point's name
	iStream >> end1Name;

	//get the wps's setup point's name
	iStream >> setupName;

	//get the wire's second end point's name
	iStream >> end2Name;

	//possibly get the transverse offset
	if (!iStream.atEnd())
		iStream >> obsTOffset;

	//possibly read the transverse offset's precision
	if (!iStream.atEnd()){
		iStream >> tSigma;
		tSigma *= LITERAL(0.001);
	}

	//TWPSMeasurement* newWPS = new TWPSMeasurement(end1Name, setupName, end2Name, obsTOffset, tSigma, obsVOffset, vSigma);
	TWPSMeasurement* newWPS = new TWPSMeasurement(obsTOffset, tSigma, obsVOffset, vSigma);
	newWPS->activateTOffset();
	newWPS->disactivateVOffset();

	wps = *newWPS;

	delete newWPS;

	return;
}


