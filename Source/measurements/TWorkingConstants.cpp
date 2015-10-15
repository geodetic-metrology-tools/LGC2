// TWorkingConstants.cpp: implementation of the TWorkingPoints class.
//




#include "TWorkingConstants.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TWorkingConstants::TWorkingConstants():fLastDistNbr(0), fLastAngleNbr(0)
{
	
}


/*TWorkingConstants::TWorkingConstants(InstNb in, PrismNb	pn):fValue(0.0){
	fInstrumentNumber = in;
	fPrismeNumber = pn;
	fStatus = TANumericValue:: kFixed;
}*/

/*TWorkingConstants::TWorkingConstants(InstNb in, PrismNb	pn, TLength c, TANumericValue::EStatus s){
	fInstrumentNumber = in;
	fPrismeNumber = pn;
	fValue = c;
	fStatus = s;
}*/


TWorkingConstants::~TWorkingConstants()
{

}




/////////////////////////////////////////////////////////////
// Returns the number of Constants stored in the constantsSet
/////////////////////////////////////////////////////////////

int TWorkingConstants::numberOfDistConstants() const{

	return fDistConstants.size();
}


/////////////////////////////////////////////////////////////////
// Returns an iterator on the first element of the fWorkingConstants
/////////////////////////////////////////////////////////////////
DistConstantIterator TWorkingConstants::getDistConstantsBeginIterator() {

	return fDistConstants.begin();
}

/////////////////////////////////////////////////////////////////
// Returns an iterator on the first element of the fWorkingConstants
/////////////////////////////////////////////////////////////////
DistConstantConstIter TWorkingConstants::getDistConstantsBeginIterator() const{

	return fDistConstants.begin();
}

////////////////////////////////////////////////////////////////////
// returns an iterator on one element past the end of fWorkingConstants
////////////////////////////////////////////////////////////////////
DistConstantIterator TWorkingConstants::getDistConstantsEndIterator() {

	return fDistConstants.end();
}

////////////////////////////////////////////////////////////////////
// returns an iterator on one element past the end of fWorkingConstants
////////////////////////////////////////////////////////////////////
DistConstantConstIter TWorkingConstants::getDistConstantsEndIterator() const{

	return fDistConstants.end();
}

//////////////////////////////////////////
// returns an iterator  given the constant
//////////////////////////////////////////
/*DistConstantIterator TWorkingConstants::getDistIterator(TDistConstants cte) {

	DistConstantIterator iter = fDistConstants.begin();
	DistConstantIterator iterEnd = fDistConstants.end();

	bool notFound = true;

	while (iter != iterEnd  &&  notFound){
		if (*iter == cte)
			notFound = false;
		else
			iter ++;
	}

	return iter;
}

//////////////////////////////////////////
// returns an iterator  given the constant
//////////////////////////////////////////
DistConstantConstIter TWorkingConstants::getDistIterator(TDistConstants cte) const{

	DistConstantConstIter iter = fDistConstants.begin();
	DistConstantConstIter iterEnd = fDistConstants.end();

	bool notFound = true;

	while (iter != iterEnd  &&  notFound){
		if (*iter == cte)
			notFound = false;
		else
			iter ++;
	}

	return iter;
}
*/

///////////////////////////////////////////////////////////////////
// returns an iterator on a point, given its TSpatialPointName
///////////////////////////////////////////////////////////////////
DistConstantIterator TWorkingConstants::getDistConstant(const string name) {

	DistConstantIterator iter = fDistConstants.begin();
	DistConstantIterator iterEnd = fDistConstants.end();

	bool notFound = true;

	while (iter != iterEnd  &&  notFound){
		if (iter->getConstantName() == name)
			notFound = false;
		else
			iter ++;
	}

	return iter;
}

///////////////////////////////////////////////////////////////////
// returns an iterator on a point, given its TSpatialPointName
///////////////////////////////////////////////////////////////////
DistConstantConstIter TWorkingConstants::getDistConstant(const string name) const{

	DistConstantConstIter iter = fDistConstants.begin();
	DistConstantConstIter iterEnd = fDistConstants.end();

	bool notFound = true;

	while (iter != iterEnd  &&  notFound){
		if (iter->getConstantName() == name)
			notFound = false;
		else
			iter ++;
	}

	return iter;
}


/////////////////////////////
// Adds a constant to the set
/////////////////////////////
TDistConstants* TWorkingConstants::addDistConstant(TDistConstants *dc){

	//possible attribution of a default fSetup string
	string empty("");
	if (dc->getConstantName() == empty){
		ostringstream oss;
		fLastDistNbr ++;
		oss << "DistInstrConstant" << fLastDistNbr;
		dc->setConstantName(oss.str());
	}

	//insertion in the container
	// Warning : no control on multiple entries
	fDistConstants.push_back(*dc);
	return &(*(--fDistConstants.end()));
	
}


/////////////////////////////////////////////////////////////
// Returns the number of Constants stored in the constantsSet
/////////////////////////////////////////////////////////////

int TWorkingConstants::numberOfAngleConstants() const{

	return fAngleConstants.size();
}


/////////////////////////////////////////////////////////////////
// Returns an iterator on the first element of the fWorkingConstants
/////////////////////////////////////////////////////////////////
AngleConstantIterator TWorkingConstants::getAngleConstantsBeginIterator() {

	return fAngleConstants.begin();
}

/////////////////////////////////////////////////////////////////
// Returns an iterator on the first element of the fWorkingConstants
/////////////////////////////////////////////////////////////////
AngleConstantConstIter TWorkingConstants::getAngleConstantsBeginIterator() const{

	return fAngleConstants.begin();
}

////////////////////////////////////////////////////////////////////
// returns an iterator on one element past the end of fWorkingConstants
////////////////////////////////////////////////////////////////////
AngleConstantIterator TWorkingConstants::getAngleConstantsEndIterator() {

	return fAngleConstants.end();
}

////////////////////////////////////////////////////////////////////
// returns an iterator on one element past the end of fWorkingConstants
////////////////////////////////////////////////////////////////////
AngleConstantConstIter TWorkingConstants::getAngleConstantsEndIterator() const{

	return fAngleConstants.end();
}

///////////////////////////////////////////////////////////////////
// returns an iterator on a point, given its TSpatialPointName
///////////////////////////////////////////////////////////////////
/*AngleConstantIterator TWorkingConstants::getAngleIterator(TAngleConstants cte) {

	AngleConstantIterator iter = fAngleConstants.begin();
	AngleConstantIterator iterEnd = fAngleConstants.end();

	bool notFound = true;

	while (iter != iterEnd  &&  notFound){
		if (*iter == cte)
			notFound = false;
		else
			iter ++;
	}

	return iter;
}

///////////////////////////////////////////////////////////////////
// returns an iterator on a point, given its TSpatialPointName
///////////////////////////////////////////////////////////////////
AngleConstantConstIter TWorkingConstants::getAngleIterator(TAngleConstants cte) const{

	AngleConstantConstIter iter = fAngleConstants.begin();
	AngleConstantConstIter iterEnd = fAngleConstants.end();

	bool notFound = true;

	while (iter != iterEnd  &&  notFound){
		if (*iter == cte)
			notFound = false;
		else
			iter ++;
	}

	return iter;
}
*/

///////////////////////////////////////////////////////////////////
// returns an iterator on a point, given its TSpatialPointName
///////////////////////////////////////////////////////////////////
AngleConstantIterator TWorkingConstants::getAngleConstant(const string name) {

	AngleConstantIterator iter = fAngleConstants.begin();
	AngleConstantIterator iterEnd = fAngleConstants.end();

	bool notFound = true;

	while (iter != iterEnd  &&  notFound){
		if (iter->getConstantName() == name)
			notFound = false;
		else
			iter ++;
	}

	return iter;
}

///////////////////////////////////////////////////////////////////
// returns an iterator on a point, given its TSpatialPointName
///////////////////////////////////////////////////////////////////
AngleConstantConstIter TWorkingConstants::getAngleConstant(const string name) const{

	AngleConstantConstIter iter = fAngleConstants.begin();
	AngleConstantConstIter iterEnd = fAngleConstants.end();

	bool notFound = true;

	while (iter != iterEnd  &&  notFound){
		if (iter->getConstantName() == name)
			notFound = false;
		else
			iter ++;
	}

	return iter;
}



/////////////////////////////
// Adds a constant to the set
/////////////////////////////
TAngleConstants* TWorkingConstants::addAngleConstant(TAngleConstants *ac){

	//possible attribution of a default fSetup string
	string empty("");
	if (ac->getConstantName() == empty){
		ostringstream oss;
		fLastAngleNbr ++;
		oss << "AngInstrConstant" << fLastAngleNbr;
		ac->setConstantName(oss.str());
	}

	//insertion in the container
	// Warning : no control on multiple entries
	fAngleConstants.push_back(*ac);
	return &(*(--fAngleConstants.end()));
	
}

