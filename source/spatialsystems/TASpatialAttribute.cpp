//  TASpatialAttribute.cpp
//
/**  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "TASpatialAttribute.h"
////////////////////////////////////////////////////////////////


//ClassImp(##Template##)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TASpatialAttribute::TASpatialAttribute() : fRefFrame(0)
{	// default constructor
	TSpatialStatus* ps = new TSpatialStatus();
	counted_ptr< TSpatialStatus >* tempPtr = new counted_ptr< TSpatialStatus >(ps);
	fSpatialStatus = *tempPtr;

	delete tempPtr;

}


TASpatialAttribute::TASpatialAttribute( TAReferenceFrame* rf )
{
	fRefFrame = rf;

	TSpatialStatus* ps = new TSpatialStatus();
	counted_ptr<TSpatialStatus>* tempPtr = new counted_ptr<TSpatialStatus>(ps);
	fSpatialStatus = *tempPtr;
	delete tempPtr;
}


TASpatialAttribute::TASpatialAttribute( TAReferenceFrame* rf, TSpatialStatus* status)
{
	fRefFrame = rf;

	counted_ptr<TSpatialStatus>* tempPtr = new counted_ptr<TSpatialStatus>(status);
	fSpatialStatus = *tempPtr;
	delete tempPtr;
}


TASpatialAttribute::TASpatialAttribute( TAReferenceFrame* rf, counted_ptr<TSpatialStatus> cp )
{
	fRefFrame = rf;
	fSpatialStatus = cp;
}

TASpatialAttribute::~TASpatialAttribute()
{//destructor
}




//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TASpatialAttribute&  TASpatialAttribute::operator=(const TASpatialAttribute& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		fSpatialStatus = right.fSpatialStatus;
		fRefFrame = right.fRefFrame;
	}
	return *this;
}


bool TASpatialAttribute::knownXCoord() const
{// checks if the X coordinate is known

	if ( this->getAttribute()->getXAxisStatus() != TANumericValue::kNull )
		return true;
	else
		return false;
}


bool TASpatialAttribute::knownYCoord() const
{// checks if the Y coordinate is known
	if ( this->getAttribute()->getYAxisStatus() != TANumericValue::kNull )
		return true;
	else
		return false;
}


bool TASpatialAttribute::knownZCoord() const
{// checks if the Z coordinate is known
	if ( this->getAttribute()->getZAxisStatus() != TANumericValue::kNull )
		return true;
	else
		return false;
}


	

//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////

