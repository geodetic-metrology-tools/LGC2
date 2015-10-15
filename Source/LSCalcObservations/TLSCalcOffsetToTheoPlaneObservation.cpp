//TLSCalcOffsetToTheoPlaneObservation.cpp

#include "TLSCalcOffsetToTheoPlaneObservation.h"



////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////


TLSCalcOffsetToTheoPlaneObservation::TLSCalcOffsetToTheoPlaneObservation(const TOffsetToTheoPlaneMeasurement* obs,
									LSPosVecIter stPoint,
									LSPosVecIter firstTargetPt, LSOrientIter v0,
									LSLengthIter cst, const TheodoliteStation* station):
TALSCalcLengthObservation(*(obs->getObservedValue()),TLength(0)),
fOffsetToTheoPlaneObs(obs),fStPoint(stPoint),fTarget(firstTargetPt), fV0(v0), fDistConst(cst), fHasOwnV0(false),
fAllPointsFixedV0(TAngle(0))
{/*constructor taking a ECTH offset measurement and pointers to lscalcparameters
 to initialize the lscalcOffsetToTheoPlane observation's attributes*/
	tStation = station;
	calculateSigma();
}


TLSCalcOffsetToTheoPlaneObservation::TLSCalcOffsetToTheoPlaneObservation(const TLSCalcOffsetToTheoPlaneObservation& source) :
TALSCalcLengthObservation(source)
{// copy constructor
	fOffsetToTheoPlaneObs = source.fOffsetToTheoPlaneObs;
	fStPoint = source.fStPoint;
	fTarget = source.fTarget;
	fV0 = source.fV0;

	fDistConst = source.fDistConst;
	fHasOwnV0 = source.hasOwnV0();
	fAllPointsFixedV0 = source.getAllPointsFixedV0();
	tStation = source.tStation;
}


TLSCalcOffsetToTheoPlaneObservation::~TLSCalcOffsetToTheoPlaneObservation()
{// Destructor
}


//////////////////////////////////////////////////////////////////////////////////////////
//PUBLIC MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////
bool	TLSCalcOffsetToTheoPlaneObservation::operator==(const TLSCalcOffsetToTheoPlaneObservation& right)
{// Overloaded equality operator
	bool isEqualTo = false;
	if (fOffsetToTheoPlaneObs == right.fOffsetToTheoPlaneObs)
	{
		isEqualTo = true;
	}
	return isEqualTo;
}

void TLSCalcOffsetToTheoPlaneObservation::calculateSigma()
{
	TReal distSigma = fOffsetToTheoPlaneObs->getSigma()->getMetresValue() +
			fOffsetToTheoPlaneObs->getObservedValue()->getKMetresValue() * fOffsetToTheoPlaneObs->getPPM()->getMetresValue();
	TReal icSigma = fOffsetToTheoPlaneObs->getScale()->getInstrumentCenteringSigma()->getMetresValue();
	TReal toSigma = fOffsetToTheoPlaneObs->getScale()->getTargetOffsetSigma()->getMetresValue();
	TReal dcSigma = fOffsetToTheoPlaneObs->getScale()->getDistanceCorrectionSigma()->getMetresValue();
	TReal theoICSigma = tStation->getInstrumentCenteringSigma()->getMetresValue();

	TLength sigma(sqrtq(distSigma * distSigma + icSigma * icSigma + toSigma * toSigma
		+ dcSigma * dcSigma + theoICSigma * theoICSigma));

	this->setSigmaAPriori(sigma);
}

//////////////////////////////////////////////////////////////////////////
//PUBLIC ACCESS METHODS
//////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////
//end
///////////////////////////////////////////////////////////////////////////////////////////
