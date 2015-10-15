//  TGC2LGTransformation.h
//
/** The transformation from a geodetic reference frame to a
    local geodetic reference frame.
	
    The transformation is defined with the origin 
	of the local geodetic reference frame.


    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_GC_2_LG_TRANSFORMATION
#define SU_GC_2_LG_TRANSFORMATION


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
#include  "TVReferenceFrame.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! The transformation from a geodetic reference frame to a local geodetic reference frame
class  TGC2LGTransformation : public TARefFrameTransformation //: public TObject  
{
public:
	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TGC2LGTransformation();

		/// Constructor taking pointers to the source reference frame and geoid model
		TGC2LGTransformation( TModifiedLocalGeodeticRF* to );

		/// Copy Constructor 
		TGC2LGTransformation(const  TGC2LGTransformation&);

		/// Destructor
		virtual  ~TGC2LGTransformation();
	//@}


	/**@name Member Functions */
	//@{
		/// Copy Assignment Operator 
		TGC2LGTransformation&				operator=( const TGC2LGTransformation& );

		/// Return a pointer to a clone of this reference frame
		virtual TGC2LGTransformation*	clone() const;

		/// Return a pointer to the inverse of this transformtion
		virtual TARefFrameTransformation*	inverse() const;

		/// Return the GC source frame
		virtual TAReferenceFrame*			getSourceFrame() const { return fFrom; }

		/// Return the LG destination frame
		virtual TAReferenceFrame*			getDestinationFrame() const { return fTo; }

		/// Set the GC source frame
		virtual void						setSourceFrame( TGeodeticRefFrame* );

		/// Set the LG destination frame
		virtual void						setDestinationFrame( TModifiedLocalGeodeticRF* );

		/// transform a position vector
		virtual  bool						transform( TPositionVector& pv ) const;

		/// transform a free vector
		virtual  bool						transform( TFreeVector& fv ) const;

		/// transform a Rotation Matrix
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

	virtual TGeodeticRefFrame*			getGCRF() const { return fFrom; }
	virtual TModifiedLocalGeodeticRF*	getLGRF() const { return fTo; }


	TCompositeAffTransform*		fTransform;
	TGeodeticRefFrame*			fFrom;
	TModifiedLocalGeodeticRF*	fTo;

	//ClassDef(TGC2LGTransformation, 1)
};
/*@}*/

#endif // SU_GC_2_LG_TRANSFORMATION
