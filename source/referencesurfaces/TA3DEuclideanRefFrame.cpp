// TA3DEuclideanRefFrame.cpp
//
/** An abstract base class for a reference frame. 
  
    Patterns:
    An Abstract STATE of a Spatial Position
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
// other forward declarations
////////////////////////////////////////////////////////////////


//#include  "TVTransformation.h"
//#include  "TTransformWrapper.h"
#include	"TA3DEuclideanRefFrame.h"
//ClassImp(TAReferenceFrame)
#include "TSpatialPosition.h"
#include "TPositionStatus.h"
#include "TSpatialPositionError.h"


#include "TSpatialVectorStatus.h"
#include "TSpatialVectorError.h"

#include "TGeodeticRefFrame.h"
#include "TModifiedLocalGeodeticRF.h"
//////////////////////////////////////////////////////////////////////
// Initialisation
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


TA3DEuclideanRefFrame::TA3DEuclideanRefFrame( const string& name )
	: TAReferenceFrame ( name )
{	// default constructor 
}


//TAReferenceFrame::TAReferenceFrame(const TAReferenceFrame& trf)
//{	// copy constructor 
//}


TA3DEuclideanRefFrame::~TA3DEuclideanRefFrame()
{
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

//Default code for accessing the position vector of a spatial position 
//see definition of TSpatialPosition
TPositionVector TA3DEuclideanRefFrame::getCoordinates(const TSpatialPosition* sp, TCoordSysFactory::ECoordSys coordsys) const
{
	if(coordsys == TCoordSysFactory::k3DCartesian) {
		return getPositionVector(sp);
	}

	else 
	{ 
		TPositionVector pv(TCoordSysFactory::k3DCartesian);
		return pv;
	}

}


//Default code for accessing the free vector of a spatial vector 
//see definition of TSpatialVector
TFreeVector TA3DEuclideanRefFrame::getElements(const TSpatialVector* sv, TCoordSysFactory::ECoordSys coordsys) const
{
	TFreeVector fv(TCoordSysFactory::k3DCartesian);
	if(coordsys == TCoordSysFactory::k3DCartesian) {
		fv = getVector(sv);
	}
	return fv;
}


//Default code for accessing the rotation matrix of a spatial orientation 
//see definition of TSpatialOrientation
TRotationMatrix TA3DEuclideanRefFrame::getElements(const TSpatialOrientation* so, TCoordSysFactory::ECoordSys coordsys) const
{
	if(coordsys == TCoordSysFactory::k3DCartesian)
	{
		return getOrientation(so);
	}
	else 
	{ 
		TRotationMatrix rm;
		return rm;
	}
}


//Default code for setting the position vector of a spatial position 
//see definition of TSpatialPosition
bool TA3DEuclideanRefFrame::setCoordinates(TSpatialPosition* sp, const TPositionVector& pv)
{
	if(pv.getCoordSys() == TCoordSysFactory::k3DCartesian)
	{
		return setPositionVector(sp, pv);
	}

	else return false;
	
}


//Default code for setting the free vector of a spatial vector 
//see definition of TSpatialVector
bool TA3DEuclideanRefFrame::setElements(TSpatialVector* sv, const TFreeVector& fv)
{
	bool result;
	result = setVector(sv, fv);
	return result;
}


//Default code for setting the rotation matrix of a spatial orientation 
//see definition of TSpatialOrientation
bool TA3DEuclideanRefFrame::setElements(TSpatialOrientation* so, const TRotationMatrix& rm)
{
	bool result;
	result = setOrientation(so, rm);
	return result;
}



/*
// Equivalence Operator
bool operator==( const TVReferenceFrame& left, const TVReferenceFrame& right )
{
	if( left.getName() == right.getName() )
		return true;
	else
		return false;
}


// Less Than Comparison Operator
bool operator<( const TVReferenceFrame& left, const TVReferenceFrame& right )
{
	if( left.getName() < right.getName() )
		return true;
	else
		return false;
}

*/
