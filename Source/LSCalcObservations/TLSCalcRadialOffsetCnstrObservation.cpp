//TLSCalcRadialOffsetCnstrObservation.cpp

#include "TLSCalcRadialOffsetCnstrObservation.h"



////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////

TLSCalcRadialOffsetCnstrObservation::TLSCalcRadialOffsetCnstrObservation(RadOffCnstrIterator obs,
									LSPosVecIter point): 
fRadialOffsetCnstrObs(obs),fPoint(point)
{
	fSigmaAPriori = obs->getCnstrSigma();
	fSigmaAPosteriori = fSigmaAPriori;
	fResidue = TLength(LITERAL(0.0));
	fResVariance = TDouble(LITERAL(0.0));
}


TLSCalcRadialOffsetCnstrObservation::TLSCalcRadialOffsetCnstrObservation(const TLSCalcRadialOffsetCnstrObservation& source) :
TALSCalcObservation(source)
{// copy constructor
	fSigmaAPriori = source.fSigmaAPriori;
	fSigmaAPosteriori = source.fSigmaAPosteriori;
	fResidue = source.fResidue;
	fResVariance = source.fResVariance;
	fRadialOffsetCnstrObs = source.fRadialOffsetCnstrObs;
	fPoint = source.fPoint;
}


TLSCalcRadialOffsetCnstrObservation::~TLSCalcRadialOffsetCnstrObservation()
{// Destructor
}


//////////////////////////////////////////////////////////////////////////////////////////
//PUBLIC MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////
bool	TLSCalcRadialOffsetCnstrObservation::operator==(const TLSCalcRadialOffsetCnstrObservation& right)
{// Overloaded equality operator
	bool isEqualTo = false;
	if (fRadialOffsetCnstrObs == right.fRadialOffsetCnstrObs)
	{
		isEqualTo = true;
	}
	return isEqualTo;
}

//////////////////////////////////////////////////////////////////////////
//PUBLIC ACCESS METHODS
//////////////////////////////////////////////////////////////////////////
TLength	TLSCalcRadialOffsetCnstrObservation::getNabla() const
{// Returns the nabla value as a TLength 
	
	TLength Nabla(getNablaValue());
	return Nabla;
}


TLength  TLSCalcRadialOffsetCnstrObservation::getG() const
{// Returns the g value as a TLength 
	
	TLength G(getGValue());
	return G;
}

/////////////////////////////////////////////////////////////////////////
// PUBLIC SETTING METHODS
/////////////////////////////////////////////////////////////////////////
void TLSCalcRadialOffsetCnstrObservation::setSigmaAPosteriori(TReal spost, TReal alpha, TReal beta, TReal s0)
{//Sets the sigma a posteriori 
	fSigmaAPosteriori.setMetresValue(spost);
	setResVariance(s0);
	computeErrorDetectionParam(alpha,beta,getResidue().getMetresValue(),getResVariance().getValue(),getSigmaAPriori().getMetresValue(),s0);
}


void TLSCalcRadialOffsetCnstrObservation::setResidual(TLength res)
{// Sets the residual of the computed measurement 
	fResidue = res;
}

TDouble		TLSCalcRadialOffsetCnstrObservation::getResDivSig() const
{/*! Returns the residue / sigma a priori */
	TLength res = fResidue;
	TLength sig = fSigmaAPriori;
	TDouble reSig = res/sig;
	return reSig;
}

/////////////////////////////////////////////////////////////////////////
// PROTECTED SETTING METHODS
/////////////////////////////////////////////////////////////////////////
void TLSCalcRadialOffsetCnstrObservation::setResVariance(TReal s0)
{// sets the variance of the residue after calculation
	TReal vr = fabsq(powq(s0*(fSigmaAPriori.getMetresValue()),2)-powq(fSigmaAPosteriori.getMetresValue(),2));
	fResVariance.setValue(vr);
}

///////////////////////////////////////////////////////////////////////////////////////////
//end
///////////////////////////////////////////////////////////////////////////////////////////
