#include "TLSCalcRelativeError.h"


// Default constructor
TLSCalcRelativeError::TLSCalcRelativeError(): 
fPoint1(nullptr),
fPoint2(nullptr)
{
	fSigmaL = TLength(LITERAL(0.0));
	fSigmaR = TLength(LITERAL(0.0));
	fSigmaZ = TLength(LITERAL(0.0));
	fSigmaG = TAngle(LITERAL(0.0));
	fSigmaV = TAngle(LITERAL(0.0));
} 

// constructor taking 2 lscalc pos. vector iterator
TLSCalcRelativeError::TLSCalcRelativeError(TAdjustablePoint& pt1, TAdjustablePoint& pt2) :
fPoint1(&pt1), fPoint2(&pt2) {

	fSigmaL = TLength(LITERAL(0.0));
	fSigmaR = TLength(LITERAL(0.0));
	fSigmaZ = TLength(LITERAL(0.0));
	fSigmaG = TAngle(LITERAL(0.0));
	fSigmaV = TAngle(LITERAL(0.0));
}

// copy constructor
TLSCalcRelativeError::TLSCalcRelativeError(const TLSCalcRelativeError& source) {

	fPoint1 = source.fPoint1;
	fPoint2 = source.fPoint2;

	fSigmaL = source.fSigmaL;
	fSigmaR = source.fSigmaR;
	fSigmaZ = source.fSigmaZ;
	fSigmaG = source.fSigmaG;
	fSigmaV = source.fSigmaV;
}
// destructor
TLSCalcRelativeError::~TLSCalcRelativeError() {
}

/* copy assignment operator: not implemented
TLSCalcRelativeError& TLSCalcRelativeError::operator=(const TLSCalcRelativeError& right) {
}*/

// overloaded equality operator
bool TLSCalcRelativeError::operator==(const TLSCalcRelativeError& right) {

	if ((fPoint1 == right.fPoint1) && (fPoint2 == right.fPoint2))
		return true;
	else
		return false;
}
/////////////////////////
// SETTINGS
////////////////////////

// sets the error in orientation
void TLSCalcRelativeError::setSigmaG(TAngle sg) {

	fSigmaG = sg;
	return;
}
// sets the longitudinal error
void TLSCalcRelativeError::setSigmaL(TLength sl) {

	fSigmaL = sl;
	return;
}
// sets the radial error
void TLSCalcRelativeError::setSigmaR(TLength sr) {

	fSigmaR = sr;
	return;
}

// sets the error in the vertical angle
void TLSCalcRelativeError::setSigmaV(TAngle sv) {

	fSigmaV = sv;
	return;
}

// sets the error in the height difference 
void TLSCalcRelativeError::setSigmaZ(TLength sz) {

	fSigmaZ = sz;
	return;
}

void TLSCalcRelativeError::serialize(SerializerObject::SerializationHelper& obj) const
{
	// Requires modifying SurveyLib
	//obj.addProperty("fPoint1/position vector iterator for the 1st point", fPoint1);
	//obj.addProperty("fPoint2/position vector iterator for the 2nd point", fPoint2);
	obj.addProperty("fSigmaL/longitudinal error", fSigmaL);
	obj.addProperty("fSigmaG/error in orientation", fSigmaG);
	obj.addProperty("fSigmaR/radial (transversal) error", fSigmaR);
	obj.addProperty("fSigmaZ/error in the height difference", fSigmaZ);
	obj.addProperty("fSigmaV/error in the vertical angle", fSigmaV);
}
