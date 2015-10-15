//TLSCalcZenithDistObservation.cpp

#include "TLSCalcZenithDistObservation.h"
#include "TZenithDistMeasurement.h"

////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////


TLSCalcZenithDistObservation::TLSCalcZenithDistObservation(const TZenithDistMeasurement* zdObs,
		LSLengthIter hi,TLength hp,LSPosVecIter st,LSPosVecIter tg, const TheodoliteStation* station):
TALSCalcAngleObservation(*(zdObs->getObservedValue()),*(zdObs->getSigma())),
fZenDistObs(zdObs),fInstrumentHeight(hi),fPrismHeight(hp),
fStPoint(st),fTgPoint(tg), fHasOwnInstrumentHeight(false), fAllPointsFixedInstrumentHeight(TLength(0))
{/* constructor taking a zenith distance measurement observation 
	to initialize the lscalczenithdist observation's attributes*/
	tStation = station;
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
	tStation = source.tStation;
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


void TLSCalcZenithDistObservation::calculateSigma()
{
	const TPositionVector& st = fStPoint->getEstimatedValue();
	const TPositionVector& tg = fTgPoint->getEstimatedValue();

	TReal dz = tg.getZ().getMetresValue() - st.getZ().getMetresValue();
	TReal dxy = sqrtq(powq(tg.getX().getMetresValue() - st.getX().getMetresValue(), 2) +
		powq(tg.getY().getMetresValue() - st.getY().getMetresValue(), 2));
	TReal dxyz = sqrtq(powq(tg.getX().getMetresValue() - st.getX().getMetresValue(), 2) +
		powq(tg.getY().getMetresValue() - st.getY().getMetresValue(), 2) +
		powq(tg.getZ().getMetresValue() - st.getZ().getMetresValue(), 2));

	TReal dxyz2 = dxyz * dxyz;
	TReal a = powq(dxy / dxyz2, 2);
	TReal b = (dz - fInstrumentHeight->getEstimatedValue().getMetresValue() +
		fPrismHeight.getMetresValue()) / dxyz2;

	TReal distSigma = fZenDistObs->getSigma()->getRadiansValue();
	TReal icSigma = tStation->getInstrumentCenteringSigma()->getMetresValue() * tStation->getInstrumentCenteringSigma()->getMetresValue();
	TReal ihSigma = tStation->getInstrumentHeightSigma()->getMetresValue() * tStation->getInstrumentHeightSigma()->getMetresValue();
	TReal tcSigma = fZenDistObs->getTargetCenteringSigma()->getMetresValue() * fZenDistObs->getTargetCenteringSigma()->getMetresValue();
	TReal thSigma = fZenDistObs->getTargetHeightSigma()->getMetresValue() * fZenDistObs->getTargetHeightSigma()->getMetresValue();
	
	TAngle sigma(sqrtq(distSigma * distSigma + a * (ihSigma + thSigma) +
		b * (icSigma + tcSigma)));

	this->setSigmaAPriori(sigma);
}


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

TLength		TLSCalcZenithDistObservation::getStTgDistance() const
{/*!return the distance between the station and the target*/
	TFreeVector  distVec =(fTgPoint->getEstimatedValue() - fStPoint->getEstimatedValue());
	return distVec.getHorDist();
}

//////////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////////



