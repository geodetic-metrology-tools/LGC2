// TSpatialPositionError.h: 
//
// The Spatial Position Error of a point in a given reference frame.
// 
// Provides an interface with restricted access to the coordinate errors
// of a TSpatialPosition Object.
// 
// Patterns:
// This class is a Proxy.
//
// Copyright 1999,2000, CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// other forward declarations
#include  "TAReferenceFrame.h"
#include  "TSpatialPositionError.h"
#include  "TLength.h"
#include  "TAngle.h"
//#include  "TReferenceEllipsoid.h"
#include  "TSpatialPosition.h"
#include <math.h>
////////////////////////////////////////////////////////////////


//ClassImp(TSpatialPositionError)

////////////////////////////////////////////////////////////////
//initialisation constantes
////////////////////////////////////////////////////////////////

///////////////////////////
// no argument constructor
///////////////////////////
TSpatialPositionError::TSpatialPositionError( TCoordSysFactory::ECoordSys cs ):
fSigmaMtrx(cs)
//fRefFrame(trf), fAssociatedPosition(0)
{
/*	TSpatialStatus* ps = new TSpatialStatus();
	counted_ptr< TSpatialStatus >* tempPtr = new counted_ptr< TSpatialStatus >(ps);
	fSigmaStatus = *tempPtr;
*/
}

/*
TSpatialPositionError::TSpatialPositionError( TSpatialPosition* sp, counted_ptr< TSpatialStatus > cpps ):
fAssociatedPosition(sp),
fRefFrame(sp->getReferenceFrame()),
fSigmaStatus(cpps)
{

};
*/

// default constructor
/*
TSpatialPositionError::TSpatialPositionError()
{
}
*/
TSpatialPositionError::TSpatialPositionError( const TSpatialPositionError& source ):
fSigmaMtrx(source.fSigmaMtrx)
//fSigmaStatus(source.fSigmaStatus)
//fRefFrame(source.getReferenceFrame()),
//fAssociatedPosition(source.fAssociatedPosition)
{//copy constructor


}


////////////////////////////////////////
// constructor setting the 3 coordinates
////////////////////////////////////////
/*TSpatialPositionError::TSpatialPositionError( TVReferenceFrame* trf, Coordinate x1, Coordinate x2, Coordinate z) 
{

	for (int i=0;i<3;i++){
		fXStatus[i] = TANumericValue::kVariable;
	}
	fPositionStatus = kVxyz;
	fX[0] = x;
	fX[1] = y;
	fX[2] = z;
	fRefFrame = trf;
};*/



/////////////
// destructor
/////////////
TSpatialPositionError::~TSpatialPositionError()
{

};


///////////////////////////////////////////////////////////////////////
// Copy assignement operator
///////////////////////////////////////////////////////////////////////
TSpatialPositionError& TSpatialPositionError::operator=(const TSpatialPositionError& source)
{
	if (this != &source)
	{
		
//		fSigmaStatus = source.fSigmaStatus;

		fSigmaMtrx = source.fSigmaMtrx;

//		fAssociatedPosition = source.fAssociatedPosition;
	}

//	fRefFrame = source.getReferenceFrame();

	return *this;
}



///////////////////////////////////////////////////////////////////////
// Member Functions
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////
// Returns the variances as a TFreeVector
///////////////////////////////////////
TFreeVector TSpatialPositionError::getVariances() const
{
	TFreeVector var = fSigmaMtrx.getDiagCoefs();
	return var;
}

/////////////////////////////////////////////////////////
// Returns the variances/covariance matrix as a T3DMatrix
/////////////////////////////////////////////////////////
T3DMatrix TSpatialPositionError::getCovariances() const
{
	return fSigmaMtrx;
}

///////////////////////////////////////
// Sets the variances from a TFreeVector
///////////////////////////////////////
bool TSpatialPositionError::setVariances(const TFreeVector& fv) 
{
	fSigmaMtrx.setDiagCoefs(fv);
	return true;
}

/////////////////////////////////////////////////////////
// Sets the variances from a T3DMatrix
/////////////////////////////////////////////////////////
bool TSpatialPositionError::setCovariances(const T3DMatrix& mx) 
{
	fSigmaMtrx = mx;
	return true;
}

/*bool  TSpatialPositionError::setSigmaXX(const TLength &x )
{	// set the sigmaXX error value

	return  fRefFrame->setSigmaXX( this, x );
}


bool  TSpatialPositionError::setSigmaYY(const TLength &x )
{	// set the sigmaYY error value

	return  fRefFrame->setSigmaYY( this, x );
}

bool  TSpatialPositionError::setSigmaZZ(const TLength &x )
{	// set the sigmaZZ error value

	return  fRefFrame->setSigmaZZ( this, x );
}

bool  TSpatialPositionError::setSigmaXY(const TLength &x )
{	// set the sigmaXY error value

	return  fRefFrame->setSigmaXY( this, x );
}

bool  TSpatialPositionError::setSigmaXZ(const TLength &x )
{	// set the sigmaXZ error value

	return  fRefFrame->setSigmaXZ( this, x );
}

bool  TSpatialPositionError::setSigmaYZ(const TLength &x )
{	// set the sigmaYZ error value

	return  fRefFrame->setSigmaYZ( this, x );
}

//!Sets al the sigmas to zero
void TSpatialPositionError::setToZero(){

	fSigmaMtrx = LITERAL(0.0);
}
*/
/*
/////////////////////////////////
// sets the status of the X error
/////////////////////////////////
void TSpatialPositionError::setSigmaXStatus(TSpatialStatus::EAxisStatus s){

	fSigmaStatus->setXAxisStatus( s );
	return;
}

/////////////////////////////////
// sets the status of the Y error
/////////////////////////////////
void TSpatialPositionError::setSigmaYStatus(TSpatialStatus::EAxisStatus s){

	fSigmaStatus->setYAxisStatus( s );
	return;
}

/////////////////////////////////
// sets the status of the Z error
/////////////////////////////////
void TSpatialPositionError::setSigmaZStatus(TSpatialStatus::EAxisStatus s){

	fSigmaStatus->setZAxisStatus( s );
	return;
}

/////////////////////////////////////////////////////
// sets the 3D Cartesian coordinates status (by copy)
/////////////////////////////////////////////////////
void TSpatialPositionError::setErrorStatus(counted_ptr< TSpatialStatus > cpps){

	fSigmaStatus = cpps;
	return;
}

*/

//////////////
// get methods
//////////////		
/*
TLength  TSpatialPositionError::getSigmaXX() const
{	//return sigmaXX error, if known
	
	return  fRefFrame->getSigmaXX( this );
}

TLength  TSpatialPositionError::getSigmaYY() const
{	//return sigmaYY error, if known
	
	return  fRefFrame->getSigmaYY( this );
}

TLength  TSpatialPositionError::getSigmaZZ() const
{	//return sigmaZZ error, if known
	
	return  fRefFrame->getSigmaZZ( this );
}

TLength  TSpatialPositionError::getSigmaXY() const
{	//return sigmaXY error, if known
	
	return  fRefFrame->getSigmaXY( this );
}

TLength  TSpatialPositionError::getSigmaXZ() const
{	//return sigmaXZ error, if known
	
	return  fRefFrame->getSigmaXZ( this );
}

TLength  TSpatialPositionError::getSigmaYZ() const
{	//return sigmaYZ error, if known
	
	return  fRefFrame->getSigmaYZ( this );
}
*/



///////////////////////////////////////////////////////////////////////////////
// Sets the error's status accordingly to the status of the associated position
///////////////////////////////////////////////////////////////////////////////
/*void TSpatialPositionError::refreshStatus(){

	TANumericValue::EStatus sX, sY, sZ;
	sX = fAssociatedPosition->getXAxisStatus();
	sY = fAssociatedPosition->getYAxisStatus();
	sZ = fAssociatedPosition->getZAxisStatus();

	//refresh the sigmaX status
	if (sX == TANumericValue::kVariable)
		fSigmaStatus->setXAxisStatus(TANumericValue::kVariable);
	else
		fSigmaStatus->setXAxisStatus(TANumericValue::kNull);

	//refresh the sigmaY status
	if (sY == TANumericValue::kVariable)
		fSigmaStatus->setYAxisStatus(TANumericValue::kVariable);
	else
		fSigmaStatus->setYAxisStatus(TANumericValue::kNull);

	//refresh the sigmaZ status
	if (sZ == TANumericValue::kVariable)
		fSigmaStatus->setZAxisStatus(TANumericValue::kVariable);
	else
		fSigmaStatus->setZAxisStatus(TANumericValue::kNull);

}


///////////////////////////////////
// gets the status of sigmaXX error
///////////////////////////////////
TSpatialStatus::EAxisStatus TSpatialPositionError::getSigmaXStatus() {

	refreshStatus();
	return  fSigmaStatus->getXAxisStatus( );
}

///////////////////////////////////
// gets the status of sigmaYY error
///////////////////////////////////
TSpatialStatus::EAxisStatus TSpatialPositionError::getSigmaYStatus() {

	refreshStatus();
	return  fSigmaStatus->getYAxisStatus( );
}

///////////////////////////////////
// gets the status of sigmaZZ error
///////////////////////////////////
TSpatialStatus::EAxisStatus TSpatialPositionError::getSigmaZStatus() {

	refreshStatus();
	return  fSigmaStatus->getZAxisStatus( );
}

///////////////////////////////////////////////////
// returns a pointer to the error's TSpatialStatus
///////////////////////////////////////////////////
TSpatialStatus* TSpatialPositionError::getStatus(){

	refreshStatus();
	return fSigmaStatus.get();
}

*/

