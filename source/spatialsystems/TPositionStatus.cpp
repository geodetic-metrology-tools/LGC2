// TPositionStatus.cpp
//
/** Class storing the statuses of objects related to a 3D position */
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
#include "TAReferenceFrame.h"

#include  "TPositionStatus.h"
////////////////////////////////////////////////////////////////


//ClassImp(TPositionStatus)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//////////////////////
// default constructor
//////////////////////
TPositionStatus::TPositionStatus( TVReferenceFrame* rf):
fRefFrame(rf)
{	
	fPositionStatus = kPosNull;
	fStatus[0] = TANumericValue::kNull;
	fStatus[1] = TANumericValue::kNull;
	fStatus[2] = TANumericValue::kNull;
}

///////////////////
// copy constructor
///////////////////
TPositionStatus::TPositionStatus( const  TPositionStatus& source ):
fRefFrame(source.fRefFrame), fPositionStatus(source.fPositionStatus)
{	
	fStatus[0] = source.getX1Status();
	fStatus[1] = source.getX2Status();
	fStatus[2] = source.getX3Status();
}

/////////////
// destructor
/////////////
TPositionStatus::~TPositionStatus()
{

}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

///////////////////////////
// Copy Assignment operator
///////////////////////////
TPositionStatus&  TPositionStatus::operator=(const TPositionStatus& source)
{	
	if (this != &source)	{

		fRefFrame = source.fRefFrame;
		fPositionStatus  = source.getPositionStatus();
		fStatus[0] = source.getX1Status();
		fStatus[1] = source.getX2Status();
		fStatus[2] = source.getX3Status();
	}
	return *this;
}

//////////////////////////////////////////////
// returns the 3D Cartesian coordinates status
//////////////////////////////////////////////
TPositionStatus::EPositionStatus TPositionStatus::getPositionStatus() const{

	//return fRefFrame->getPositionStatus( this );
	return kCala;
}



//////////////////////////////////////////
// gets the number of variable coordinates
//////////////////////////////////////////
int TPositionStatus::getVariableDimension() const{

	switch(fPositionStatus){

	case kCala:
	case kPosNull:
	case kUnknown:
		return 0;
		break;

	case kVx:
	case kVy:
	case kVz:
		return 1;
		break;

	case kVxy:
	case kVxz:
	case kVyz:
		return 2;
		break;

	case kVxyz:
		return 3;
		break;

	default:
		return 0;
		break;
	}
}

/////////////////////////////////////////////
// gets the status of one of the X coordinate
/////////////////////////////////////////////
TPositionStatus::ECoordStatus TPositionStatus::getXCoordStatus() const{

	//return  fRefFrame->getXCoordStatus( this );
	return TANumericValue::kFixed;
}

/////////////////////////////////////////////
// gets the status of one of the Y coordinate
/////////////////////////////////////////////
TPositionStatus::ECoordStatus TPositionStatus::getYCoordStatus() const{

	//return  fRefFrame->getYCoordStatus( this );
	return TANumericValue::kFixed;
}

/////////////////////////////////////////////
// gets the status of one of the Z coordinate
/////////////////////////////////////////////
TPositionStatus::ECoordStatus TPositionStatus::getZCoordStatus() const{

	//return  fRefFrame->getZCoordStatus( this );
	return TANumericValue::kFixed;
}

///////////////////////////////////////////
// sets the 3D Cartesian coordinates status
///////////////////////////////////////////
bool TPositionStatus::setPositionStatus(EPositionStatus ps){

	//return fRefFrame->setPositionStatus(this, ps);
	return false;
}


//////////////////////////////////////
// sets the status of the X coordinate
//////////////////////////////////////
bool TPositionStatus::setXCoordStatus(ECoordStatus s){

	//return  fRefFrame->setXCoordStatus( this, s );
	return false;
}

//////////////////////////////////////
// sets the status of the Y coordinate
//////////////////////////////////////
bool TPositionStatus::setYCoordStatus(ECoordStatus s){

	//return  fRefFrame->setYCoordStatus( this, s );
	return false;
}

//////////////////////////////////////
// sets the status of the Z coordinate
//////////////////////////////////////
bool TPositionStatus::setZCoordStatus(ECoordStatus s){

	//return  fRefFrame->setZCoordStatus( this, s );
	return false;
}