//TLSCalcSpatialDistObservation.cpp

#include "TLSCalcSpatialDistObservation.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////////////////
TLSCalcSpatialDistObservation::TLSCalcSpatialDistObservation()
{// Default constructor
	//sets attributes to 0
	// TODO: commented the next four lines
	//fInstrumentHeight = 0;
	//fDistConst = 0;
	//fTgPoint = 0;
	//fStPoint = 0;

	TLength zero (0);
	fPrismHeight = zero;

	fHasOwnInstrumentHeight = false;
	fAllPointsFixedInstrumentHeight = zero;
}

TLSCalcSpatialDistObservation::TLSCalcSpatialDistObservation(SpaDistObsConstIter sdObs,
															 LSLengthIter hi,TLength hp,
															LSLengthIter cst, LSPosVecIter st,LSPosVecIter tg):
TALSCalcLengthObservation(sdObs->getSpatialDistValue(),
						  (sdObs->getSigmaValue() + (sdObs->getSpatialDistValue().getKMetresValue() * sdObs->getSigmaPpmValue()) ) ),
fSpaDistObs(sdObs),fInstrumentHeight(hi),fPrismHeight(hp),fDistConst(cst),fStPoint(st),
fTgPoint(tg), fHasOwnInstrumentHeight(false), fAllPointsFixedInstrumentHeight(TLength(0))
{/*constructor taking a spatial distance measurement and pointers
 to lscalcparameters to initialize the lscalcspatialdist observation's attributes*/
}



TLSCalcSpatialDistObservation::TLSCalcSpatialDistObservation(const TLSCalcSpatialDistObservation& source) :
TALSCalcLengthObservation(source)
{// copy constructor
	fSpaDistObs = source.fSpaDistObs;
	fInstrumentHeight = source.fInstrumentHeight;
	fPrismHeight = source.fPrismHeight;
	fDistConst = source.fDistConst;
	fStPoint = source.fStPoint;
	fTgPoint = source.fTgPoint;

	fHasOwnInstrumentHeight = source.hasOwnInstrumentHeight();
	fAllPointsFixedInstrumentHeight = source.getAllPointsFixedInstrumentHeight();

}

/*
// copy assignment operator
TLSCalcSpatialDistObservation& TLSCalcSpatialDistObservation::operator=(TLSCalcSpatialDistObservation& right) 
{	
	// not implemented
}
*/


TLSCalcSpatialDistObservation::~TLSCalcSpatialDistObservation()
{// Destructor
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool	TLSCalcSpatialDistObservation::operator==(const TLSCalcSpatialDistObservation& right)
{// Overloaded equality operator
	bool isEqualTo = false;
	if (fSpaDistObs == right.fSpaDistObs)
		isEqualTo = true;

	return isEqualTo;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC ACCESS METHODS
////////////////////////////////////////////////////////////////////////////////////////////////////////
TDouble		TLSCalcSpatialDistObservation::getSensitivity() const
{
	TLength deltaZ =	fTgPoint->getEstimatedValue().getZ()
						+ getHPrism()
						- fStPoint->getEstimatedValue().getZ()
						- getEstHInst();
	TLength dist = getEstimatedDist() + getEstDistConst();
	TDouble sensitivity ((deltaZ.getMMetresValue()/dist.getMMetresValue())*10);

	return sensitivity;
}


TLength		TLSCalcSpatialDistObservation::getEstValueForVariableDistConst() const
{
	
	if(getDistConstStatus() == TALSCalcParameter::kVariable)
	{
		return getEstDistConst();
	}
	else
	{
		TLength zero (0);
		return (zero);
	}
}


TLength		TLSCalcSpatialDistObservation::getEstSigmaValueForVariableDistConst() const
{
	if(getDistConstStatus() == TALSCalcParameter::kVariable)
	{
		return (getDistConst()->getEstimatedPrecision());
	}
	else
	{
		TLength zero (0);
		return (zero);
	}
}


