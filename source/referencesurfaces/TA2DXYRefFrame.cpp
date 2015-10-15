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
#include	"TA2DXYRefFrame.h"
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


TA2DXYRefFrame::TA2DXYRefFrame( const string& name )
	: TAReferenceFrame( name )
{	// default constructor 
}


//TA2DXYRefFrame::TA2DXYRefFrame(const TA2DXYRefFrame& trf)
//{	// copy constructor 
//}


TA2DXYRefFrame::~TA2DXYRefFrame()
{
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

/*string  TA2DXYRefFrame::getName() const
{	//return the name of the reference frame
	
	return fName;
}
*/



bool  TA2DXYRefFrame::setXCoord( TSpatialPosition* posn, const TLength &x )
{	// set the x coordinate value and mark the coordinate as variable
	setX1( posn, x.getMetresValue() );
	posn->setXAxisStatus(TANumericValue::kVariable);
	//setXCoordStatus( getStatus(posn), TANumericValue::kVariable );
	return true;
}

bool  TA2DXYRefFrame::setYCoord( TSpatialPosition* posn, const TLength &y )
{	// set the y coordinate value and mark the coordinate as variable
	setX2( posn, y.getMetresValue() );
	posn->setYAxisStatus(TANumericValue::kVariable);
	//setYCoordStatus( getStatus(posn), TANumericValue::kVariable );
	return true;
}


TLength  TA2DXYRefFrame::getXCoord( const TSpatialPosition* posn ) const
{	//return X coord from the spatial position

	TLength  x( getX1( posn ) );
//	x.setStatus( getX1Status( posn ) );

	return x;
}


TLength  TA2DXYRefFrame::getYCoord( const TSpatialPosition* posn ) const
{	//return Y coord from the spatial position

	TLength  x( getX2( posn ) );
//	x.setStatus( getX2Status( posn ) );

	return x;
}



////////////////////////////////////////////
// sets the components of the Spatial Vector
////////////////////////////////////////////
bool  TA2DXYRefFrame::setXComp( TSpatialVector* sf, const TLength &x )
{	// set the x comonent value and mark the coordinate as variable
	setF1( sf, x.getMetresValue() );
	sf->setXAxisStatus(TANumericValue::kVariable);
	//setXCompStatus( getStatus(sf), TANumericValue::kVariable );
	return true;
}


bool  TA2DXYRefFrame::setYComp( TSpatialVector* sf, const TLength &y )
{	// set the y comonent value and mark the coordinate as variable
	setF2( sf, y.getMetresValue() );
	sf->setYAxisStatus(TANumericValue::kVariable);
	//setYCompStatus( getStatus(sf), TANumericValue::kVariable );
	return true;
}




// Returns the X component of the given Spatial Vector
TLength  TA2DXYRefFrame::getXComp( const TSpatialVector* svec ) const
{	
	TLength x(getF1(svec));
	return x;
}


// Returns the Y component of the given Spatial Vector
TLength  TA2DXYRefFrame::getYComp( const TSpatialVector* svec ) const
{	
	TLength  x(getF2(svec));
	return x;
}

/*		
///////////////////////////////////////////////////////////////////////////
// transforms a spatial position with an affine transformation, if possible
///////////////////////////////////////////////////////////////////////////
bool TA2DXYRefFrame::transform( TSpatialPosition* sp, TAAffineTransformation* trans)
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
bool TA2DXYRefFrame::transform(TSpatialOrientation* so, TAAffineTransformation* trans)
{
	return false;
}
*/
//////////////////////////////////////////////
// Overloaded == operator definition
//////////////////////////////////////////////
/*bool TA2DXYRefFrame::operator==(TA2DXYRefFrame &trf) const {

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

