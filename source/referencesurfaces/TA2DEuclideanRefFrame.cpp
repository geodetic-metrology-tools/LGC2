// TA2DXYRefFrame.cpp
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
#include	"TA2DEuclideanRefFrame.h"
//ClassImp(TAReferenceFrame)
#include "TSpatialPosition.h"
#include "TPositionStatus.h"
#include "TSpatialPositionError.h"

#include "TSpatialVectorStatus.h"
#include "TSpatialVectorError.h"



//////////////////////////////////////////////////////////////////////
// Initialisation
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


TA2DEuclideanRefFrame::TA2DEuclideanRefFrame( const string& name )
	: TAReferenceFrame( name )
{	// default constructor 
}


//TA2DXYRefFrame::TA2DXYRefFrame(const TA2DXYRefFrame& trf)
//{	// copy constructor 
//}


TA2DEuclideanRefFrame::~TA2DEuclideanRefFrame()
{
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
/*
//Default code for accessing the position vector of a spatial position 
//see definition of TSpatialPosition
TPositionVector TA2DEuclideanRefFrame::getCoordinates(const TSpatialPosition* sp, TCoordSysFactory::ECoordSys coordsys) const
{
	if(coordsys == TCoordSysFactory::k3DCartesian) {
		return getPositionVector(sp);
	}

	else 
	{ 
		TPositionVector pv(TGraph::getGraph()->getCoordSys(TCoordSysFactory::k3DCartesian));
		return pv;
	}
}

//Default code for accessing the spatial position error of a spatial position 
//see definition of TSpatialPosition
TSpatialPositionError TA2DEuclideanRefFrame::getCoordCovariances(const TSpatialPosition* sp, TCoordSysFactory::ECoordSys coordsys) const 
{
	TSpatialPositionError spe;
	if(coordsys == TCoordSysFactory::k3DCartesian) {
		spe = getCovariances(sp);
	}
	return spe;
}

//Default code for accessing the free vector of a spatial vector 
//see definition of TSpatialVector
TFreeVector TA2DEuclideanRefFrame::getElements(const TSpatialVector* sv, TCoordSysFactory::ECoordSys coordsys) const
{
	TFreeVector fv;
	if(coordsys == TGraph::k3DCartesian) {
		fv = getVector(sv);
	}
	return fv;
}

 //Default code for accessing the spatial vector error of a spatial vector 
//see definition of TSpatialVector	
TSpatialVectorError TA2DEuclideanRefFrame::getElementCovars(const TSpatialVector* sv, TCoordSysFactory::ECoordSys) const 
{
	TSpatialVectorError sve;
	if(coordsys == TGraph::k3DCartesian) {
		sve = getCovariances(sv);
	}
	return sve;
}

//Default code for accessing the rotation matrix of a spatial orientation 
//see definition of TSpatialOrientation
TRotationMatrix TA2DEuclideanRefFrame::getElements(const TSpatialOrientation* so, TCoordSysFactory::ECoordSys coordsys) const
{
	TRotationMatrix rm;
	if(coordsys == TCoordSysFactory::k3DCartesian) {
		rm = getOrientation(so);
	}
	return rm;
}

//Default code for accessing the spatial vector error of a spatial orientation 
//see definition of TSpatialOrientation	
TSpatialOrientationError TA2DEuclideanRefFrame::getElementCovars(const TSpatialOrientation* so, TCoordSysFactory::ECoordSys) const 
{
	TSpatialOrientationError soe;
	if(coordsys == TCoordSysFactory::k3DCartesian) {
		soe = getOrientationCovars(so);
	}
	return soe;
}


//Default code for setting the position vector of a spatial position 
//see definition of TSpatialPosition
bool TA2DEuclideanRefFrame::setCoordinates(TSpatialPosition* sp, const TPositionVector& pv)
{
	bool result;
	result = setPositionVector(sp, pv);
	return result;
}

//Default code for setting the spatial position error of a spatial position 
//see definition of TSpatialPosition
bool TA2DEuclideanRefFrame::setCoordCovariances(TSpatialPosition* sp, const TSpatialPositionError& spe)
{
	bool result;
	result = setCovariances(sp, spe);
	return true; 
}

//Default code for setting the free vector of a spatial vector 
//see definition of TSpatialVector
bool TA2DEuclideanRefFrame::setElements(TSpatialVector* sv, const TFreeVector& fv)
{
	bool result;
	result = setPositionVector(sv, fv);
	return result;
}

 //Default code for setting the spatial vector error of a spatial vector
bool TA2DEuclideanRefFrame::setElementCovars(TSpatialVector* sv, const TSpatialVectorError& sve)
{
	bool result;
	result = setCovariances(sv, sve);
	return true; 
}

  
//Default code for setting the rotation matrix of a spatial orientation 
//see definition of TSpatialOrientation
bool TA2DEuclideanRefFrame::setElements(TSpatialOrientation* so, const TRotationMatrix& rm)
{
	bool result;
	result = setOrientation(so, rm);
	return result;
}

//Default code for setting the spatial orientation error of a spatial orientation
//see definition of TSpatialOrientation
bool TA2DEuclideanRefFrame::setElementCovars(TSpatialOrientation* so, const TSpatialOrientationError& soe)
{
	bool result;
	result = setOrientationCovars(so, soe);
	return true; 
}



*/
/*string  TA2DEuclideanRefFrame::getName() const
{	//return the name of the reference frame
	
	return fName;
}
*/


/*
bool  TA2DEuclideanRefFrame::setXCoord( TSpatialPosition* posn, const TLength &x )
{	// set the x coordinate value and mark the coordinate as variable
	setX1( posn, x.getMetresValue() );
	posn->setXAxisStatus(TANumericValue::kVariable);
	//setXCoordStatus( getStatus(posn), TANumericValue::kVariable );
	return true;
}

bool  TA2DEuclideanRefFrame::setYCoord( TSpatialPosition* posn, const TLength &y )
{	// set the y coordinate value and mark the coordinate as variable
	setX2( posn, y.getMetresValue() );
	posn->setYAxisStatus(TANumericValue::kVariable);
	//setYCoordStatus( getStatus(posn), TANumericValue::kVariable );
	return true;
}


TLength  TA2DEuclideanRefFrame::getXCoord( const TSpatialPosition* posn ) const
{	//return X coord from the spatial position

	TLength  x( getX1( posn ) );
//	x.setStatus( getX1Status( posn ) );

	return x;
}


TLength  TA2DEuclideanRefFrame::getYCoord( const TSpatialPosition* posn ) const
{	//return Y coord from the spatial position

	TLength  x( getX2( posn ) );
//	x.setStatus( getX2Status( posn ) );

	return x;
}



////////////////////////////////////////////
// sets the components of the Spatial Vector
////////////////////////////////////////////
bool  TA2DEuclideanRefFrame::setXComp( TSpatialVector* sf, const TLength &x )
{	// set the x comonent value and mark the coordinate as variable
	setF1( sf, x.getMetresValue() );
	sf->setXAxisStatus(TANumericValue::kVariable);
	//setXCompStatus( getStatus(sf), TANumericValue::kVariable );
	return true;
}


bool  TA2DEuclideanRefFrame::setYComp( TSpatialVector* sf, const TLength &y )
{	// set the y comonent value and mark the coordinate as variable
	setF2( sf, y.getMetresValue() );
	sf->setYAxisStatus(TANumericValue::kVariable);
	//setYCompStatus( getStatus(sf), TANumericValue::kVariable );
	return true;
}




// Returns the X component of the given Spatial Vector
TLength  TA2DEuclideanRefFrame::getXComp( const TSpatialVector* svec ) const
{	
	TLength x(getF1(svec));
	return x;
}


// Returns the Y component of the given Spatial Vector
TLength  TA2DEuclideanRefFrame::getYComp( const TSpatialVector* svec ) const
{	
	TLength  x(getF2(svec));
	return x;
}
*/
/*		
///////////////////////////////////////////////////////////////////////////
// transforms a spatial position with an affine transformation, if possible
///////////////////////////////////////////////////////////////////////////
bool TA2DEuclideanRefFrame::transform( TSpatialPosition* sp, TAAffineTransformation* trans)
{
	//default : don't transform anything
	return false;
}


////////////////////////////////////////////////////////////////////////
// transforms a spatial vector with an affine transformation, if possible
////////////////////////////////////////////////////////////////////////
bool TA2DXYRefFrame::transform(TSpatialVector* sv, TAAffineTransformation* trans)
{
	return false;
}


////////////////////////////////////////////////////////////////////////
// transforms a spatial orientation with an affine transformation, if possible
////////////////////////////////////////////////////////////////////////
bool TA2DEuclideanRefFrame::transform(TSpatialOrientation* so, TAAffineTransformation* trans)
{
	return false;
}
*/
//////////////////////////////////////////////
// Overloaded == operator definition
//////////////////////////////////////////////
/*bool TA2DEuclideanRefFrame::operator==(TA2DXYRefFrame &trf) const {

	return false;
}*/



		

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

