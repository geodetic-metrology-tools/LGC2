// TSpatialOrientation.cpp
//
/** Class used for a spatial orientation for RF or stations */
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
#include  "TAngle.h"
//#include  "TRotationMatrix.h"
#include  "TSpatialOrientation.h"
////////////////////////////////////////////////////////////////

//ClassImp(TSpatialOrientation)



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TSpatialOrientation::TSpatialOrientation(TAReferenceFrame* frame)
{// default constructor
	fRotationMatrix =  new TRotationMatrix();
	setRefFrame(frame);
	setObjectStatus(TSpatialStatus::kPosNull);

}



TSpatialOrientation::TSpatialOrientation(TRotationMatrix::ERotationType kR,
										 TAngle& om, TAngle& p, TAngle& k,
										 TAReferenceFrame* frame,
										 TCoordSysFactory::ECoordSys cs)
{//Constructor takink the angles in the order omega-phi-kappa
	ignoring(cs);
	TReal omega, phi, kappa;
	omega = om.getRadiansValue();
	phi = p.getRadiansValue();
	kappa = k.getRadiansValue();

	fRotationMatrix = new TRotationMatrix(kR, omega, phi, kappa);
	if(fRotationMatrix == 0)
	{
		// object not created properly
		// reagi
		setObjectStatus(TSpatialStatus::kPosNull);
	}
	else
	{
		setObjectStatus(TSpatialStatus::kCala);
	}

	setRefFrame(frame);
	
}



TSpatialOrientation::TSpatialOrientation(TRotationMatrix::ERotationType kR,
										 TReal& om, TReal& p, TReal& k,
										 TAReferenceFrame* frame, TCoordSysFactory::ECoordSys cs)
{
	ignoring(cs);
	fRotationMatrix =  new TRotationMatrix(kR, om, p, k);
	if(fRotationMatrix == 0)
	{
		// object not created properly
		// reagi
		setObjectStatus(TSpatialStatus::kPosNull);
	}
	else
	{	
		setObjectStatus(TSpatialStatus::kCala);
	}

	setRefFrame(frame);
}


TSpatialOrientation::TSpatialOrientation(const TRotationMatrix matrix, TAReferenceFrame* frame)
{
	setRefFrame(frame);
	setObjectStatus(TSpatialStatus::kCala);
	fRotationMatrix = new TRotationMatrix (matrix);

}


TSpatialOrientation::TSpatialOrientation( const  TSpatialOrientation& original )
{	// copy constructor
	fRotationMatrix =0;
	*this = original;
}


TSpatialOrientation::~TSpatialOrientation()
{//destructor
	if(fRotationMatrix != 0)
	{
		delete fRotationMatrix ;
	}
}




//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TSpatialOrientation&  TSpatialOrientation::operator=(const TSpatialOrientation& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		if(fRotationMatrix != 0)
		{
			delete fRotationMatrix ;
			fRotationMatrix = 0;
		}

		fRotationMatrix = new TRotationMatrix(*(right.fRotationMatrix));
		if(fRotationMatrix == 0)
		{
			// object not created properly
			// reagi
			setObjectStatus(TSpatialStatus::kPosNull);
		}
		else
		{
			setRefFrame(right.getRefFrame());
			setObjectStatus(right.getObjectStatus());
		}
	}
	return *this;
}





//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

///////////////////
// Access Methods
///////////////////
// Return the rotation matrix of the orientation: public method
TRotationMatrix TSpatialOrientation::getElements(TCoordSysFactory::ECoordSys coordsys) const
{
	return getRefFrame()->getElements( this, coordsys );
}

///////////////////
// Settings
///////////////
// set the rotation matrix : public method
bool TSpatialOrientation::setElements(const TRotationMatrix& rm)
{
	return getRefFrame()->setElements(this, rm);
}

// Set the rotation matrix : private method
bool TSpatialOrientation::setOrientation(const TRotationMatrix& rm)
{
	if(fRotationMatrix != 0)
	{
		delete fRotationMatrix ;
		fRotationMatrix = 0;
	}

	fRotationMatrix = new TRotationMatrix(rm);
	if(fRotationMatrix == 0)
	{
		// object not created properly
		// reagi
		setObjectStatus(TSpatialStatus::kPosNull);
	}

	return true;
}






TRotationMatrix TSpatialOrientation::getOrientation() const
{	
	TRotationMatrix rm(*fRotationMatrix);
	return rm;
}


