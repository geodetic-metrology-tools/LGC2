//TALS2DOffMeasContribGenerator.cpp : implementation file
//Base Class for a LS contrib generator processing 2D offset measurements


#include "TLSInputMatrices.h"

#include "TALS2DOffMeasContribGenerator.h"

//////////////////////////
// no argument constructor
//////////////////////////
TALS2DOffMeasContribGenerator::TALS2DOffMeasContribGenerator(){

	fInputMatrices = 0;
}

////////////////////////////////////////////////////////
// constructor setting the pointer to the input matrices
////////////////////////////////////////////////////////
TALS2DOffMeasContribGenerator::TALS2DOffMeasContribGenerator(TLSInputMatrices* lsim){

	fInputMatrices = lsim;
	fS0APrioriScaleFactor = fInputMatrices->getS0APrioriScaleFactor();
}

///////////////////
// copy constructor
///////////////////
TALS2DOffMeasContribGenerator::TALS2DOffMeasContribGenerator(const TALS2DOffMeasContribGenerator& source):
fInputMatrices(source.fInputMatrices),
fS0APrioriScaleFactor(source.fS0APrioriScaleFactor),
fCurrentStation(source.fCurrentStation), fCurrentROM(source.fCurrentROM){

}

	

/////////////
// destructor
/////////////
TALS2DOffMeasContribGenerator::~TALS2DOffMeasContribGenerator(){

}


//////////////////////////////////////////////
// sets the iterator to the current dist meas station
//////////////////////////////////////////////
void TALS2DOffMeasContribGenerator::setCurrentDistStation(TLSCalcWorkingStations::CalcDistStIterator dst){

	fCurrentStation = dst;
}

////////////////////////////////////////////////
// sets the pointer to the current 2D offset ROM
////////////////////////////////////////////////
void TALS2DOffMeasContribGenerator::setCurrent2DOffsetROM(TLSCalcDistMeasStation::Calc2DOffsetROMIterator or){

	fCurrentROM = or;
}