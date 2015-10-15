//TLSCalcHorAngleObservation.cpp

#include "TLSCalcHorAngleObservation.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
///////////////////////////////////////////////////////////////////////////////////////////////////////
TLSCalcHorAngleObservation::TLSCalcHorAngleObservation() : TALSCalcAngleObservation() 
{// Default constructor
	//sets attributes to 0
	// TODO: commented the next four lines
	//fV0 = 0;
	//fTgPoint = 0;
	//fStPoint = 0;
	//fAngleConst = 0;
	fHasOwnV0 = false;
}

TLSCalcHorAngleObservation::TLSCalcHorAngleObservation(HorAngObsConstIter haObs,LSOrientIter v0,
													   LSPosVecIter st,LSPosVecIter tg,LSOrientIter cte) :
TALSCalcAngleObservation(haObs->getHorAngleValue(),haObs->getSigmaValue()), fHorAngObs(haObs),fV0(v0),
fStPoint(st),fTgPoint(tg),fAngleConst(cte), fHasOwnV0(false)
{/* constructor taking an horizontal angle measurement and pointers to lscalcparameters
	to initialize the lscalchorangle observation's attributes*/
}


TLSCalcHorAngleObservation::TLSCalcHorAngleObservation(const TLSCalcHorAngleObservation& source) :
TALSCalcAngleObservation(source)
{// copy constructor
	fHorAngObs = source.fHorAngObs; 

	fV0 = source.fV0; 
	fTgPoint = source.fTgPoint;
	fStPoint = source.fStPoint; 
	fAngleConst = source.fAngleConst;
	fHasOwnV0 = source.hasOwnV0();
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
bool TLSCalcHorAngleObservation::operator==(const TLSCalcHorAngleObservation& right)
{// Overloaded equality operator 

	bool isEqualTo = false;
	if (fHorAngObs == right.fHorAngObs)
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









