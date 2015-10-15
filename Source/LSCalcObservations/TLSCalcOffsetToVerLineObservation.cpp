//TLSCalcOffsetToVerLineObservation.cpp

#include "TLSCalcOffsetToVerLineObservation.h"



////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////
TLSCalcOffsetToVerLineObservation::TLSCalcOffsetToVerLineObservation()
{//Default constructor
	//sets attributes to 0
	// TODO: commented the next three lines
	//fTgPoint = 0;
	//fStPoint = 0;
	//fDistConst = 0;
}


TLSCalcOffsetToVerLineObservation::TLSCalcOffsetToVerLineObservation(OffsetToVerLineObsConstIter obs,
									LSPosVecIter stPoint,LSPosVecIter firstTargetPt,
									LSLengthIter cst):
TALSCalcLengthObservation(obs->getOffsetToVerLineValue(),obs->getSigmaValue()),
fOffsetToVerLineObs(obs),fStPoint(stPoint),fTgPoint(firstTargetPt), fDistConst(cst)
{/*constructor taking a ECVE offset measurement and pointers to lscalcparameters
 to initialize the lscalcOffsetToVerLine observation's attributes*/
}


TLSCalcOffsetToVerLineObservation::TLSCalcOffsetToVerLineObservation(const TLSCalcOffsetToVerLineObservation& source) :
TALSCalcLengthObservation(source)
{// copy constructor
	fOffsetToVerLineObs = source.fOffsetToVerLineObs;
	fStPoint = source.fStPoint;
	fTgPoint = source.fTgPoint;
	fDistConst = source.fDistConst;
}


TLSCalcOffsetToVerLineObservation::~TLSCalcOffsetToVerLineObservation()
{// Destructor
}


//////////////////////////////////////////////////////////////////////////////////////////
//PUBLIC MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////
bool	TLSCalcOffsetToVerLineObservation::operator==(const TLSCalcOffsetToVerLineObservation& right)
{// Overloaded equality operator
	bool isEqualTo = false;
	if (fOffsetToVerLineObs == right.fOffsetToVerLineObs)
	{
		isEqualTo = true;
	}
	return isEqualTo;
}

//////////////////////////////////////////////////////////////////////////
//PUBLIC ACCESS METHODS : Facade
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////
//end
///////////////////////////////////////////////////////////////////////////////////////////