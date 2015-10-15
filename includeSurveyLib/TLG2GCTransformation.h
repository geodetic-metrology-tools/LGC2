//  TLG2GCTransformation.h
//
/** The transformation from a geodetic reference frame to a
    local geodetic reference frame.
	
    The transformation is defined with the origin 
	of the local geodetic reference frame.


    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_LG_2_GC_TRANSFORMATION
#define SU_LG_2_GC_TRANSFORMATION


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
#include  "TGeodeticRefFrame.h"
#include  "TModifiedLocalGeodeticRF.h"
#include  "TAReferenceFrame.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! The transformation from a geodetic reference frame to a local geodetic reference frame
class  TLG2GCTransformation : public TARefFrameTransformation //: public TObject  
{
public:
	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TLG2GCTransformation();

		/// Constructor taking pointers to the source reference frame and geoid model
		TLG2GCTransformation( TModifiedLocalGeodeticRF* from );

		/// Copy Constructor 
		TLG2GCTransformation(const  TLG2GCTransformation&);

		/// Destructor
		virtual  ~TLG2GCTransformation();
	//@}


	/**@name Member Functions */
	//@{
		/// Copy Assignment Operator 
		TLG2GCTransformation&				operator=( const TLG2GCTransformation& );

		/// Return a pointer to a clone of this reference frame
		virtual TLG2GCTransformation*	clone() const;

		/// Return a pointer to the inverse of this transformtion
		virtual TARefFrameTransformation*	inverse() const;

		/// Return the GC source frame
		virtual TAReferenceFrame*			getSourceFrame() const { return fFrom; }

		/// Return the LG destination frame
		virtual TAReferenceFrame*			getDestinationFrame() const { return fTo; }

		/// Set the GC source frame
		virtual void						setSourceFrame( TModifiedLocalGeodeticRF* );

		/// Set the LG destination frame
		virtual void						setDestinationFrame( TGeodeticRefFrame* );

		/// transform a position vector
		virtual  bool						transform( TPositionVector& pv ) const;

		/// transform a free vector
		virtual  bool						transform( TFreeVector& fv ) const;

		/// transform a rotation matrix
		virtual  bool						transform( TRotationMatrix& rmx ) const;

		bool								isInitialised()  const {return ( 0 != fFrom  &&  0 != fTo ); } 

		TCompositeAffTransform*				getTransformer() const { return fTransform; }

	//@}


protected:
	/**@name Protected Member Functions */
	//@{
		/// Initialise the transformation using the parameters of the two reference frames
		virtual  void  initialise();

	//@}


private:

	virtual TGeodeticRefFrame*			getGCRF() const { return fTo; }
	virtual TModifiedLocalGeodeticRF*	getLGRF() const { return fFrom; }

	TCompositeAffTransform*		fTransform;
	TModifiedLocalGeodeticRF*	fFrom;
	TGeodeticRefFrame*			fTo;


	//ClassDef(TLG2GCTransformation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


#endif // SU_LG_2_GC_TRANSFORMATION
