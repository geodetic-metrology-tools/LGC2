//TALSCalcAngleObservation.cpp

#include "TALSCalcAngleObservation.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
///////////////////////////////////////////////////////////////////////////////////////////////////
TALSCalcAngleObservation::TALSCalcAngleObservation() : TALSCalcObservation(), fObsAngle(LITERAL(0.0)),
fResidue(LITERAL(0.0)), fSigmaAPriori(LITERAL(0.0)), fEstimatedAngle(LITERAL(0.0)), fSigmaAPosteriori(LITERAL(0.0)), fResVariance(LITERAL(0.0))
{// Default constructor
}


TALSCalcAngleObservation::TALSCalcAngleObservation(TAngle obsAngle, TAngle sAPriori) : 
TALSCalcObservation(), fObsAngle(obsAngle),
fSigmaAPriori(sAPriori)
{// Constructor
	
	fEstimatedAngle = fObsAngle;
	fSigmaAPosteriori = fSigmaAPriori;
	fResidue = TAngle(LITERAL(0.0));
	fResVariance = TDouble(LITERAL(0.0));
}


TALSCalcAngleObservation::TALSCalcAngleObservation(const TALSCalcAngleObservation& source) : 
TALSCalcObservation(source)
{// copy constructor

	fObsAngle = source.fObsAngle;
	fResidue = source.fResidue;
	fSigmaAPriori = source.fSigmaAPriori;
	fEstimatedAngle = source.fEstimatedAngle;
	fSigmaAPosteriori = source.fSigmaAPosteriori;
	fResVariance = source.fResVariance;
}


TALSCalcAngleObservation::~TALSCalcAngleObservation()
{// Destructor
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//ACCESS METHOD : FACADE
///////////////////////////////////////////////////////////////////////////////////////////////////
TAngle	TALSCalcAngleObservation::getNabla() const
{// Returns the nabla value as a TAngle 
	TAngle Nabla(getNablaValue());
	return Nabla;
}


TAngle  TALSCalcAngleObservation::getG() const
{// Returns the g value as a TAngle 
	TAngle G(getGValue());
	return G;
}


TDouble		TALSCalcAngleObservation::getResDivSig() const
{/*! Returns the residue / sigma a priori */
	TAngle res = fResidue;
	TAngle sig = fSigmaAPriori;
	TDouble reSig = res/sig;
	return reSig;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//SETTING METHOD
///////////////////////////////////////////////////////////////////////////////////////////////////
void TALSCalcAngleObservation::setSigmaAPosteriori(TReal spost, TReal alpha, TReal beta, TReal s0)
{// Sets the sigma a posteriori and reliability factors alpha & beta
	fSigmaAPosteriori.setRadiansValue(spost);
	setResVariance(s0);
	computeErrorDetectionParam(alpha,beta,getResidue().getRadiansValue(),getResVariance().getValue(),getSigmaAPriori().getRadiansValue(),s0);
}

 
void TALSCalcAngleObservation::setResidual(TAngle res)
{// Sets the residual of the computed measurement
	fResidue = res;
	setEstimatedAngle();
}


void TALSCalcAngleObservation::setResVariance(TReal s0)
{// sets the variance of the residue after calculation
	TReal vr = fabsq(powq((s0*fSigmaAPriori.getRadiansValue()),2)-powq(fSigmaAPosteriori.getRadiansValue(),2));
	fResVariance.setValue(vr);
}


void TALSCalcAngleObservation::setSimulatedObsAngle(TAngle sObsAng)
{// Sets the simulated value of the observed angle 
	// the observed angle is now equal to a simulated value
	fObsAngle = sObsAng;
	fEstimatedAngle = fObsAngle;
}

 
void TALSCalcAngleObservation::setEstimatedAngle()
{// Sets the estimated angle (observed + residual)
	fEstimatedAngle = fObsAngle + fResidue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//END
///////////////////////////////////////////////////////////////////////////////////////////////////
