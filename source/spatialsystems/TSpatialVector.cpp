// TSpatialVector.cpp
//
/** 3D Vector used as a Spatial Position for stations or as translation vector in Helmert transformation */
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
#include  "TAReferenceFrame.h"
#include  "TSpatialVector.h"
#include  "TAngle.h"
#include <math.h>

////////////////////////////////////////////////////////////////


//ClassImp(TSpatialVector)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TSpatialVector::TSpatialVector(TAReferenceFrame* frame)
{
	fFv = new TFreeVector(TCoordSysFactory::k3DCartesian);
	setRefFrame(frame);
	setObjectStatus(TSpatialStatus::kPosNull);
}


TSpatialVector::TSpatialVector(TAReferenceFrame* rf,
							   const TReal x, const TReal y, const TReal z,
							   TCoordSysFactory::ECoordSys cs )
{
	fFv = new TFreeVector(x,y,z,cs);
	setRefFrame(rf);
	setObjectStatus(TSpatialStatus::kCala);
}

TSpatialVector::~TSpatialVector()
{//destructor
	if(fFv != 0)
	{
		delete fFv;
	}
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////



TSpatialVector&  TSpatialVector::operator=( const TSpatialVector& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		if(fFv != 0)
		{
			delete fFv;
			fFv = 0;
		}

		fFv = new TFreeVector(*(right.fFv));
		if(fFv == 0)
		{
			setObjectStatus(TSpatialStatus::kPosNull);
		}
		else
		{
			setObjectStatus(right.getObjectStatus());
		}
		setRefFrame(right.getRefFrame());
	}
	return *this;
}




TSpatialVector& TSpatialVector::operator+( const TSpatialVector& second)
{	/// Plus Operator
	*fFv = *fFv + (second.getVector());
	return *this;
}


TSpatialVector& TSpatialVector::operator-( const TSpatialVector& second)
{	/// Minus Operator
	*fFv = *fFv -  (second.getVector());
	return *this;
}


TSpatialVector& TSpatialVector::operator*( const TReal factor)
{	/// Multiplication by a constant
	*fFv = *fFv *  factor;
	return *this;
}


TFreeVector TSpatialVector::getElements(TCoordSysFactory::ECoordSys coordsys) const
{//Get the coordinates as a free vector: public method
	return getRefFrame()->getElements(this, coordsys);
}


bool TSpatialVector::setElements(const TFreeVector& fv)
{//Set the coordinates as a free vector: public method
	return getRefFrame()->setElements(this, fv);
}



/////////////////////////////////////////////////////////////////////
// Protected Methods
/////////////////////////////////////////////////////////////////////
bool TSpatialVector::setVector(const TFreeVector& fv)
{ // Set the coordinates as a free vector
	if(fFv != 0)
	{
		delete fFv;
		fFv = 0;
	}
	fFv = new TFreeVector(fv);
	return true;
}


bool TSpatialVector::transform(TAReferenceFrame* to)
{// transformation between 2 RF
	if ( this->getRefFrame() == to )
	{ return true; }

	else
	{ return this->getRefFrame()->transform( this, to ); }
	
}


void TSpatialVector::changeRefFrameTo(TAReferenceFrame* rf)
{// change the reference frame
	setRefFrame(rf);
}


/////////////////////////////////////////////////////////////////////
//END
/////////////////////////////////////////////////////////////////////
