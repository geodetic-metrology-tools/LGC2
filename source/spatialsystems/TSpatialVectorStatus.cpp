// TSpatialVectorStatus.cpp
//
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
#include  "TSpatialVectorStatus.h"

////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//////////////////////
// default constructor
//////////////////////
TSpatialVectorStatus::TSpatialVectorStatus( TVReferenceFrame* rf):
fRefFrame(rf)
{	
	fVectorStatus = kPosNull;
	fStatus[0] = TANumericValue::kNull;
	fStatus[1] = TANumericValue::kNull;
	fStatus[2] = TANumericValue::kNull;
}

///////////////////
// copy constructor
///////////////////
TSpatialVectorStatus::TSpatialVectorStatus( const  TSpatialVectorStatus& source ):
fRefFrame(source.fRefFrame), fVectorStatus(source.fVectorStatus)
{	
	fStatus[0] = source.getF1Status();
	fStatus[1] = source.getF2Status();
	fStatus[2] = source.getF3Status();
}

/////////////
// destructor
/////////////
TSpatialVectorStatus::~TSpatialVectorStatus()
{

}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

///////////////////////////
// Copy Assignment operator
///////////////////////////
TSpatialVectorStatus&  TSpatialVectorStatus::operator=(const TSpatialVectorStatus& source)
{	
	if (this != &source)	{

		fRefFrame = source.fRefFrame;
//		fVectorStatus  = source.getSpatialVectorStatus();
		fStatus[0] = source.getF1Status();
		fStatus[1] = source.getF2Status();
		fStatus[2] = source.getF3Status();
	}
	return *this;
}

//////////////////////////////////////////////
// returns the 3D Cartesian coordinates status
//////////////////////////////////////////////
/*TSpatialVectorStatus::EVectorStatus TSpatialVectorStatus::getSpatialVectorStatus() const{

	return fRefFrame->getSpatialVectorStatus( this );
}
*/


//////////////////////////////////////////
// gets the number of variable coordinates
//////////////////////////////////////////
int TSpatialVectorStatus::getVariableDimension() const{

	switch(fVectorStatus){

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
TSpatialVectorStatus::ECompStatus TSpatialVectorStatus::getXCompStatus() const{

	return  fRefFrame->getXCompStatus( this );
}

/////////////////////////////////////////////
// gets the status of one of the Y coordinate
/////////////////////////////////////////////
TSpatialVectorStatus::ECompStatus TSpatialVectorStatus::getYCompStatus() const{

	return  fRefFrame->getYCompStatus( this );
}

/////////////////////////////////////////////
// gets the status of one of the Z coordinate
/////////////////////////////////////////////
TSpatialVectorStatus::ECompStatus TSpatialVectorStatus::getZCompStatus() const{

	return  fRefFrame->getZCompStatus( this );
}

///////////////////////////////////////////
// sets the 3D Cartesian coordinates status
///////////////////////////////////////////
/*bool TSpatialVectorStatus::setSpatialVectorStatus(EVectorStatus ps){

	return fRefFrame->setSpatialVectorStatus(this, ps);
}
*/

//////////////////////////////////////
// sets the status of the X coordinate
//////////////////////////////////////
bool TSpatialVectorStatus::setXCompStatus(ECompStatus s){

	return  fRefFrame->setXCompStatus( this, s );
}

//////////////////////////////////////
// sets the status of the Y coordinate
//////////////////////////////////////
bool TSpatialVectorStatus::setYCompStatus(ECompStatus s){

	return  fRefFrame->setYCompStatus( this, s );
}

//////////////////////////////////////
// sets the status of the Z coordinate
//////////////////////////////////////
bool TSpatialVectorStatus::setZCompStatus(ECompStatus s){

	return  fRefFrame->setZCompStatus( this, s );
}