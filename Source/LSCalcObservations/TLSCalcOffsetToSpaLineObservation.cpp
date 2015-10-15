//TLSCalcOffsetToSpaLineObservation.cpp

#include "TLSCalcOffsetToSpaLineObservation.h"



////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////
TLSCalcOffsetToSpaLineObservation::TLSCalcOffsetToSpaLineObservation()
{//Default constructor
	//sets attributes to 0
	// TODO: commented the next four lines
	//fFirstTgPoint = 0;
	//fSecondTgPoint = 0;
	//fStPoint = 0;
	//fDistConst = 0;
}


TLSCalcOffsetToSpaLineObservation::TLSCalcOffsetToSpaLineObservation(OffsetToSpaLineObsConstIter obs,
									LSPosVecIter stPoint,
									LSPosVecIter firstTargetPt, LSPosVecIter secondTargetPt,
									LSLengthIter cst):
TALSCalcLengthObservation(obs->getOffsetToSpaLineValue(),obs->getSigmaValue()),
fOffsetToSpaLineObs(obs),fStPoint(stPoint),fFirstTgPoint(firstTargetPt),fSecondTgPoint(secondTargetPt), fDistConst(cst)
{/*constructor taking a ECSP offset measurement and pointers to lscalcparameters
 to initialize the lscalcOffsetToSpaLine observation's attributes*/
}


TLSCalcOffsetToSpaLineObservation::TLSCalcOffsetToSpaLineObservation(const TLSCalcOffsetToSpaLineObservation& source) :
TALSCalcLengthObservation(source)
{// copy constructor
	fOffsetToSpaLineObs = source.fOffsetToSpaLineObs;
	fStPoint = source.fStPoint;
	fFirstTgPoint = source.fFirstTgPoint;
	fSecondTgPoint = source.fSecondTgPoint;

	fDistConst = source.fDistConst;
}


TLSCalcOffsetToSpaLineObservation::~TLSCalcOffsetToSpaLineObservation()
{// Destructor
}


//////////////////////////////////////////////////////////////////////////////////////////
//PUBLIC MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////
bool	TLSCalcOffsetToSpaLineObservation::operator==(const TLSCalcOffsetToSpaLineObservation& right)
{// Overloaded equality operator
	bool isEqualTo = false;
	if (fOffsetToSpaLineObs == right.fOffsetToSpaLineObs)
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