// TSpatialStatus.cpp
//
/** Class storing the statuses of spatial objects */
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

#include  "TSpatialStatus.h"
////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TSpatialStatus::TSpatialStatus()
{// default constructor	
	fSpatialStatus = kPosNull;
	fStatus[0] = TANumericValue::kNull;
	fStatus[1] = TANumericValue::kNull;
	fStatus[2] = TANumericValue::kNull;
}


TSpatialStatus::TSpatialStatus( const  TSpatialStatus& source ):
fSpatialStatus(source.fSpatialStatus)
{// copy constructor	
	fStatus[0] = source.getXAxisStatus();
	fStatus[1] = source.getYAxisStatus();
	fStatus[2] = source.getZAxisStatus();
}


TSpatialStatus::~TSpatialStatus()
{// destructor
}


//////////////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////
TSpatialStatus&  TSpatialStatus::operator=(const TSpatialStatus& source)
{// Copy Assignment operator	

	if (this != &source)
	{
		//fRefFrame = source.fRefFrame;
		fSpatialStatus  = source.getObjectStatus();
		fStatus[0] = source.getXAxisStatus();
		fStatus[1] = source.getYAxisStatus();
		fStatus[2] = source.getZAxisStatus();
	}
	return *this;
}



int TSpatialStatus::getVariableDimension() const
{// gets the number of variable coordinates

	switch(fSpatialStatus){

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


//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////
