//TLSCalcGyroOrientationObservation.cpp

#include "TLSCalcGyroOrientationObservation.h"


////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////
TLSCalcGyroOrientationObservation::TLSCalcGyroOrientationObservation()
{// Default constructor
	//sets attributes to 0
	// TODO: commented the next three lines
	//fTgPoint = 0;
	//fStPoint = 0;
	//fAngleConst = 0;
}

TLSCalcGyroOrientationObservation::TLSCalcGyroOrientationObservation(GyroOrieObsConstIter obs,
													   LSPosVecIter st,LSPosVecIter tg,
													   LSOrientIter cte) :
TALSCalcAngleObservation(obs->getGyroOrieValue(),obs->getSigmaValue()),
fGyroOrieObs(obs),fStPoint(st),fTgPoint(tg),fAngleConst(cte)
{/*constructor taking an horizontal angle measurement and pointers to
	lscalcparameters  to initialize the lscalcGyroOrientation
	observation's attributes*/
}


// copy constructor
TLSCalcGyroOrientationObservation::TLSCalcGyroOrientationObservation(const TLSCalcGyroOrientationObservation& source) :
TALSCalcAngleObservation(source) {

	fGyroOrieObs = source.fGyroOrieObs; 
	fTgPoint = source.fTgPoint;
	fStPoint = source.fStPoint; 
	fAngleConst = source.fAngleConst;
}


TLSCalcGyroOrientationObservation::~TLSCalcGyroOrientationObservation()
{// Destructor
}


//////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION
//////////////////////////////////////////////////////////////////////////
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