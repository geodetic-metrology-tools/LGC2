//TLSCalcVertDistObservation.cpp

#include "TLSCalcVertDistObservation.h"
#include "TVerticalDistMeasurement.h"


///////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
///////////////////////////////////////////////////////////////////////////////////////////
TLSCalcVertDistObservation::TLSCalcVertDistObservation()
{// Default constructor
	//sets attributes to 0
	// TODO: commented the next three lines
	//fRefPoint = 0;
	//fTgPoint = 0;
	//fDistConst = 0;
}

TLSCalcVertDistObservation::TLSCalcVertDistObservation(VertDistObsConstIter vdmeas,LSPosVecIter ref,LSPosVecIter tg, LSLengthIter cst):
TALSCalcLengthObservation(vdmeas->getVertDistValue(),vdmeas->getSigmaValue()), 
fVertDistObs(vdmeas), fDistConst(cst)
{/*constructor taking a vertical distance measurement and pointers to
 lscalcparameters to initialize the lscalcvertdist observation's attributes*/
	fRefPoint = ref;
	fTgPoint = tg;
}


TLSCalcVertDistObservation::TLSCalcVertDistObservation(const TLSCalcVertDistObservation& source) :
TALSCalcLengthObservation(source) 
{// copy constructor
	fVertDistObs = source.fVertDistObs;
	fRefPoint = source.fRefPoint;
	fTgPoint = source.fTgPoint;
	fDistConst = source.fDistConst;
}

/*
// copy assignment operator
TLSCalcVertDistObservation& TLSCalcVertDistObservation::operator=(TLSCalcVertDistObservation& right) 
{	
	// not implemented
}
*/


TLSCalcVertDistObservation::~TLSCalcVertDistObservation()
{// Destructor
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//MEMBER FUNCTION
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool TLSCalcVertDistObservation::operator==(const TLSCalcVertDistObservation& right)
{// Overloaded equality operator 
	bool isEqualTo = false;
	if (fVertDistObs == right.fVertDistObs)
		isEqualTo = true;

	return isEqualTo;
}


//////////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////////




