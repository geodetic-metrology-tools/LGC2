// TSpatialOrientationError.h: 
//
// The Spatial Orientation Error of an orientation in a given reference frame.
// 
// Provides an interface with restricted access to the coordinate errors
// of a TSpatialOrientation Object.
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
#include  "TSpatialOrientationError.h"
#include <math.h>
#include  "TAngle.h"
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
TSpatialOrientationError::TSpatialOrientationError()
{
}
*/

TSpatialOrientationError::TSpatialOrientationError( TCoordSysFactory::ECoordSys cs ):
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

TSpatialOrientationError::TSpatialOrientationError( const TSpatialOrientationError& source ):
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
TSpatialOrientationError::~TSpatialOrientationError()
{

};


///////////////////////////////////////////////////////////////////////
// Copy assignement operator
///////////////////////////////////////////////////////////////////////
TSpatialOrientationError& TSpatialOrientationError::operator=(const TSpatialOrientationError& source)
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
TFreeVector TSpatialOrientationError::getVariances() const{

	TFreeVector var = fSigmaMtrx.getDiagCoefs();
	return var;
}

/////////////////////////////////////////////////////////
// Returns the variances/covariance matrix as a T3DMatrix
/////////////////////////////////////////////////////////
T3DMatrix TSpatialOrientationError::getCovariances() const{

	return fSigmaMtrx;
}

/////////////////////////////////////////
// Sets the covariances from a T3DMatrix
/////////////////////////////////////////
bool TSpatialOrientationError::setCovariances(const T3DMatrix& mx)
{
	fSigmaMtrx = mx;
	return true;
}

/////////////////////////////////////////
// Sets the variances from a TFreeVector
/////////////////////////////////////////
bool TSpatialOrientationError::setVariances(const TFreeVector& fv)
{
	fSigmaMtrx.setDiagCoefs(fv);
	return true;
}

