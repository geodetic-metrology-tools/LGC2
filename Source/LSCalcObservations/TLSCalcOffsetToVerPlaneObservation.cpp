//TLSCalcOffsetToVerPlaneObservation.cpp

#include "TLSCalcOffsetToVerPlaneObservation.h"



////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////


TLSCalcOffsetToVerPlaneObservation::TLSCalcOffsetToVerPlaneObservation(const TOffsetToLineOrPlaneMeasurement* obs,
			LSPosVecIter measuredPoint,
			LSPosVecIter origin,
			LSFreeVecIter puv,
			LSLengthIter cte,
			LSLengthIter distanceFromOrigin):
TALSCalcLengthObservation(*(obs->getObservedValue()),TLength(0)),
fOffsetToVerPlaneObs(obs),fMeasuredPoint(measuredPoint),fDistConst(cte) 
{
	planeNormalVector = puv;
	fPlaneDistanceFromOrigin = distanceFromOrigin;
	originPosition = origin;

	calculateSigma();
}


//////////////////////////////////////////////////////////////////////////////////////////
//PUBLIC MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////
bool	TLSCalcOffsetToVerPlaneObservation::operator==(const TLSCalcOffsetToVerPlaneObservation& right)
{// Overloaded equality operator
	bool isEqualTo = false;
	if (fOffsetToVerPlaneObs == right.fOffsetToVerPlaneObs)
	{
		isEqualTo = true;
	}
	return isEqualTo;
}

void TLSCalcOffsetToVerPlaneObservation::calculateSigma()
{
	TReal distSigma = fOffsetToVerPlaneObs->getSigma()->getMetresValue() +
			fOffsetToVerPlaneObs->getObservedValue()->getKMetresValue() * fOffsetToVerPlaneObs->getPPM()->getMetresValue();
	TReal icSigma = fOffsetToVerPlaneObs->getScale()->getInstrumentCenteringSigma()->getMetresValue();
	TReal toSigma = fOffsetToVerPlaneObs->getScale()->getTargetOffsetSigma()->getMetresValue();
	TReal dcSigma = fOffsetToVerPlaneObs->getScale()->getDistanceCorrectionSigma()->getMetresValue();

	TLength sigma(sqrtq(distSigma * distSigma + icSigma * icSigma + toSigma * toSigma
		+ dcSigma * dcSigma));

	this->setSigmaAPriori(sigma);
}

//////////////////////////////////////////////////////////////////////////
//PUBLIC ACCESS METHODS
//////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////
//end
///////////////////////////////////////////////////////////////////////////////////////////
