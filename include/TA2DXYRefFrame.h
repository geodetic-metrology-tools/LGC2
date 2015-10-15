// TA2DXYRefFrame.h
//
/*  
   Patterns:
   An Abstract STATE of a Spatial Position
   
   Copyright 2000 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////



#ifndef SU_A_2D_XY_RF
#define SU_A_2D_XY_RF


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

#include  "TAReferenceFrame.h"
//using namespace std;
//
//class  TVTransformation;
// typedefs
//
//
//////////////////////////////////////////////////////////////!/

/*! \ingroup spatialobjects 
	@{*/

//! An abstract base class for a reference frame.
class  TA2DXYRefFrame : public  TAReferenceFrame //: public TObject  
{
public:
	 
	/*!@name Constructors and Destructors */
	//@{
		//! Constructor taking the name of the reference frame
		TA2DXYRefFrame( const string& name );

		//! Destructor
		virtual  ~TA2DXYRefFrame();
	//@}


	/*!@name Access methods*/
	//@{

		//! Get the name of the reference frame
		//virtual string  getName() const;

		/*! Add a transformation for transforming objects from this 
		    reference frame to another 
		 */
		//void  insert( TVTransformation*  transformer );

		//! Return an Iterator to the beginning of the set of Transformations
		//TVReferenceFrame::iterator  begin() const;

		//! Return an Iterator to the end of the set of Transformations
		//TVReferenceFrame::iterator  end() const;


		//! returns the X coordinate if defined for the reference frame
		virtual TLength  getXCoord( const TSpatialPosition* ) const;
		//! returns the Y coordinate if defined for the reference frame
		virtual TLength  getYCoord( const TSpatialPosition* ) const;

		//! returns the X component
		virtual TLength  getXComp( const TSpatialVector* ) const;
		//! returns the Y component
		virtual TLength  getYComp( const TSpatialVector* ) const;
		/*!@name Set methods*/
		//@{
		//! sets the X Cartesian coordinate of the given Spatial Position if defined for the Reference Frame
		virtual bool  setXCoord( TSpatialPosition*, const TLength & );  
		//! sets the Y Cartesian coordinate of the given Spatial Position if defined for the Reference Frame
		virtual bool  setYCoord( TSpatialPosition*, const TLength & );  

		//! sets the X Component of the given Spatial Vector
		virtual bool  setXComp( TSpatialVector*, const TLength & );  
		//! sets the Y Component of the given Spatial Vector
		virtual bool  setYComp( TSpatialVector*, const TLength & ); 

		
/*		//! transforms a spatial position with an affine transformation, if possible
		bool transform( TSpatialPosition* sp, TAAffineTransformation* trans);

		//! transforms a spatial vector with an affine transformation, if possible
		bool transform( TSpatialVector* sv, TAAffineTransformation* trans);

		//! transforms a spatial orientation with an affine transformation, if possible
		bool transform( TSpatialOrientation* so, TAAffineTransformation* trans);
*/
		//@}
		


protected:
	
	

private:

	// Constructors and Destructors
	// Copy Constructor 
	TA2DXYRefFrame( const  TA2DXYRefFrame& );


	// Private Member Functions
	// Copy Assignment Operator
	TA2DXYRefFrame&  TA2DXYRefFrame::operator=( const TA2DXYRefFrame& );


private:

//	string  fName;
//	TransformationSet  fTransformations;

	//ClassDef(TAReferenceFrame, 1)
};

/*@}*/
////////////////////////////////////////////////////////////////////
// Inline Definitions
////////////////////////////////////////////////////////////////////






#endif // SU_A_REFERENCE_FRAME
