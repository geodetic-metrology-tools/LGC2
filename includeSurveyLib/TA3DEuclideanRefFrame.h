// TA3DEuclideanRefFrame.h
//
/*!  
   Patterns:
   An Abstract STATE of a Spatial Position
   
   Copyright 2000 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////



#ifndef SU_A_3D_EUCLIDEAN_RF
#define SU_A_3D_EUCLIDEAN_RF


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
#include  <set>
#include  "TSpatialPosition.h"
#include  "TSpatialVector.h"

#include "TAReferenceFrame.h"

//
//class  TVTransformation;
//#include  "TA2DXYRefFrame.h"

//#include "TVReferenceFrame.h"
//using namespace std;
// typedefs
//
//
//////////////////////////////////////////////////////////////!/

/*! \ingroup spatialobjects
	@{*/

//! An abstract base class for a reference frame.
class  TA3DEuclideanRefFrame : public  TAReferenceFrame //: public TObject  
{
public:
	 
	/*!@name Constructors and Destructors */
	//@{
		//! Constructor taking the name of the reference frame
		explicit TA3DEuclideanRefFrame( const string& name );

		//! Destructor
		virtual  ~TA3DEuclideanRefFrame();
	//@}

		using TAReferenceFrame::getOrientation;

	/*!@name default definition of TVReferenceFrame's Data extraction methods*/
	//@{
		//! return the position vector of a spatial position 
		virtual TPositionVector getCoordinates(const TSpatialPosition* sp, TCoordSysFactory::ECoordSys) const;
		//! return the free vector of a spatial vector
		virtual TFreeVector getElements(const TSpatialVector* sv, TCoordSysFactory::ECoordSys) const;
		//! return the rotation matrix of a spatial orientation
		virtual TRotationMatrix getElements(const TSpatialOrientation* so, TCoordSysFactory::ECoordSys ) const;
	//@}

	/*!@name default definition of TVReferenceFrame's Settings methods*/
	//@{
		//! set the position vector of a spatial position 
		virtual bool setCoordinates(TSpatialPosition* sp, const TPositionVector& pv);
		//! set the free vector of a spatial vector
		virtual bool setElements(TSpatialVector* sv, const TFreeVector& fv);
		//! set the rotation matrix of a spatial orientation
		virtual bool setElements(TSpatialOrientation* so, const TRotationMatrix& rm);
	//@}


	
protected:
	
private:

	// Constructors and Destructors
	// Copy Constructor 
	TA3DEuclideanRefFrame( const  TA3DEuclideanRefFrame& );


	// Private Member Functions
	// Copy Assignment Operator
	TA3DEuclideanRefFrame& operator=( const TA3DEuclideanRefFrame& );


private:

	//ClassDef(TAReferenceFrame, 1)
};

/*@}*/
////////////////////////////////////////////////////////////////////
// Inline Definitions
////////////////////////////////////////////////////////////////////






#endif // SU_A_REFERENCE_FRAME
