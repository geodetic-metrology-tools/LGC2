//  TLG2LATransformation.h
//
/** The transformation from a local geodetic reference frame to a 
    local astronomical reference frame.
	
    The deflection of the vertical, and the laplace correction at the
	origin of the reference frames define the transformation.


    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_LG_2_LA_TRANSFORMATION
#define SU_LG_2_LA_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <##classname##>
//using namespace std;
//
#include  "TCompositeAffTransform.h"
#include  "TARefFrameTransformation.h"
#include  "TModifiedLocalGeodeticRF.h"
#include  "TGraphLocalAstronomicalRF.h"
#include  "TAReferenceFrame.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! The transformation from a local geodetic reference frame to a local astronomical reference frame
class  TLG2LATransformation : public TARefFrameTransformation //: public TObject  
{
public:

	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TLG2LATransformation();

		/// Constructor taking pointers to the destination reference frames and geoid model
		TLG2LATransformation( TGraphLocalAstronomicalRF* to );

		/// Copy Constructor 
		TLG2LATransformation(const  TLG2LATransformation&);

		/// Destructor
		virtual  ~TLG2LATransformation();
	//@}


	/**@name Member Functions */
	//@{
		/// Copy Assignment Operator 
		TLG2LATransformation&				operator=( const TLG2LATransformation& );

		/// Return a pointer to a clone of this reference frame
		virtual TLG2LATransformation*	clone() const;

		/// Return a pointer to the inverse of this transformtion
		virtual TARefFrameTransformation*	inverse() const;

		/// Return the LG source frame
		virtual TAReferenceFrame*			getSourceFrame() const { return fFrom; }

		/// Return the LA destination frame
		virtual TAReferenceFrame*			getDestinationFrame() const { return fTo; }

		/// Set the LG source frame
		virtual void						setSourceFrame( TModifiedLocalGeodeticRF* );

		/// Set the LA destination frame
		virtual void						setDestinationFrame( TGraphLocalAstronomicalRF* );

		/// transform a position vector
		virtual  bool						transform( TPositionVector& pv ) const;

		/// transform a free vector
		virtual  bool						transform( TFreeVector& fv ) const;

		/// transform a rotation matrix
		virtual  bool						transform( TRotationMatrix& rmx ) const;

		bool								isInitialised()  const {return ( 0 != fFrom  &&  0 != fTo ); } 
	//@}


protected:
	/**@name Protected Member Functions */
	//@{
		/// Initialise the transformation using the parameters of the two reference frames
		virtual  void  initialise();

	//@}


private:

	
	virtual TModifiedLocalGeodeticRF*	getLGRF() const { return fFrom; }
	virtual TGraphLocalAstronomicalRF*	getLARF() const { return fTo; }

	
	TCompositeAffTransform*		fTransform;
	TModifiedLocalGeodeticRF*	fFrom;
	TGraphLocalAstronomicalRF*	fTo;

	//ClassDef(TLG2LATransformation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


#endif // SU_LG_2_LA_TRANSFORMATION
