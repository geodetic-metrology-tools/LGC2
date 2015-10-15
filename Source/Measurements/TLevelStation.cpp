// TLevelStation.cpp
//
/** Class for a level instrument station */
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
#include  "TLevelStation.h"
////////////////////////////////////////////////////////////////

//ClassImp(TLevelStation)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
//////////////
// Constructor
//////////////
TLevelStation::TLevelStation(TWorkingStations* parent):
fLastVertDistSeriesNbr(0)
{	
	TLevelStation* lst = parent->addLevelStation(this);
}
*/

/////////////////////////////
// Default constructor
////////////////////////
TLevelStation::TLevelStation() {
}

///////////////////
// Copy constructor
///////////////////
// **This needs to be implemented**
TLevelStation::TLevelStation( const  TLevelStation& source ):
TAFreeInstrumentStation(source),
fLastVertDistSeriesNbr(source.fLastVertDistSeriesNbr), 
fMadeVDistROMs(source.fMadeVDistROMs)
{	

}

/////////////
// Destructor
/////////////
TLevelStation::~TLevelStation()
{

}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

// **This needs to be implemented**
///////////////////////////
// Copy Assignment operator
///////////////////////////
TLevelStation&  TLevelStation::operator=(const TLevelStation& right)
{	

	if (this != &right)
	{
		fInstrumentNumber = right.fInstrumentNumber;
		fSetup = right.fSetup;
		fMadeVDistROMs = right.fMadeVDistROMs;
		fLastVertDistSeriesNbr = right.fLastVertDistSeriesNbr;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// returns an iterator pointing to the first vertical dist rom
//////////////////////////////////////////////////////////////
VertDistROMIterator TLevelStation::getVertDistROMBeginIterator() {

	return fMadeVDistROMs.begin();
}

///////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the first vertical dist rom
///////////////////////////////////////////////////////////////////
VertDistROMConstIter TLevelStation::getVertDistROMBeginIterator() const{

	return fMadeVDistROMs.begin();
}

//////////////////////////////////////////////////////////////////////
// returns an iterator pointing to one past the last vertical dist rom
//////////////////////////////////////////////////////////////////////
VertDistROMIterator TLevelStation::getVertDistROMEndIterator() {

	return fMadeVDistROMs.end();
}

///////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to one past the last vertical dist rom
///////////////////////////////////////////////////////////////////////////
VertDistROMConstIter TLevelStation::getVertDistROMEndIterator() const{

	return fMadeVDistROMs.end();
}

///////////////////////////////////////////////////////////////////////////
// returns an iterator to the v. dist ROM referenced from a specified point
///////////////////////////////////////////////////////////////////////////
VertDistROMIterator TLevelStation::getVertDistROM(/*TWorkingPoints::PointIterator*/ TSpatialPointName iterRef) {

	VertDistROMIterator iter = fMadeVDistROMs.begin();
	VertDistROMIterator iterEnd = fMadeVDistROMs.end();

	bool notFound = true;

	while ((iter != iterEnd) && notFound){
		if ((iter->getRefPtName()) == iterRef)
			notFound = false;
		else
			iter++;
	}

	return iter;
}

////////////////////////////////////////////////////////////////////////////////
// returns a const iterator to the v. dist ROM referenced from a specified point
////////////////////////////////////////////////////////////////////////////////
VertDistROMConstIter TLevelStation::getVertDistROM(/*TWorkingPoints::PointIterator*/TSpatialPointName iterRef) const{

	VertDistROMConstIter iter = fMadeVDistROMs.begin();
	VertDistROMConstIter iterEnd = fMadeVDistROMs.end();

	bool notFound = true;

	while ((iter != iterEnd) && notFound){
		if ((iter->getRefPtName()) == iterRef)
			notFound = false;
		else
			iter++;
	}

	return iter;
}

////////////////////////////////////////////////////
// adds a vertical dist round of meas to the station
////////////////////////////////////////////////////
TVerticalDistROM* TLevelStation::addVertDistROM(TVerticalDistROM* vdr){

	//possible definition of the rom's fSeries
	string empty("");
	if ((vdr->getSeries()) == empty){

		fLastVertDistSeriesNbr ++;
		ostringstream oss;
		oss << "LevelVertDistROM" << fLastVertDistSeriesNbr;
		vdr->setSeries(oss.str());
	}

	if (notInContainer(vdr)){

		//insertion in the container
		fMadeVDistROMs.push_back(*vdr);
		return &(*(-- fMadeVDistROMs.end()));
	}
	else{
		cerr << "VertdistROM not inserted : already stored\n";
		return 0;
	}
}

////////////////////////////////////////////////////////////
// tells if a specified ROM is already stored by the station
////////////////////////////////////////////////////////////
bool TLevelStation::notInContainer(TVerticalDistROM* vdr) const{

	VertDistROMConstIter iter = fMadeVDistROMs.begin();
	VertDistROMConstIter iterEnd = fMadeVDistROMs.end();

	bool notPresent = true;

	while ((iter != iterEnd) && notPresent){

		if (*iter == *vdr)
			notPresent = false;
		else
			iter++;
	}

	return notPresent;
}
/*
///////////////////////////////////////////////////////////
// Tells the children ROMs that the station is disactivated
///////////////////////////////////////////////////////////
void TLevelStation::sendDisactivatedToChildren() {

	VertDistROMIterator iter = fMadeVDistROMs.begin();
	VertDistROMIterator iterEnd = fMadeVDistROMs.end();

	while (iter != iterEnd){
		iter->parentDisactivated();
		iter++;
	}
}

////////////////////////////////////////////////////////
// Tells the children ROMs that the station is activated
////////////////////////////////////////////////////////
void TLevelStation::sendActivatedToChildren() {

	VertDistROMIterator iter = fMadeVDistROMs.begin();
	VertDistROMIterator iterEnd = fMadeVDistROMs.end();

	while (iter != iterEnd){
		iter->parentActivated();
		iter++;
	}
}


*/