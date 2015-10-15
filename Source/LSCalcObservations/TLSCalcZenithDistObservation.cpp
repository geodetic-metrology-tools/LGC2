//TLSCalcZenithDistObservation.cpp

#include "TLSCalcZenithDistObservation.h"
#include "TZenithDistMeasurement.h"

////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////
TLSCalcZenithDistObservation::TLSCalcZenithDistObservation()
{// Default constructor
	//sets attributes to 0
	// TODO: commented the next three lines
	//fInstrumentHeight = 0;
	//fTgPoint = 0;
	//fStPoint = 0;

	TLength zero (0);
	fPrismHeight = zero;
	fAllPointsFixedInstrumentHeight = TLength(0);
	fHasOwnInstrumentHeight = false;
}


TLSCalcZenithDistObservation::TLSCalcZenithDistObservation(ZenDistObsConstIter zdObs,
		LSLengthIter hi,TLength hp,LSPosVecIter st,LSPosVecIter tg):
TALSCalcAngleObservation(zdObs->getZenAngleValue(),zdObs->getSigmaValue()),
fZenDistObs(zdObs),fInstrumentHeight(hi),fPrismHeight(hp),
fStPoint(st),fTgPoint(tg), fHasOwnInstrumentHeight(false), fAllPointsFixedInstrumentHeight(TLength(0))
{/* constructor taking a zenith distance measurement observation 
	to initialize the lscalczenithdist observation's attributes*/
}


TLSCalcZenithDistObservation::TLSCalcZenithDistObservation(const TLSCalcZenithDistObservation& source) :
TALSCalcAngleObservation(source)
{// copy constructor
	fZenDistObs = source.fZenDistObs;

	fInstrumentHeight = source.fInstrumentHeight; 
	fPrismHeight = source.fPrismHeight; 
	fTgPoint = source.fTgPoint; 
	fStPoint = source.fStPoint;
	fHasOwnInstrumentHeight = source.hasOwnInstrumentHeight();
	fAllPointsFixedInstrumentHeight = source.getAllPointsFixedInstrumentHeight();
}


TLSCalcZenithDistObservation::~TLSCalcZenithDistObservation() 
{// Destructor
}

/*
// copy assignment operator
TLSCalcZenithDistObservation& TLSCalcZenithDistObservation::operator=(TLSCalcZenithDistObservation& right) 
{	
	// not implemented
}
*/

////////////////////////////////////////////////////////
//MEMBER FUNCTION
////////////////////////////////////////////////////////
bool	TLSCalcZenithDistObservation::operator==(const TLSCalcZenithDistObservation& right)
{// Overloaded equality operator

	bool isEqualTo = false;

	if (fZenDistObs == right.fZenDistObs)
		isEqualTo = true;

	return isEqualTo;
}

		
TLength		TLSCalcZenithDistObservation::getTgMovement() const
{/*!return the movement of the target (length)*/
	TFreeVector  distVec = fTgPoint->getEstimatedValue() - fStPoint->getEstimatedValue();
	TAngle residue = getResidue();
	return residue.getRadiansValue()*distVec.length();
}


//////////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////////



