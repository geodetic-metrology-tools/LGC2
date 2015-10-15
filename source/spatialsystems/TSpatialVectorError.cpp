// TSpatialVectorError.h: 
//
// The Spatial Vector Error of a vector in a given reference frame.
// 
// Provides an interface with restricted access to the coordinate errors
// of a TSpatialVector Object.
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
#include  "TSpatialVectorError.h"
#include  "TAngle.h"
#include <math.h>
//#include  "TReferenceEllipsoid.h"
////////////////////////////////////////////////////////////////


//ClassImp(TSpatialPositionError)

////////////////////////////////////////////////////////////////
//initialisation constantes
////////////////////////////////////////////////////////////////

///////////////////////////
// no argument constructor
///////////////////////////
/*
TSpatialVectorError::TSpatialVectorError()
{
}
*/

TSpatialVectorError::TSpatialVectorError( TCoordSysFactory::ECoordSys cs ):
	fSigmaMtrx(cs)
{
}

/*
TSpatialVectorError::TSpatialVectorError( TSpatialVector* sp, counted_ptr< TSpatialStatus > cpps ):
fAssociatedVector(sp),
fRefFrame(sp->getRefFrame()),
fSigmaStatus(cpps)
{

};

*/

TSpatialVectorError::TSpatialVectorError( const TSpatialVectorError& source ):
fSigmaMtrx(source.fSigmaMtrx)
//fSigmaStatus(source.fSigmaStatus),
//fRefFrame(source.getReferenceFrame()),
//fAssociatedVector(source.fAssociatedVector)
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
TSpatialVectorError::~TSpatialVectorError()
{

};


///////////////////////////////////////////////////////////////////////
// Copy assignement operator
///////////////////////////////////////////////////////////////////////
TSpatialVectorError& TSpatialVectorError::operator=(const TSpatialVectorError& source)
{
	if (this != &source)
	{
		
//		fSigmaStatus = source.fSigmaStatus;

		fSigmaMtrx = source.fSigmaMtrx;

//		fAssociatedVector = source.fAssociatedVector;
	}

//	fRefFrame = source.getReferenceFrame();

	return *this;
}



///////////////////////////////////////////////////////////////////////
// Member Functions
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////
// Returns the variances as a T3DVector
///////////////////////////////////////
TFreeVector TSpatialVectorError::getVariances() const{

	TFreeVector var = fSigmaMtrx.getDiagCoefs();
	return var;
}

/////////////////////////////////////////////////////////
// Returns the variances/covariance matrix as a T3DMatrix
/////////////////////////////////////////////////////////
T3DMatrix TSpatialVectorError::getCovariances() const{

	return fSigmaMtrx;
}

/////////////////////////////////////////
// Sets the covariances from a T3DMatrix
/////////////////////////////////////////
bool TSpatialVectorError::setCovariances(const T3DMatrix& mx)
{
	fSigmaMtrx = mx;
	return true;
}

/////////////////////////////////////////
// Sets the variances from a TFreeVector
/////////////////////////////////////////
bool TSpatialVectorError::setVariances(const TFreeVector& fv)
{
	fSigmaMtrx.setDiagCoefs(fv);
	return true;
}


/*
bool  TSpatialVectorError::setSigmaXX(const TLength &x )
{	// set the sigmaXX error value

	return  fRefFrame->setSigmaXX( this, x );
}


bool  TSpatialVectorError::setSigmaYY(const TLength &x )
{	// set the sigmaYY error value

	return  fRefFrame->setSigmaYY( this, x );
}

bool  TSpatialVectorError::setSigmaZZ(const TLength &x )
{	// set the sigmaZZ error value

	return  fRefFrame->setSigmaZZ( this, x );
}

bool  TSpatialVectorError::setSigmaXY(const TLength &x )
{	// set the sigmaXY error value

	return  fRefFrame->setSigmaXY( this, x );
}

bool  TSpatialVectorError::setSigmaXZ(const TLength &x )
{	// set the sigmaXZ error value

	return  fRefFrame->setSigmaXZ( this, x );
}

bool  TSpatialVectorError::setSigmaYZ(const TLength &x )
{	// set the sigmaYZ error value

	return  fRefFrame->setSigmaYZ( this, x );
}

//!Sets al the sigmas to zero
void TSpatialVectorError::setToZero(){

	fSigmaMtrx = LITERAL(0.0);
}


/////////////////////////////////
// sets the status of the X error
/////////////////////////////////
void TSpatialVectorError::setSigmaXStatus(TSpatialStatus::EAxisStatus s){

	fSigmaStatus->setXAxisStatus( s );
	return;
}

/////////////////////////////////
// sets the status of the Y error
/////////////////////////////////
void TSpatialVectorError::setSigmaYStatus(TSpatialStatus::EAxisStatus s){

	fSigmaStatus->setYAxisStatus( s );
	return;
}

/////////////////////////////////
// sets the status of the Z error
/////////////////////////////////
void TSpatialVectorError::setSigmaZStatus(TSpatialStatus::EAxisStatus s){

	fSigmaStatus->setZAxisStatus( s );
	return;
}

/////////////////////////////////////////////////////
// sets the 3D Cartesian coordinates status (by copy)
/////////////////////////////////////////////////////
void TSpatialVectorError::setVectorStatus(counted_ptr< TSpatialStatus > cpps){

	fSigmaStatus = cpps;
	return;
}


//////////////
// get methods
//////////////		

TLength  TSpatialVectorError::getSigmaXX() const
{	//return sigmaXX error, if known
	
	return  fRefFrame->getSigmaXX( this );
}

TLength  TSpatialVectorError::getSigmaYY() const
{	//return sigmaYY error, if known
	
	return  fRefFrame->getSigmaYY( this );
}

TLength  TSpatialVectorError::getSigmaZZ() const
{	//return sigmaZZ error, if known
	
	return  fRefFrame->getSigmaZZ( this );
}

TLength  TSpatialVectorError::getSigmaXY() const
{	//return sigmaXY error, if known
	
	return  fRefFrame->getSigmaXY( this );
}

TLength  TSpatialVectorError::getSigmaXZ() const
{	//return sigmaXZ error, if known
	
	return  fRefFrame->getSigmaXZ( this );
}

TLength  TSpatialVectorError::getSigmaYZ() const
{	//return sigmaYZ error, if known
	
	return  fRefFrame->getSigmaYZ( this );
}
*/


/*
///////////////////////////////////////////////////////////////////////////////
// Sets the error's status accordingly to the status of the associated position
///////////////////////////////////////////////////////////////////////////////
void TSpatialVectorError::refreshStatus(){

	TANumericValue::EStatus sX, sY, sZ;
	sX = fAssociatedVector->getXAxisStatus();
	sY = fAssociatedVector->getYAxisStatus();
	sZ = fAssociatedVector->getZAxisStatus();

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
TSpatialStatus::EAxisStatus TSpatialVectorError::getSigmaXStatus() {

	refreshStatus();
	return  fSigmaStatus->getXAxisStatus( );
}

///////////////////////////////////
// gets the status of sigmaYY error
///////////////////////////////////
TSpatialStatus::EAxisStatus TSpatialVectorError::getSigmaYStatus() {

	refreshStatus();
	return  fSigmaStatus->getYAxisStatus( );
}

///////////////////////////////////
// gets the status of sigmaZZ error
///////////////////////////////////
TSpatialStatus::EAxisStatus TSpatialVectorError::getSigmaZStatus() {

	refreshStatus();
	return  fSigmaStatus->getZAxisStatus( );
}

///////////////////////////////////////////////////
// returns a pointer to the error's TPositionStatus
///////////////////////////////////////////////////
TSpatialStatus* TSpatialVectorError::getStatus(){

	refreshStatus();
	return fSigmaStatus.get();
}


*/
