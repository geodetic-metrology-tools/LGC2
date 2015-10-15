// TALSCalcLengthObservation.cpp

#include "TALSCalcLengthObservation.h"

//////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////
TALSCalcLengthObservation::TALSCalcLengthObservation() : TALSCalcObservation(), fObsDist(LITERAL(0.0)),
fResidue(LITERAL(0.0)), fSigmaAPriori(LITERAL(0.0)), fEstimatedDist(LITERAL(0.0)), fSigmaAPosteriori(LITERAL(0.0)), fResVariance(LITERAL(0.0))
{// Default constructor
}


TALSCalcLengthObservation::TALSCalcLengthObservation(TLength obsDist, TLength sAPriori) : 
TALSCalcObservation(), fObsDist(obsDist),fSigmaAPriori(sAPriori)
{// Constructor
	fEstimatedDist = fObsDist;
	fSigmaAPosteriori = fSigmaAPriori;
	fResidue = TLength(LITERAL(0.0));
	fResVariance = TDouble(LITERAL(0.0));
}


TALSCalcLengthObservation::TALSCalcLengthObservation(const TALSCalcLengthObservation& source) : 
TALSCalcObservation(source)
{// copy constructor
	fObsDist = source.fObsDist;
	fSigmaAPriori = source.fSigmaAPriori;
	fEstimatedDist = source.fEstimatedDist;
	fSigmaAPosteriori = source.fSigmaAPosteriori;
	fResidue = source.fResidue;
	fResVariance = source.fResVariance;
}


TALSCalcLengthObservation::~TALSCalcLengthObservation()
{// Destructor
}

//////////////////////////////////////
// Access methods
//////////////////////////////////////
TLength	TALSCalcLengthObservation::getNabla() const
{// Returns the nabla value as a TLength 
	
	TLength Nabla(getNablaValue());
	return Nabla;
}


TLength  TALSCalcLengthObservation::getG() const
{// Returns the g value as a TLength 
	
	TLength G(getGValue());
	return G;
}


TDouble		TALSCalcLengthObservation::getResDivSig() const
{/*! Returns the residue / sigma a priori */
	TLength res = fResidue;
	TLength sig = fSigmaAPriori;
	TDouble reSig = res/sig;
	return reSig;
}
///////////////////////////
// Settings
///////////////////////////

 
void TALSCalcLengthObservation::setSigmaAPosteriori(TReal  spost, TReal alpha, TReal beta, TReal s0)
{// Sets the sigma a posteriori
	fSigmaAPosteriori.setMetresValue(spost);
	setResVariance(s0);
	computeErrorDetectionParam(alpha,beta,getResidue().getMetresValue(),getResVariance().getValue(),getSigmaAPriori().getMetresValue(),s0);
}


void TALSCalcLengthObservation::setResidual(TLength res)
{// Sets the residual of the computed measurement 
	fResidue = res;
	setEstimatedDist();
}


void TALSCalcLengthObservation::setResVariance(TReal s0)
{// Sets the residual variance
	TReal rv = fabsq(powq(s0*(fSigmaAPriori.getMetresValue()),2)-powq(fSigmaAPosteriori.getMetresValue(),2));
	fResVariance.setValue(rv);
}


void TALSCalcLengthObservation::setSimulatedObsDist(TLength sObsDist)
{// Sets the simulated value of the observed distance 
	// the observed distance is now equal to a simulated value
	fObsDist = sObsDist;
	fEstimatedDist = fObsDist;
}

 
void TALSCalcLengthObservation::setEstimatedDist()
{// Sets the estimated distance (observed + residual)
	fEstimatedDist = fObsDist + fResidue;
}
