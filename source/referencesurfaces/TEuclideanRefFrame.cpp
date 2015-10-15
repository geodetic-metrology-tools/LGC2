// TEuclideanRefFrame.cpp
//
/** A Euclidean Reference Frame. 
    Deals with requests for Spatial Position coordinates
    appropriate to a Euclidean reference frame.

  
    Patterns:
    A concrete STATE of a Spatial Position
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include "TPositionStatus.h"

#include  "TEuclideanRefFrame.h"
////////////////////////////////////////////////////////////////


//ClassImp(TEuclideanRefFrame)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TEuclideanRefFrame::TEuclideanRefFrame( const string& name ) 
	: TAReferenceFrame( name )//, fFalseOrigin( TReferenceDatumServer::kLocal )
{	// constructor taking the name of the reference frame
}

// **Deliberately not implemented**
//TEuclideanRefFrame::TEuclideanRefFrame( const  TEuclideanRefFrame& original )
//{	// copy constructor
//}


TEuclideanRefFrame::~TEuclideanRefFrame()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


// **Deliberately not implemented**
//TEuclideanRefFrame&  TEuclideanRefFrame::operator=(const TEuclideanRefFrame& right)
//{	// Copy Assignment operator
//
//	if (this != &right)
//	{
//	}
//	return *this;
//}






///////////////////////////////////////////////////////////////////////
// Member Functions
///////////////////////////////////////////////////////////////////////

bool  TEuclideanRefFrame::setXCoord( TSpatialPosition* posn, const TLength &x )
{	// set the x coordinate value and mark the coordinate as variable
	setX1( posn, x.getMetresValue() );
	//(getStatus(posn))->setXCoordStatus( TANumericValue::kVariable );
	return true;
}

bool  TEuclideanRefFrame::setYCoord( TSpatialPosition* posn, const TLength &y )
{	// set the y coordinate value and mark the coordinate as variable
	setX2( posn, y.getMetresValue() );
	//(getStatus(posn))->setYCoordStatus( TANumericValue::kVariable );
	return true;
}

bool  TEuclideanRefFrame::setZCoord( TSpatialPosition* posn, const TLength &z )
{	// set the z coordinate value and mark the coordinate as variable
	setX3( posn, z.getMetresValue() );
	//(getStatus(posn))->setZCoordStatus( TANumericValue::kVariable );
	return true;
}	


///////////////////////////////////////////////////////
// sets the sigmaXX of the given Spatial Position Error
///////////////////////////////////////////////////////
bool TEuclideanRefFrame::setSigmaXX( TSpatialPositionError* posEr, const TLength& s){
	
	setSigma11(posEr, s);
	return true;
}

///////////////////////////////////////////////////////
// sets the sigmaYY of the given Spatial Position Error
///////////////////////////////////////////////////////
bool TEuclideanRefFrame::setSigmaYY( TSpatialPositionError* posEr, const TLength& s){
	
	setSigma22(posEr, s);
	return true;
}

///////////////////////////////////////////////////////
// sets the sigmaZZ of the given Spatial Position Error
///////////////////////////////////////////////////////
bool TEuclideanRefFrame::setSigmaZZ( TSpatialPositionError* posEr, const TLength& s){
	
	setSigma33(posEr, s);
	return true;
}

///////////////////////////////////////////////////////
// sets the sigmaXY of the given Spatial Position Error
///////////////////////////////////////////////////////
bool TEuclideanRefFrame::setSigmaXY( TSpatialPositionError* posEr, const TLength& s){
	
	setSigma12(posEr, s);
	return true;
}

///////////////////////////////////////////////////////
// sets the sigmaXZ of the given Spatial Position Error
///////////////////////////////////////////////////////
bool TEuclideanRefFrame::setSigmaXZ( TSpatialPositionError* posEr, const TLength& s){
	
	setSigma13(posEr, s);
	return true;
}

///////////////////////////////////////////////////////
// sets the sigmaYZ of the given Spatial Position Error
///////////////////////////////////////////////////////
bool TEuclideanRefFrame::setSigmaYZ( TSpatialPositionError* posEr, const TLength& s){
	
	setSigma23(posEr, s);
	return true;
}



//////////////////////////////////////
// sets the status of the X coordinate
//////////////////////////////////////
bool TEuclideanRefFrame::setXCoordStatus( TPositionStatus* posn, TANumericValue::EStatus s ) 
{
	setX1Status( posn, s );
	return true;
}


//////////////////////////////////////
// sets the status of the Y coordinate
//////////////////////////////////////
bool TEuclideanRefFrame::setYCoordStatus( TPositionStatus* posn, TANumericValue::EStatus s ) 
{
	setX2Status( posn, s );
	return true;
}


//////////////////////////////////////
// sets the status of the Z coordinate
//////////////////////////////////////
bool TEuclideanRefFrame::setZCoordStatus( TPositionStatus* posn, TANumericValue::EStatus s ) 
{
	setX3Status( posn, s );
	return true;
}


/////////////////////////////
// sets the position's status
/////////////////////////////
bool TEuclideanRefFrame::setPositionStatus( TPositionStatus* posn, TPositionStatus::EPositionStatus ps )  
{
	switch(ps){

	case TPositionStatus::kCala : 
		setX1Status( posn, TANumericValue::kFixed );
		setX2Status( posn, TANumericValue::kFixed );
		setX3Status( posn, TANumericValue::kFixed );
		break;

	case TPositionStatus::kVx : 
		setX1Status( posn, TANumericValue::kVariable );
		setX2Status( posn, TANumericValue::kFixed );
		setX3Status( posn, TANumericValue::kFixed );
		break;

	case TPositionStatus::kVy : 
		setX1Status( posn, TANumericValue::kFixed );
		setX2Status( posn, TANumericValue::kVariable );
		setX3Status( posn, TANumericValue::kFixed );
		break;

	case TPositionStatus::kVz : 
		setX1Status( posn, TANumericValue::kFixed );
		setX2Status( posn, TANumericValue::kFixed );
		setX3Status( posn, TANumericValue::kVariable );
		break;

	case TPositionStatus::kVxy : 
		setX1Status( posn, TANumericValue::kVariable );
		setX2Status( posn, TANumericValue::kVariable );
		setX3Status( posn, TANumericValue::kFixed );
		break;

	case TPositionStatus::kVxz : 
		setX1Status( posn, TANumericValue::kVariable );
		setX2Status( posn, TANumericValue::kFixed );
		setX3Status( posn, TANumericValue::kVariable );
		break;

	case TPositionStatus::kVyz : 
		setX1Status( posn, TANumericValue::kFixed );
		setX2Status( posn, TANumericValue::kVariable );
		setX3Status( posn, TANumericValue::kVariable );
		break;

	case TPositionStatus::kVxyz : 
		setX1Status( posn, TANumericValue::kVariable );
		setX2Status( posn, TANumericValue::kVariable );
		setX3Status( posn, TANumericValue::kVariable );
		break;

	default:
		setX1Status( posn, TANumericValue::kNull);
		setX2Status( posn, TANumericValue::kNull);
		setX3Status( posn, TANumericValue::kNull);
		break;
	}

	return 	setStatus( posn, ps );
}

		
// Returns the X coordinate of the given Spatial Position
TLength  TEuclideanRefFrame::getXCoord( const TSpatialPosition* posn ) const
{	//return X coord from the spatial position

	TLength  x( getX1( posn ) );
	x.setStatus( posn->getXCoordStatus());

	return x;
}


// Returns the X coordinate of the given Spatial Position
TLength  TEuclideanRefFrame::getYCoord( const TSpatialPosition* posn ) const
{	//return Y coord from the spatial position

	TLength  x( getX2( posn ) );
	x.setStatus( posn->getYCoordStatus() );

	return x;
}


// Returns the X coordinate of the given Spatial Position
TLength  TEuclideanRefFrame::getZCoord( const TSpatialPosition* posn ) const
{	//return Z coord from the spatial position

	TLength  x( getX3( posn ) );
	x.setStatus( posn->getZCoordStatus() );

	return x;
}

//////////////////////////////////////////////////////////
// returns the sigmaXX of the given Spatial Position Error
//////////////////////////////////////////////////////////
TLength TEuclideanRefFrame::getSigmaXX(const TSpatialPositionError* posEr) const{

	TLength s( getSigma11(posEr) );
	TSpatialPositionError *spe = const_cast< TSpatialPositionError* >(posEr);
	s.setStatus( (getStatus(spe))->getXCoordStatus());

	return s;
}

//////////////////////////////////////////////////////////
// returns the sigmaYY of the given Spatial Position Error
//////////////////////////////////////////////////////////
TLength TEuclideanRefFrame::getSigmaYY(const TSpatialPositionError* posEr) const{

	TLength s( getSigma22(posEr) );
	TSpatialPositionError *spe = const_cast< TSpatialPositionError* >(posEr);
	s.setStatus( (getStatus(spe))->getYCoordStatus());


	return s;
}

//////////////////////////////////////////////////////////
// returns the sigmaZZ of the given Spatial Position Error
//////////////////////////////////////////////////////////
TLength TEuclideanRefFrame::getSigmaZZ(const TSpatialPositionError* posEr) const{

	TLength s( getSigma33(posEr) );
	TSpatialPositionError *spe = const_cast< TSpatialPositionError* >(posEr);
	s.setStatus( (getStatus(spe))->getZCoordStatus());


	return s;
}

//////////////////////////////////////////////////////////
// returns the sigmaXY of the given Spatial Position Error
//////////////////////////////////////////////////////////
TLength TEuclideanRefFrame::getSigmaXY(const TSpatialPositionError* posEr) const{

	TLength s( getSigma12(posEr) );
	TSpatialPositionError *spe = const_cast< TSpatialPositionError* >(posEr);
	TANumericValue::EStatus stX, stY;
	stX = (getStatus(spe))->getXCoordStatus();
	stY = (getStatus(spe))->getYCoordStatus();

	//the lowest status is assigned to the TLength return
	if ( stX <= stY)
		s.setStatus( stX );
	else
		s.setStatus( stY );

	return s;
}

//////////////////////////////////////////////////////////
// returns the sigmaXZ of the given Spatial Position Error
//////////////////////////////////////////////////////////
TLength TEuclideanRefFrame::getSigmaXZ(const TSpatialPositionError* posEr) const{

	TLength s( getSigma13(posEr) );

	TSpatialPositionError *spe = const_cast< TSpatialPositionError* >(posEr);
	TANumericValue::EStatus stX, stZ;
	stX = (getStatus(spe))->getXCoordStatus();
	stZ = (getStatus(spe))->getZCoordStatus();

	//the lowest status is assigned to the TLength return
	if ( stX <= stZ)
		s.setStatus( stX );
	else
		s.setStatus( stZ );

	return s;

}

//////////////////////////////////////////////////////////
// returns the sigmaYZ of the given Spatial Position Error
//////////////////////////////////////////////////////////
TLength TEuclideanRefFrame::getSigmaYZ(const TSpatialPositionError* posEr) const{

	TLength s( getSigma23(posEr) );

	TSpatialPositionError *spe = const_cast< TSpatialPositionError* >(posEr);
	TANumericValue::EStatus stY, stZ;
	stY = (getStatus(spe))->getYCoordStatus();
	stZ = (getStatus(spe))->getZCoordStatus();

	//the lowest status is assigned to the TLength return
	if ( stY <= stZ)
		s.setStatus( stY );
	else
		s.setStatus( stZ );

	return s;
}



/////////////////////////////////////////////
// gets the status of one of the X coordinate
/////////////////////////////////////////////
TANumericValue::EStatus  TEuclideanRefFrame::getXCoordStatus( const TPositionStatus* posn ) const
{
	return getX1Status( posn );
}

/////////////////////////////////////////////
// gets the status of one of the Y coordinate
/////////////////////////////////////////////
TANumericValue::EStatus  TEuclideanRefFrame::getYCoordStatus( const TPositionStatus* posn ) const
{
	return getX2Status( posn );
}

/////////////////////////////////////////////
// gets the status of one of the Z coordinate
/////////////////////////////////////////////
TANumericValue::EStatus  TEuclideanRefFrame::getZCoordStatus( const TPositionStatus* posn ) const
{
	return getX3Status( posn );
}

//////////////////////////////////
// gets the global position status
//////////////////////////////////
TPositionStatus::EPositionStatus  TEuclideanRefFrame::getPositionStatus( const TPositionStatus* poss) const{

	return getStatus( poss);
}



///////////////////////////////////////////
// checks if the X coordinate isn't unknown
///////////////////////////////////////////
bool TEuclideanRefFrame::isXCoordKnown( const TSpatialPosition* posn ) const
{
	TSpatialPosition* sp = const_cast< TSpatialPosition *>(posn);
	if ( (getStatus( sp ))->getXCoordStatus() != TANumericValue::kNull )
		return true;
	else
		return false;
}

///////////////////////////////////////////
// checks if the Y coordinate isn't unknown
///////////////////////////////////////////
bool TEuclideanRefFrame::isYCoordKnown( const TSpatialPosition* posn ) const
{
	TSpatialPosition* sp = const_cast< TSpatialPosition *>(posn);
	if ( (getStatus( sp ))->getYCoordStatus() != TANumericValue::kNull )
		return true;
	else
		return false;
}

///////////////////////////////////////////
// checks if the Z coordinate isn't unknown
///////////////////////////////////////////
bool TEuclideanRefFrame::isZCoordKnown( const TSpatialPosition* posn ) const
{
	TSpatialPosition* sp = const_cast< TSpatialPosition *>(posn);
	if ( (getStatus( sp ))->getZCoordStatus() != TANumericValue::kNull )
		return true;
	else
		return false;
}



//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

