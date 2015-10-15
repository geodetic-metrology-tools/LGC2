//TLSCalcOrientationCnstrObservation.cpp

#include "TLSCalcOrientationCnstrObservation.h"



////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////
TLSCalcOrientationCnstrObservation::TLSCalcOrientationCnstrObservation() : TALSCalcObservation()
{//Default constructor
	//sets attributes to 0
	// TODO: commented the next two lines
	//fFixedPoint = 0;
	//fRefPoint = 0;
}


TLSCalcOrientationCnstrObservation::TLSCalcOrientationCnstrObservation(OrieCnstrObsConstIter obs,
									LSPosVecIter fixedPoint, LSPosVecIter refPoint):
TALSCalcObservation(),
fOrieCnstrObs(obs),fFixedPoint(fixedPoint),fRefPoint(refPoint)
{
}


TLSCalcOrientationCnstrObservation::TLSCalcOrientationCnstrObservation(const TLSCalcOrientationCnstrObservation& source) :
TALSCalcObservation(source)
{// copy constructor
	fOrieCnstrObs = source.fOrieCnstrObs;
	fFixedPoint = source.fFixedPoint;
	fRefPoint = source.fRefPoint;
}


TLSCalcOrientationCnstrObservation::~TLSCalcOrientationCnstrObservation()
{// Destructor
}


//////////////////////////////////////////////////////////////////////////////////////////
//PUBLIC MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////
bool	TLSCalcOrientationCnstrObservation::operator==(const TLSCalcOrientationCnstrObservation& right)
{// Overloaded equality operator
	bool isEqualTo = false;
	if (fOrieCnstrObs == right.fOrieCnstrObs)
	{
		isEqualTo = true;
	}
	return isEqualTo;
}

//////////////////////////////////////////////////////////////////////////
//PUBLIC ACCESS METHODS
//////////////////////////////////////////////////////////////////////////
LSPosVecIter TLSCalcOrientationCnstrObservation::getFixedPoint() const
{/*Returns a pointer to the point*/
	return fFixedPoint;
}


LSPosVecIter TLSCalcOrientationCnstrObservation::getRefPoint() const
{/*Returns a pointer to the point*/
	return fRefPoint;
}


TAngle		TLSCalcOrientationCnstrObservation::getBearing() const
{
	return fOrieCnstrObs->getOrieCnstrBearing();
}




TAngle		TLSCalcOrientationCnstrObservation::getSigmaAPriori() const
{
	return fOrieCnstrObs->getSigmaValue();
}



///////////////////////////////////////////////////////////////////////////////////////////
//end
///////////////////////////////////////////////////////////////////////////////////////////