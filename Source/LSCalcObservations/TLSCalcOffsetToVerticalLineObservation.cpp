//TLSCalcOffsetToVerticalLineObservation.cpp

#include "TLSCalcOffsetToVerticalLineObservation.h"



TLSCalcOffsetToVerticalLineObservation::TLSCalcOffsetToVerticalLineObservation(const TOffsetToLineOrPlaneMeasurement* obs,
			LSPosVecIter target,
			LSPosVecIter origin,
			LSPosVecIter pointOnLine,
			LSFreeVecIter unit,
			LSLengthIter cte) :
TALSCalcLengthObservation(*(obs->getObservedValue()), TLength(0)),
fUnitVector(unit), fOffsetToLineObs(obs),fTgPoint(target), fDistConst(cte)
{
	this->pointOnLine = pointOnLine;
	this->origin = origin;

	this->calculateSigma();
}


//////////////////////////////////////////////////////////////////////////////////////////
//PUBLIC MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////
bool	TLSCalcOffsetToVerticalLineObservation::operator==(const TLSCalcOffsetToVerticalLineObservation& right)
{// Overloaded equality operator
	bool isEqualTo = false;
	if (fOffsetToLineObs == right.fOffsetToLineObs)
	{
		isEqualTo = true;
	}
	return isEqualTo;
}

void TLSCalcOffsetToVerticalLineObservation::calculateSigma()
{
	TReal distSigma = fOffsetToLineObs->getSigma()->getMetresValue() +
			fOffsetToLineObs->getObservedValue()->getKMetresValue() * fOffsetToLineObs->getPPM()->getMetresValue();
	TReal icSigma = fOffsetToLineObs->getScale()->getInstrumentCenteringSigma()->getMetresValue();
	TReal toSigma = fOffsetToLineObs->getScale()->getTargetOffsetSigma()->getMetresValue();
	TReal dcSigma = fOffsetToLineObs->getScale()->getDistanceCorrectionSigma()->getMetresValue();

	TLength sigma(sqrtq(distSigma * distSigma + icSigma * icSigma + toSigma * toSigma
		+ dcSigma * dcSigma));

	this->setSigmaAPriori(sigma);
}

//////////////////////////////////////////////////////////////////////////
//PUBLIC ACCESS METHODS : Facade
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////
//end
///////////////////////////////////////////////////////////////////////////////////////////
