// TLocalGeodeticRefFrame.cpp
//
/** A Geodetic Ellisoidal Reference Frame. 
    Deals with requests for Spatial Position coordinates
    appropriate to a Geodetic reference frame.

  
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
#include  "TLocalGeodeticRefFrame.h"
#include  "TGraph.h"
#include  "TReferenceEllipsoid.h"
#include  "TGeodeticRefFrame.h"
#include  "TRotation.h"
#include  "TGC2LGTransformation.h"
#include  "TLG2GCTransformation.h"
////////////////////////////////////////////////////////////////


//ClassImp(TLocalGeodeticRefFrame)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
const double TLocalGeodeticRefFrame::precisionH = 0.0000001; //precision calcul h
const double TLocalGeodeticRefFrame::precisionPhi = 0.00000000000001; //precision calcul phi



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TLocalGeodeticRefFrame::TLocalGeodeticRefFrame( const string& name, TReferenceEllipsoid* ellipsoid,
											   const TSpatialPosition spos,
											   const TFreeVector falseOrigin,
											   const TRotationMatrix orientation,
											   TGeodeticRefFrame* GRF) 
	: TA3DEuclideanRefFrame( name ), fEllipsoid( ellipsoid ), fOrigin( spos ),
	fFalseOrigin( falseOrigin ), fOrientationMatrix( orientation ), fGeodeticSys( GRF )
{	// constructor taking the name of the reference frame
}


TLocalGeodeticRefFrame::TLocalGeodeticRefFrame( const string& name,
								   TSpatialPosition origin, TFreeVector falseOrigin, 
								   const TAngle gis, const TAngle slope)
	: TA3DEuclideanRefFrame(name)
{
	// geodetic reference frame
	fGeodeticSys = TGraph::getGraph()->getRefFrame(TGraph::kCCS)->getGeodeticRF();

	// ellipsoid
	fEllipsoid = TGraph::getGraph()->getEllipsoid(TGraph::kGRS80);

	// origin
	origin.transform(fGeodeticSys);
	fOrigin = origin;

	// false origin
	fFalseOrigin = falseOrigin;

	// orientation angles
	// orientation in CCS as a spatial vector
	TSpatialVector unitVector(TGraph::getGraph()->getRefFrame(TGraph::kCCS));
	TAngle alpha(slope), beta(gis);
	TFreeVector vector(alpha.cosine()*beta.sine(), alpha.cosine()*beta.cosine(),
		alpha.sine());
	unitVector.setXYZVector(vector);

	TAngle azcern(37.77864 * TAngle::gonsToRadsFactor());
	TAngle phiP0, lambdaP0;
	TVReferenceFrame* rf = TGraph::getGraph()->getRefFrame(TGraph::kCCS);
	phiP0 = rf->getOrigin().getPhiEllipsoid(fEllipsoid);
	lambdaP0 = rf->getOrigin().getLambdaEllipsoid(fEllipsoid);
	TZAxisRotation r1( ( (azcern + TAngle::piBy2()).getRadiansValue() ) );
	TRotation r2( 0, (TAngle::piBy2().getRadiansValue() - phiP0.getRadiansValue()),
		(TAngle::pi().getRadiansValue() - lambdaP0.getRadiansValue()) );
	TRotation *pr1 = &r1, *pr2 = &r2;
	unitVector.transform( pr1 );
	unitVector.transform( pr2 );

	TRotation r3( 0, TAngle::piBy2().getRadiansValue() - origin.getPhiEllipsoid(fEllipsoid).getRadiansValue(),
		TAngle::pi().getRadiansValue() - origin.getLambdaEllipsoid(fEllipsoid).getRadiansValue() );
	r3.invert();
	TRotation* pr3 = &r3;
	unitVector.transform( pr3 );
	TLength ycomp( - (unitVector.getYComponent().getMetresValue()) );
	unitVector.setYComponent( ycomp );


	TAngle bearing(TAngle::aTan2( unitVector.getYComponent().getMetresValue(),
		unitVector.getXComponent().getMetresValue() ));
	TRotationMatrix orientation(0,0,bearing.getRadiansValue());
	fOrientationMatrix = orientation;
	
}


// **Deliberately not implemented**
//TLocalGeodeticRefFrame::TLocalGeodeticRefFrame( const  TLocalGeodeticRefFrame& original )
//{	// copy constructor
//}


TLocalGeodeticRefFrame::~TLocalGeodeticRefFrame()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


// **Deliberately not implemented**
//TLocalGeodeticRefFrame&  TLocalGeodeticRefFrame::operator=(const TLocalGeodeticRefFrame& right)
//{	// Copy Assignment operator
//
//	if (this != &right)
//	{
//	}
//	return *this;
//}


// Return a clone
/*TVReferenceFrame*  TLocalGeodeticRefFrame::clone() const
{	
	return new TLocalGeodeticRefFrame( *this->getName(), *this->getReferenceEllipsoid() );
}
*/



///////////////////////////////////////////////////////
// sets the sigmaXX of the given Spatial Position Error
///////////////////////////////////////////////////////
bool TLocalGeodeticRefFrame::setSigmaXX( TSpatialPositionError* posEr, const TLength& s){
	
	setSigma11(posEr, s);
	return true;
}

///////////////////////////////////////////////////////
// sets the sigmaYY of the given Spatial Position Error
///////////////////////////////////////////////////////
bool TLocalGeodeticRefFrame::setSigmaYY( TSpatialPositionError* posEr, const TLength& s){
	
	setSigma22(posEr, s);
	return true;
}

///////////////////////////////////////////////////////
// sets the sigmaZZ of the given Spatial Position Error
///////////////////////////////////////////////////////
bool TLocalGeodeticRefFrame::setSigmaZZ( TSpatialPositionError* posEr, const TLength& s){
	
	setSigma33(posEr, s);
	return true;
}

///////////////////////////////////////////////////////
// sets the sigmaXY of the given Spatial Position Error
///////////////////////////////////////////////////////
bool TLocalGeodeticRefFrame::setSigmaXY( TSpatialPositionError* posEr, const TLength& s){
	
	setSigma12(posEr, s);
	return true;
}

///////////////////////////////////////////////////////
// sets the sigmaXZ of the given Spatial Position Error
///////////////////////////////////////////////////////
bool TLocalGeodeticRefFrame::setSigmaXZ( TSpatialPositionError* posEr, const TLength& s){
	
	setSigma13(posEr, s);
	return true;
}

///////////////////////////////////////////////////////
// sets the sigmaYZ of the given Spatial Position Error
///////////////////////////////////////////////////////
bool TLocalGeodeticRefFrame::setSigmaYZ( TSpatialPositionError* posEr, const TLength& s){
	
	setSigma23(posEr, s);
	return true;
}





//////////////////////////////////////
// sets the status of the X coordinate
//////////////////////////////////////
bool TLocalGeodeticRefFrame::setXCoordStatus( TPositionStatus* posn, TANumericValue::EStatus s ) 
{
	setX1Status( posn, s );
	return true;
}


//////////////////////////////////////
// sets the status of the Y coordinate
//////////////////////////////////////
bool TLocalGeodeticRefFrame::setYCoordStatus( TPositionStatus* posn, TANumericValue::EStatus s ) 
{
	setX2Status( posn, s );
	return true;
}


//////////////////////////////////////
// sets the status of the Z coordinate
//////////////////////////////////////
bool TLocalGeodeticRefFrame::setZCoordStatus( TPositionStatus* posn, TANumericValue::EStatus s ) 
{
	setX3Status( posn, s );
	return true;
}


/////////////////////////////
// sets the position's status
/////////////////////////////
bool TLocalGeodeticRefFrame::setPositionStatus( TPositionStatus* posn, TPositionStatus::EPositionStatus ps )  
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

		


//////////////////////////////////////////////////////////
// returns the sigmaXX of the given Spatial Position Error
//////////////////////////////////////////////////////////
TLength TLocalGeodeticRefFrame::getSigmaXX(const TSpatialPositionError* posEr) const{

	TLength s( getSigma11(posEr) );
	TSpatialPositionError *spe = const_cast< TSpatialPositionError* >(posEr);
	s.setStatus( (getStatus(spe))->getXCoordStatus());

	return s;
}

//////////////////////////////////////////////////////////
// returns the sigmaYY of the given Spatial Position Error
//////////////////////////////////////////////////////////
TLength TLocalGeodeticRefFrame::getSigmaYY(const TSpatialPositionError* posEr) const{

	TLength s( getSigma22(posEr) );
	TSpatialPositionError *spe = const_cast< TSpatialPositionError* >(posEr);
	s.setStatus( (getStatus(spe))->getYCoordStatus());


	return s;
}

//////////////////////////////////////////////////////////
// returns the sigmaZZ of the given Spatial Position Error
//////////////////////////////////////////////////////////
TLength TLocalGeodeticRefFrame::getSigmaZZ(const TSpatialPositionError* posEr) const{

	TLength s( getSigma33(posEr) );
	TSpatialPositionError *spe = const_cast< TSpatialPositionError* >(posEr);
	s.setStatus( (getStatus(spe))->getZCoordStatus());


	return s;
}

//////////////////////////////////////////////////////////
// returns the sigmaXY of the given Spatial Position Error
//////////////////////////////////////////////////////////
TLength TLocalGeodeticRefFrame::getSigmaXY(const TSpatialPositionError* posEr) const{

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
TLength TLocalGeodeticRefFrame::getSigmaXZ(const TSpatialPositionError* posEr) const{

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
TLength TLocalGeodeticRefFrame::getSigmaYZ(const TSpatialPositionError* posEr) const{

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
TANumericValue::EStatus  TLocalGeodeticRefFrame::getXCoordStatus( const TPositionStatus* posn ) const
{
	return getX1Status( posn );
}

/////////////////////////////////////////////
// gets the status of one of the Y coordinate
/////////////////////////////////////////////
TANumericValue::EStatus  TLocalGeodeticRefFrame::getYCoordStatus( const TPositionStatus* posn ) const
{
	return getX2Status( posn );
}

/////////////////////////////////////////////
// gets the status of one of the Z coordinate
/////////////////////////////////////////////
TANumericValue::EStatus  TLocalGeodeticRefFrame::getZCoordStatus( const TPositionStatus* posn ) const
{
	return getX3Status( posn );
}


//////////////////////////////////
// gets the global position status
//////////////////////////////////
TPositionStatus::EPositionStatus  TLocalGeodeticRefFrame::getPositionStatus( const TPositionStatus* poss) const{

	return getStatus( poss);
}




// Returns a pointer to the references ellipsoid
TReferenceEllipsoid*  TLocalGeodeticRefFrame::getReferenceEllipsoid() const
{
	return fEllipsoid;
}


///////////////////////////////////////////
// checks if the X coordinate isn't unknown
///////////////////////////////////////////
bool TLocalGeodeticRefFrame::isXCoordKnown( const TSpatialPosition* posn ) const
{
	if ( getX1Status( getStatus(posn) ) != TANumericValue::kNull )
		return true;
	else
		return false;
}

///////////////////////////////////////////
// checks if the Y coordinate isn't unknown
///////////////////////////////////////////
bool TLocalGeodeticRefFrame::isYCoordKnown( const TSpatialPosition* posn ) const
{
	if ( getX2Status( getStatus(posn) ) != TANumericValue::kNull )
		return true;
	else
		return false;
}

///////////////////////////////////////////
// checks if the Z coordinate isn't unknown
///////////////////////////////////////////
bool TLocalGeodeticRefFrame::isZCoordKnown( const TSpatialPosition* posn ) const
{
	if ( getX3Status( getStatus(posn) ) != TANumericValue::kNull )
		return true;
	else
		return false;
}


//////////////////////////////
// transforms spatial position
//////////////////////////////
bool TLocalGeodeticRefFrame::transform( TSpatialPosition *sp, TVAffineTransformation *trans )
{
	TPositionVector pv, pv2;
	pv = sp->getXYZ();

	pv2 = trans->transform(pv);
	sp->setXYZ(pv2);

	return true;
}


///////////////////////////////////////////////////////
// transform a position from a reference frame to CGRF
//////////////////////////////////////////////////////
bool TLocalGeodeticRefFrame::transformToCGRF( TSpatialPosition* sp )
{
	return sp->transform( (new TLG2GCTransformation(this))->getTransformation() );
}


//////////////////////////////////////////////////////////
// transform a vector from CGRF to another reference frame
//////////////////////////////////////////////////////////
bool TLocalGeodeticRefFrame::transformFromCGRF( TSpatialPosition* sp )
{
	return sp->transform( (new TGC2LGTransformation(this))->getTransformation() );
}





//////////////////////////////
// transforms spatial vector
//////////////////////////////
bool TLocalGeodeticRefFrame::transform( TSpatialVector *sv, TVAffineTransformation *trans )
{
	TFreeVector fv, fv2;
	fv = sv->getXYZVector();

	fv2 = trans->transform(fv);
	sv->setXYZVector(fv2);

	return true;
}




//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

