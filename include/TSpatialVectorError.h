// TSpatialVectorError.h: 
//
/*  
   Patterns:
  
   
   Copyright 1999,2000 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////




#ifndef SU_SPATIAL_VECTOR_ERROR
#define SU_SPATIAL_VECTOR_ERROR

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////

//class  TAngle;
//class  TReferenceEllipsoid;
class  TVReferenceFrame;
#include "T3DMatrix.h"

#include  "TANumericValue.h"
#include  "TLength.h"
#include  "TAngle.h"
#include "counted_ptr.h"

#include  "TSpatialStatus.h"

using namespace std;

// typedefs
typedef  TReal  Component;
typedef  bool  CompKnown;
//
//
/////////////////////////////



//! The Spatial Vector Error of a point in a given reference frame.
class TSpatialVectorError  //: public TObject  
{

public:
	friend  class  TAReferenceFrame;

	//typedef  TANumericValue::EStatus  ESigmaStatus;

	/*!@name Constructors and Destructors */
	//@{
//		explicit  TSpatialVectorError( TReferenceFrameServer::ERefFrame );
	explicit  TSpatialVectorError( TCoordSysFactory::ECoordSys );
//		TSpatialVectorError( TSpatialVector*, counted_ptr< TSpatialStatus> );
//	TSpatialVectorError();	
	TSpatialVectorError( const TSpatialVectorError& );
	virtual ~TSpatialVectorError();
	//@}

	//! Copy Assignment Operator 
	TSpatialVectorError& operator=(const TSpatialVectorError&);

	/*!@return the variances as a TFreeVector*/
	TFreeVector				getVariances() const;

	/*!@return the variance/covariance as a T3DMatrix*/
	T3DMatrix				getCovariances() const;

	//! set the variances from a 3D-matrix and 
	bool setVariances(const TFreeVector& fv) ;
	//! set the covariances form a 3D-matrix
	bool setCovariances(const T3DMatrix& mx) ;
	//! return the reference frame of this spatial Vector error
//	TVReferenceFrame*       getReferenceFrame() const;


		
		
		/*! sets the sigmaXX (cartesian) if defined in the Vector's Reference Frame
			@return true if the error is set */
//		bool					setSigmaXX( const TLength & );  
		/*! sets the sigmaYY (cartesian) if defined in the Vector's Reference Frame
			@return true if the error is set */
//		bool					setSigmaYY( const TLength & );  
		/*! sets the sigmaZZ (cartesian) if defined in the Vector's Reference Frame
			@return true if the error is set */
//		bool					setSigmaZZ( const TLength & );  
		/*! sets the sigmaXY (cartesian) if defined in the Vector's Reference Frame
			@return true if the error is set */
//		bool					setSigmaXY( const TLength & );  
		/*! sets the sigmaXZ (cartesian) if defined in the Vector's Reference Frame
			@return true if the error is set */
//		bool					setSigmaXZ( const TLength & );  
		/*! sets the sigmaYZ (cartesian) if defined in the Vector's Reference Frame
			@return true if the error is set */
//		bool					setSigmaYZ( const TLength & );  
		//!Sets all the sigmas to zero
//		void					setToZero();



		//! sets the X Cartesian coordinate error status if defined in the Vector's Reference Frame
//		virtual void			setSigmaXStatus(TSpatialStatus::EAxisStatus);
		//! sets the Y Cartesian coordinate error status if defined in the Vector's Reference Frame
//		virtual void			setSigmaYStatus(TSpatialStatus::EAxisStatus);
		//! sets the Z Cartesian coordinate error status if defined in the Vector's Reference Frame
//		virtual void			setSigmaZStatus(TSpatialStatus::EAxisStatus);
		//! sets the 3D Cartesian coordinates status (by copy) if defined in the Vector's Reference Frame
//		virtual void			setVectorStatus(counted_ptr< TSpatialStatus > );




		//! returns the sigmaXX error if defined for the Vector's reference frame
//		TLength 				getSigmaXX() const;
		//! returns the sigmaYY error if defined for the Vector's reference frame
//		TLength 				getSigmaYY() const;
		//! returns the sigmaZZ error if defined for the Vector's reference frame
//		TLength 				getSigmaZZ() const;
		//! returns the sigmaXY error if defined for the Vector's reference frame
//		TLength 				getSigmaXY() const;
		//! returns the sigmaXZ error if defined for the Vector's reference frame
//		TLength 				getSigmaXZ() const;
		//! returns the sigmaYZ error if defined for the Vector's reference frame
//		TLength 				getSigmaYZ() const;


		//! sets the error's associated TSpatialVector
//		void					setAssociatedVector(TSpatialVector* sp);

		//! sets the error's status accordingly to the status of the associated Vector
//		void		refreshStatus();

	


protected:

		//! returns the X coordinate status if defined for the Vector's reference frame
//		virtual TSpatialStatus::EAxisStatus	getSigmaXStatus();
		//! returns the Y coordinate status if defined for the Vector's reference frame
//		virtual TSpatialStatus::EAxisStatus	getSigmaYStatus();
		//! returns the Z coordinate status if defined for the Vector's reference frame
//		virtual TSpatialStatus::EAxisStatus	getSigmaZStatus();


private:
	//private functions
	//! sets the sigma11 error
//	void  setSigma11( TReal x )  { fSigmaMtrx(1,1) = x;  return; }
	//! sets the sigma22 error
//	void  setSigma22( TReal x )  { fSigmaMtrx(2,2) = x; return; }
	//! sets the sigma33 error
//	void  setSigma33( TReal x )  { fSigmaMtrx(3,3) = x; return; }
	//! sets the sigma12 (and sigma 21) error
//	void  setSigma12( TReal x ) { fSigmaMtrx(1,2) = x; fSigmaMtrx(2,1) = x; return;}
	//! sets the sigma13 (and sigma 31) error
//	void  setSigma13( TReal x ) { fSigmaMtrx(1,3) = x; fSigmaMtrx(3,1) = x; return;}
	//! sets the sigma23 (and sigma 32) error
//	void  setSigma23( TReal x ) { fSigmaMtrx(2,3) = x; fSigmaMtrx(3,2) = x; return;}

	
	//! gets the sigma11
//	TLength  getSigma11() const  { return  getCovariances()(1,1); }
	//! gets the sigma22
//	TLength  getSigma22() const  { return  getCovariances()(2,2); }
	//! gets the sigma33
//	TLength  getSigma33() const  { return  getCovariances()(3,3); }
	//! gets the sigma12
//	TLength  getSigma12() const  { return  getCovariances()(1,2); }
	//! gets the sigma13
//	TLength  getSigma13() const  { return  getCovariances()(1,3); }
	//! gets the sigma23
//	TLength  getSigma23() const  { return  getCovariances()(2,3); }


	/*!@return a pointer to the TSpatialVectorStatus*/
//	TSpatialStatus*		getStatus();

private:
	// member attributes
	T3DMatrix  fSigmaMtrx;

//	counted_ptr< TSpatialStatus >  fSigmaStatus;

//	TVReferenceFrame*  fRefFrame;

//	TSpatialVector*  fAssociatedVector;
	
};


/////////////////////
// Inline Definitions
/////////////////////

//inline TVReferenceFrame* TSpatialVectorError::getReferenceFrame() const{return fRefFrame;}

//inline void TSpatialVectorError::setAssociatedVector(TSpatialVector* sp){ fAssociatedVector = sp;}

#endif // SU_SPATIAL_VECTOR_ERROR
