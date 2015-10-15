//
//TASpatialPtListener.cpp : implementation file
// abstract base class defining the default implementation
// of the method of the TVSpatialPtListener interface
//////////////////////////////////////////////////////////

#include "TASpatialPtListener.h"

///////////////////////////
// no arguments constructor
///////////////////////////
TASpatialPtListener::TASpatialPtListener(){

}

/////////////
// destructor
/////////////
TASpatialPtListener::~TASpatialPtListener(){

}

////////////////////////////////////////////
// default response to a point disactivation
////////////////////////////////////////////
void TASpatialPtListener::pointDisactivated(){

}

///////////////////////////////////////////
// default response to a point reactivation
///////////////////////////////////////////
void TASpatialPtListener::pointActivated(){

}

////////////////////////////////////////////
// default response to a point update
////////////////////////////////////////////
void TASpatialPtListener::pointUpdated(EUpdateMessage umess){

	switch (umess){

	case kNameChange:
		break;

	case kPositionChange:
		break;

	case kStatusChange:
		break;

	default:
		break;
	}
}






