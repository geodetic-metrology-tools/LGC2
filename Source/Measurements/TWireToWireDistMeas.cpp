// TWireToWireDistMeas.cpp
//
/** Class representing a wire to wire distance measurement*/
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
//#include "TWorkingStations.h"
#include "TWTWDistStation.h"

#include  "TWireToWireDistMeas.h"
////////////////////////////////////////////////////////////////


//ClassImp(TWireToWireDistMeas)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//////////////////////
// Default constructor
//////////////////////
TWireToWireDistMeas::TWireToWireDistMeas():
/*fFirstWire(0), fSecondWire(0),*/ fPositionOnWire1(0), fPositionOnWire2(0),
fHDist(0), fHSigma(0), fVDist(0), fVSigma(0), fWire1Name(""), fWire2Name("")
/* fHUsedInCalc(false), fVUsedInCalc(false), fParentStation(0),
fWire1End1Name(""), fWire1End2Name(""), fWire2End1Name(""), fWire2End2Name("")*/{

}
/*
//////////////
// Constructor
//////////////
TWireToWireDistMeas::TWireToWireDistMeas(TWTWDistStation& parent, TWorkingTargetObjects::WireIterator wire1, TLength wire1Pos, 
										 TWorkingTargetObjects::WireIterator wire2, TLength wire2Pos, 
										 TLength hDist, TLength hSigma, TLength vDist, TLength vSigma):
fParentStation(&parent), fFirstWire(wire1), fPositionOnWire1(wire1Pos), 
fSecondWire(wire2), fPositionOnWire2(wire2Pos), 
fHDist(hDist), fHSigma(hSigma), fVDist(vDist), fVSigma(vSigma), 
fHUsedInCalc(true), fVUsedInCalc(true), 
fWire1End1Name(""), fWire1End2Name(""),
fWire2End1Name(""), fWire2End2Name("")
{	

	TWireToWireDistMeas* wtwm = fParentStation->setWTWDistMeas(this);
	fFirstWire->addListener(static_cast<TVMeasurableEntityLstnr*>(wtwm));
	fSecondWire->addListener(static_cast<TVMeasurableEntityLstnr*>(wtwm));
	
}
*/
//////////////
// Constructor
//////////////
TWireToWireDistMeas::TWireToWireDistMeas( TLength wire1Pos, TLength wire2Pos, string w1, string w2,
			TLength hDist, TLength hSigma, TLength vDist, TLength vSigma /*TSpatialPointName w1End1Name, TSpatialPointName w1End2Name, TSpatialPointName w2End1Name, TSpatialPointName w2End2Name*/):
 fPositionOnWire1(wire1Pos), fPositionOnWire2(wire2Pos), fWire1Name(w1), fWire2Name(w2),
fHDist(hDist), fHSigma(hSigma), fVDist(vDist), fVSigma(vSigma)
/*fHUsedInCalc(true), fVUsedInCalc(true), fWire1End1Name(w1End1Name), fWire1End2Name(w1End2Name), */
/*fFirstWire(0), fSecondWire(0), fWire2End1Name(w2End1Name), fWire2End2Name(w2End2Name)*/
{

}


///////////////////
// copy constructor
///////////////////
TWireToWireDistMeas::TWireToWireDistMeas( const  TWireToWireDistMeas& source ):
TAObjectMeasurement(source), fPositionOnWire1(source.getWire1Pos()),
fPositionOnWire2(source.getWire2Pos()), fWire1Name(source.fWire1Name), fWire2Name(source.fWire2Name),
fHDist(source.getHDist()), fHSigma(source.getHSigma()), fVDist(source.getVDist()), fVSigma(source.getVSigma())
/*fHUsedInCalc(source.hActive()), fVUsedInCalc(source.vActive()) */
/*fFirstWire(source.getFirstWire()), fSecondWire(source.getSecondWire())*/
/*fWire1End1Name(source.fWire1End1Name), fWire1End2Name(source.fWire1End2Name),
fWire2End1Name(source.fWire2End1Name), fWire2End2Name(source.fWire2End2Name)*/
{	

}

/////////////
// destructor
/////////////
TWireToWireDistMeas::~TWireToWireDistMeas()
{

}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

////////////////////////////
// copy assignement operator
////////////////////////////
TWireToWireDistMeas&  TWireToWireDistMeas::operator=(const TWireToWireDistMeas& source)
{	
	if (this != &source){

/*		fWire1End1Name = source.fWire1End1Name;
		fWire1End2Name = source.fWire1End2Name;
		fWire2End1Name = source.fWire2End1Name;
		fWire2End2Name = source.fWire2End2Name;
*/
		fWire1Name = source.fWire1Name;
		fWire2Name = source.fWire2Name;
		fPositionOnWire1 = source.getWire1Pos();
		fPositionOnWire2 = source.getWire2Pos();

		fHDist = source.getHDist();
		fHSigma = source.getHSigma();
		fVDist = source.getVDist();
		fVSigma = source.getVSigma(); 
//		fHUsedInCalc = source.hActive();
//		fVUsedInCalc = source.vActive(); 
//		fFirstWire = source.getFirstWire();
//		fSecondWire = source.getSecondWire();
	}

	return *this;
}

////////////////////
// Equality operator
////////////////////
bool TWireToWireDistMeas::operator==(const TWireToWireDistMeas& right) const{

/*	if ((fFirstWire == right.getFirstWire())
		&& (fSecondWire == right.getSecondWire())
		&& (fPositionOnWire1 == right.getWire1Pos())
		&& (fPositionOnWire2 == right.getWire2Pos()))
		return true;
	else
		return false;*/
	if ((fWire1Name == (right.fWire1Name)) 
		&& (fWire2Name == (right.fWire2Name)))
		return true;
	else
		return false;
}



///////////////////////////////////////////////////////
// returns the string representing the measurement kind
///////////////////////////////////////////////////////
string TWireToWireDistMeas::getMeasKind() const{

	string s("WTWDist");
	return s;
}

//////////////////////////////////////////////////////////////
// sets the horizontal sigma zero a priori if it is still zero
//////////////////////////////////////////////////////////////
void TWireToWireDistMeas::ifNotDoneSetHSigma(TLength sigma){

	if (fHSigma.getMetresValue() == LITERAL(0.0))
		fHSigma = sigma;
}

////////////////////////////////////////////////////////////
// sets the vertical sigma zero a priori if it is still zero
////////////////////////////////////////////////////////////
void TWireToWireDistMeas::ifNotDoneSetVSigma(TLength sigma){

	if (fVSigma.getMetresValue() == LITERAL(0.0))
		fVSigma = sigma;
}

//////////////////////
// get wire1's name
//////////////////////
string TWireToWireDistMeas::getWire1Name() const{

	return fWire1Name;
}

/////////////////////
// get wire2's name
/////////////////////
string TWireToWireDistMeas::getWire2Name() const{

	return fWire2Name;

}

//////////////////////
// sets wire1's name
//////////////////////
void TWireToWireDistMeas::setWire1Name(string w1) {

	fWire1Name = w1;
}
//////////////////////
// sets wire2's name
//////////////////////
void TWireToWireDistMeas::setWire2Name(string w2) {

	fWire2Name = w2;
}
/*
/////////////////////////////////////////////////////////////////
// tells the parent station that this measurement is disactivated
/////////////////////////////////////////////////////////////////
void TWireToWireDistMeas::sendDisactivatedToParent() {

	fParentStation->childDisactivated(measurementsCount(), equationsCount(), unknownsCount());
}

//////////////////////////////////////////////////////////////
// tells the parent station that this measurement is activated
//////////////////////////////////////////////////////////////
void TWireToWireDistMeas::sendActivatedToParent() {

	fParentStation->childActivated(measurementsCount(), equationsCount(), unknownsCount());
}

///////////////////////////////////////////////////////////
// sets the iterators to the two wires from the point names
///////////////////////////////////////////////////////////
bool TWireToWireDistMeas::setWires(TWorkingPoints* wp, TWorkingTargetObjects* wto){

	return ((setFirstWire(wp, wto)) && (setSecondWire(wp, wto)));
}

///////////////////////////////////////////////////////////
// sets the iterator to the first wire from the point names
///////////////////////////////////////////////////////////
bool TWireToWireDistMeas::setFirstWire(TWorkingPoints* wp, TWorkingTargetObjects* wto){

	//check if the two end points exist
	TWorkingPoints::PointIterator noSuchPoint = wp->getPointsEndIterator();

	TWorkingPoints::PointIterator iterPt1 = wp->getPoint(fWire1End1Name);
	TWorkingPoints::PointIterator iterPt2 = wp->getPoint(fWire1End2Name);

	if ((iterPt1 == noSuchPoint) || (iterPt1 == noSuchPoint))
		return false;

	///get the wire or create it
	else{

		TWorkingTargetObjects::WireIterator iterW = wto->getWire(iterPt1, iterPt2);
		TWorkingTargetObjects::WireIterator noSuchWire = wto->getWireEndIterator();

		//case where the wire already exists
		if (iterW != noSuchWire){
			fFirstWire = iterW;
			return true;
		}
		//case where the wire has to be created
		else{
			TWire* newWire = new TWire(iterPt1, iterPt2, "");
			wto->addWire(newWire);
			delete newWire;

			fFirstWire = wto->getWire(iterPt1, iterPt2);
			if (fFirstWire != noSuchWire)
				return true;
			else
				return false;
		}
	}
}


////////////////////////////////////////////////////////////
// sets the iterator to the second wire from the point names
////////////////////////////////////////////////////////////
bool TWireToWireDistMeas::setSecondWire(TWorkingPoints* wp, TWorkingTargetObjects* wto){

	//check if the two end points exist
	TWorkingPoints::PointIterator noSuchPoint = wp->getPointsEndIterator();

	TWorkingPoints::PointIterator iterPt1 = wp->getPoint(fWire2End1Name);
	TWorkingPoints::PointIterator iterPt2 = wp->getPoint(fWire2End2Name);

	if ((iterPt1 == noSuchPoint) || (iterPt1 == noSuchPoint))
		return false;

	///get the wire or create it
	else{

		TWorkingTargetObjects::WireIterator iterW = wto->getWire(iterPt1, iterPt2);
		TWorkingTargetObjects::WireIterator noSuchWire = wto->getWireEndIterator();

		//case where the wire already exists
		if (iterW != noSuchWire){
			fSecondWire = iterW;
			return true;
		}
		//case where the wire has to be created
		else{
			TWire* newWire = new TWire(iterPt1, iterPt2, "");
			wto->addWire(newWire);
			delete newWire;

			fSecondWire = wto->getWire(iterPt1, iterPt2);
			if (fSecondWire != noSuchWire)
				return true;
			else
				return false;
		}
	}
}


//////////////////////////////////////////////////////////////
// Creates the WTWDistStation associated with this measurement
//////////////////////////////////////////////////////////////
bool	TWireToWireDistMeas::addToStation(TWorkingStations* ws){

	TWTWDistStation* newWTWSt = new TWTWDistStation(ws);
	delete newWTWSt;

	TWorkingStations::WTWStIterator iterWTWSt = --(ws->getWTWStEndIterator());
	TWireToWireDistMeas* wtwdm = iterWTWSt->setWTWDistMeas(this);
	fFirstWire->addListener(static_cast<TVMeasurableEntityLstnr*>(wtwdm));
	fSecondWire->addListener(static_cast<TVMeasurableEntityLstnr*>(wtwdm));

	return true;
}

////////////////////////////////////////////////
// activates the horizontal distance measurement
////////////////////////////////////////////////
void TWireToWireDistMeas::activateHDist(){

	fHUsedInCalc = true;
}

///////////////////////////////////////////////////
// disactivates the horizontal distance measurement
///////////////////////////////////////////////////
void TWireToWireDistMeas::disactivateHDist(){

	fHUsedInCalc = false;
}

////////////////////////////////////////////////
// activates the vertical distance measurement
////////////////////////////////////////////////
void TWireToWireDistMeas::activateVDist(){

	fVUsedInCalc = true;
}

///////////////////////////////////////////////////
// disactivates the vertical distance measurement
///////////////////////////////////////////////////
void TWireToWireDistMeas::disactivateVDist(){

	fVUsedInCalc = false;
}

*/
