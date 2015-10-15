//TLSCalcOffsetToTheoPlaneObservation.cpp

#include "TLSCalcOffsetToTheoPlaneObservation.h"



////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////
TLSCalcOffsetToTheoPlaneObservation::TLSCalcOffsetToTheoPlaneObservation()
{//Default constructor
	//sets attributes to 0
	// TODO: commented the next four lines
	//fFirstTgPoint = 0;
	//fV0 = 0;
	//fStPoint = 0;
	//fDistConst = 0;

	fHasOwnV0 = false;
	fAllPointsFixedV0 = TAngle(0);
}


TLSCalcOffsetToTheoPlaneObservation::TLSCalcOffsetToTheoPlaneObservation(OffsetToTheoPlaneObsConstIter obs,
									LSPosVecIter stPoint,
									LSPosVecIter firstTargetPt, LSOrientIter v0,
									LSLengthIter cst):
TALSCalcLengthObservation(obs->getOffsetToTheoPlaneValue(),obs->getSigmaValue()),
fOffsetToTheoPlaneObs(obs),fStPoint(stPoint),fFirstTgPoint(firstTargetPt), fV0(v0), fDistConst(cst), fHasOwnV0(false),
fAllPointsFixedV0(TAngle(0))
{/*constructor taking a ECTH offset measurement and pointers to lscalcparameters
 to initialize the lscalcOffsetToTheoPlane observation's attributes*/
}


TLSCalcOffsetToTheoPlaneObservation::TLSCalcOffsetToTheoPlaneObservation(const TLSCalcOffsetToTheoPlaneObservation& source) :
TALSCalcLengthObservation(source)
{// copy constructor
	fOffsetToTheoPlaneObs = source.fOffsetToTheoPlaneObs;
	fStPoint = source.fStPoint;
	fFirstTgPoint = source.fFirstTgPoint;
	fV0 = source.fV0;

	fDistConst = source.fDistConst;
	fHasOwnV0 = source.hasOwnV0();
	fAllPointsFixedV0 = getAllPointsFixedV0();
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

//////////////////////////////////////////////////////////////////////////
//PUBLIC ACCESS METHODS
//////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////
//end
///////////////////////////////////////////////////////////////////////////////////////////