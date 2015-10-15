//TLSCalcHorDistObservation.cpp

#include "TLSCalcHorDistObservation.h"
#include "THorizontalDistMeas.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////////////////////////////////////
TLSCalcHorDistObservation::TLSCalcHorDistObservation()
{// Default constructor
	//sets attributes to 0
	// TODO: commented the next three lines
	//fTgPoint = 0;
	//fStPoint = 0;
	//fDistConst = 0;
}


TLSCalcHorDistObservation::TLSCalcHorDistObservation(HorDistObsConstIter hdmeas,
													 LSPosVecIter st,LSPosVecIter tg, LSLengthIter cst):
TALSCalcLengthObservation(hdmeas->getHorDistValue(),(hdmeas->getSigmaValue() + (hdmeas->getHorDistValue().getKMetresValue() * hdmeas->getSigmaPpmValue())) ),
fHorDistObs(hdmeas),fStPoint(st),fTgPoint(tg), fDistConst(cst)
{/* constructor taking an horizontal distance measurement
	and pointers to lscalcparameters  to initialize
		the lscalchordist observation's attributes*/

}


TLSCalcHorDistObservation::TLSCalcHorDistObservation(const TLSCalcHorDistObservation& source) :
TALSCalcLengthObservation(source)
{// copy constructor

	fHorDistObs = source.fHorDistObs;
	fStPoint = source.fStPoint;
	fTgPoint = source.fTgPoint;
	fDistConst = source.fDistConst;
}

/*
// copy assignment operator
TLSCalcHorDistObservation& TLSCalcHorDistObservation::operator=(TLSCalcHorDistObservation& right) 
{	
	// not implemented
}
*/


TLSCalcHorDistObservation::~TLSCalcHorDistObservation()
{// Destructor
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//MEMBER FUNCTION
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool	TLSCalcHorDistObservation::operator==(const TLSCalcHorDistObservation& right)
{// Overloaded equality operator

	bool isEqualTo = false;
	if (fHorDistObs == right.fHorDistObs)
		isEqualTo = true;

	return isEqualTo;
}

//////////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////////


