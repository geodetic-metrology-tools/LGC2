//TLSCalcGyroOrientationObservation.cpp

#include "TLSCalcGyroOrientationObservation.h"


////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////
TLSCalcGyroOrientationObservation::TLSCalcGyroOrientationObservation(const THorAngleMeasurement* obs,
													   LSPosVecIter st,LSPosVecIter tg,
													   LSOrientIter cte, const GyroscopeStation* station) :
TALSCalcAngleObservation(*(obs->getObservedValue()),*(obs->getSigma())),
fGyroOrieObs(obs),fStPoint(st),fTgPoint(tg),fAngleConst(cte)
{/*constructor taking an horizontal angle measurement and pointers to
	lscalcparameters  to initialize the lscalcGyroOrientation
	observation's attributes*/
	gStation = station;
}


// copy constructor
TLSCalcGyroOrientationObservation::TLSCalcGyroOrientationObservation(const TLSCalcGyroOrientationObservation& source) :
TALSCalcAngleObservation(source) {

	fGyroOrieObs = source.fGyroOrieObs; 
	fTgPoint = source.fTgPoint;
	fStPoint = source.fStPoint; 
	fAngleConst = source.fAngleConst;
	gStation = source.gStation;
}


TLSCalcGyroOrientationObservation::~TLSCalcGyroOrientationObservation()
{// Destructor
}


//////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION
//////////////////////////////////////////////////////////////////////////

void TLSCalcGyroOrientationObservation::calculateSigma()
{
	const TPositionVector& st = fStPoint->getEstimatedValue();
	const TPositionVector& tg = fTgPoint->getEstimatedValue();

	TReal dxy = sqrtq(powq(tg.getX().getMetresValue() - st.getX().getMetresValue(), 2) +
		powq(tg.getY().getMetresValue() - st.getY().getMetresValue(), 2));

	TReal a = powq(TAngle::gonsToRadsFactor() * LITERAL(0.0001) / dxy, 2);

	TReal distSigma = fGyroOrieObs->getSigma()->getRadiansValue();
	TReal icSigma = gStation->getInstrumentCenteringSigma()->getMetresValue() * gStation->getInstrumentCenteringSigma()->getMetresValue();
	TReal tcSigma = fGyroOrieObs->getTargetCenteringSigma()->getMetresValue() * fGyroOrieObs->getTargetCenteringSigma()->getMetresValue();

	TAngle sigma(sqrtq(distSigma * distSigma + a * (icSigma + tcSigma)));

	this->setSigmaAPriori(sigma);
}


bool TLSCalcGyroOrientationObservation::operator==(const TLSCalcGyroOrientationObservation& right)
{// Overloaded equality operator

	bool isEqualTo = false;
	if (fGyroOrieObs == right.fGyroOrieObs)
		isEqualTo = true;

	return isEqualTo;
}
 


//////////////////////////////////////////////////////////////////////////////////////
//PUBLIC ACCESS METHOD : FACADE
//////////////////////////////////////////////////////////////////////////////////////
TLength		TLSCalcGyroOrientationObservation::getStTgDistance() const
{/*!return the distance between the station and the target*/
	TFreeVector  distVec =(fTgPoint->getEstimatedValue() - fStPoint->getEstimatedValue());
	return distVec.getHorDist();
}


		
TLength		TLSCalcGyroOrientationObservation::getTgMovement() const
{/*!return the movement of the target (length)*/
	TFreeVector  distVec =(fTgPoint->getEstimatedValue() - fStPoint->getEstimatedValue());
	TAngle residue = getResidue();
	return residue.getRadiansValue()*distVec.length();
}

//////////////////////////////////////////////////////////////////
//end
//////////////////////////////////////////////////////////////////
