//TLSCalcHorAngleObservation.cpp

#include "TLSCalcHorAngleObservation.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
///////////////////////////////////////////////////////////////////////////////////////////////////////

TLSCalcHorAngleObservation::TLSCalcHorAngleObservation(const THorAngleMeasurement* haObs,LSOrientIter v0,
													   LSPosVecIter st,LSPosVecIter tg,LSOrientIter cte, const TheodoliteStation* station) :
TALSCalcAngleObservation(*(haObs->getObservedValue()),*(haObs->getSigma())), horizontalAngleObservation(haObs),fV0(v0),
fStPoint(st),fTgPoint(tg),fAngleConst(cte), fHasOwnV0(false)
{/* constructor taking an horizontal angle measurement and pointers to lscalcparameters
	to initialize the lscalchorangle observation's attributes*/
	tStation = station;
}


TLSCalcHorAngleObservation::TLSCalcHorAngleObservation(const TLSCalcHorAngleObservation& source) :
TALSCalcAngleObservation(source)
{// copy constructor
	horizontalAngleObservation = source.horizontalAngleObservation; 

	fV0 = source.fV0; 
	fTgPoint = source.fTgPoint;
	fStPoint = source.fStPoint; 
	fAngleConst = source.fAngleConst;
	fHasOwnV0 = source.hasOwnV0();
	tStation = source.tStation;
}

/*
// copy assignment operator
TLSCalcHorAngleObservation& TLSCalcHorAngleObservation::operator=(TLSCalcHorAngleObservation& right) 
{	
	// not implemented
}
*/


TLSCalcHorAngleObservation::~TLSCalcHorAngleObservation()
{// Destructor
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//MEMBER FUNCTION
///////////////////////////////////////////////////////////////////////////////////////////////////////


void TLSCalcHorAngleObservation::calculateSigma()
{
	const TPositionVector& st = fStPoint->getEstimatedValue();
	const TPositionVector& tg = fTgPoint->getEstimatedValue();

	TReal dxy = sqrtq(powq(tg.getX().getMetresValue() - st.getX().getMetresValue(), 2) +
		powq(tg.getY().getMetresValue() - st.getY().getMetresValue(), 2));

	TReal a = powq(1 / dxy, 2);

	TReal distSigma = horizontalAngleObservation->getSigma()->getRadiansValue();
	TReal icSigma = tStation->getInstrumentCenteringSigma()->getMetresValue() * tStation->getInstrumentCenteringSigma()->getMetresValue();
	TReal tcSigma = horizontalAngleObservation->getTargetCenteringSigma()->getMetresValue() * horizontalAngleObservation->getTargetCenteringSigma()->getMetresValue();

	TAngle sigma(sqrtq(distSigma * distSigma + a * (icSigma + tcSigma)));

	this->setSigmaAPriori(sigma);
}


bool TLSCalcHorAngleObservation::operator==(const TLSCalcHorAngleObservation& right)
{// Overloaded equality operator 

	bool isEqualTo = false;
	if (horizontalAngleObservation == right.horizontalAngleObservation)
		isEqualTo = true;

	return isEqualTo;
}

//////////////////////////////////////////////////////////////////////////////////////
//PUBLIC ACCESS METHOD : FACADE
//////////////////////////////////////////////////////////////////////////////////////
TLength		TLSCalcHorAngleObservation::getStTgDistance() const
{/*!return the distance between the station and the target*/
	TFreeVector  distVec =(fTgPoint->getEstimatedValue() - fStPoint->getEstimatedValue());
	return distVec.getHorDist();
}


		
TLength		TLSCalcHorAngleObservation::getTgMovement() const
{/*!return the movement of the target (length)*/
	TFreeVector  distVec =(fTgPoint->getEstimatedValue() - fStPoint->getEstimatedValue());
	TAngle residue = getResidue();
	return residue.getRadiansValue()*distVec.getHorDist();
}


//////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////









