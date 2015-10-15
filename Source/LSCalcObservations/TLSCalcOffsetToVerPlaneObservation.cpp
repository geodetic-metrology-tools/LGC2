//TLSCalcOffsetToVerPlaneObservation.cpp

#include "TLSCalcOffsetToVerPlaneObservation.h"



////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////
TLSCalcOffsetToVerPlaneObservation::TLSCalcOffsetToVerPlaneObservation()
{//Default constructor
	//sets attributes to 0
	// TODO: commented the next four lines
	//fFirstTgPoint = 0;
	//fSecondTgPoint = 0;
	//fStPoint = 0;
	//fDistConst = 0;
}


TLSCalcOffsetToVerPlaneObservation::TLSCalcOffsetToVerPlaneObservation(OffsetToVerPlaneObsConstIter obs,
									LSPosVecIter stPoint,
									LSPosVecIter firstTargetPt, LSPosVecIter secondTargetPt,
									LSLengthIter cst):
TALSCalcLengthObservation(obs->getOffsetToVerPlaneValue(),obs->getSigmaValue()),
fOffsetToVerPlaneObs(obs),fStPoint(stPoint),fFirstTgPoint(firstTargetPt),fSecondTgPoint(secondTargetPt), fDistConst(cst) 
{/*constructor taking a ECHO offset measurement and pointers to lscalcparameters
 to initialize the lscalcOffsetToVerPlane observation's attributes*/
}


TLSCalcOffsetToVerPlaneObservation::TLSCalcOffsetToVerPlaneObservation(const TLSCalcOffsetToVerPlaneObservation& source) :
TALSCalcLengthObservation(source)
{// copy constructor
	fOffsetToVerPlaneObs = source.fOffsetToVerPlaneObs;
	fStPoint = source.fStPoint;
	fFirstTgPoint = source.fFirstTgPoint;
	fSecondTgPoint = source.fSecondTgPoint;

	fDistConst = source.fDistConst;
}


TLSCalcOffsetToVerPlaneObservation::~TLSCalcOffsetToVerPlaneObservation()
{// Destructor
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

//////////////////////////////////////////////////////////////////////////
//PUBLIC ACCESS METHODS
//////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////
//end
///////////////////////////////////////////////////////////////////////////////////////////