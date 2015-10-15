//  TLA2LGTransformation.h
//
/** The transformation from a local astronomical reference frame to a
    local geodetic reference frame.
	
    The deflection of the vertical, and the laplace correction at the
	origin of the reference frames define the transformation.


    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_LA_2_LG_TRANSFORMATION
#define SU_LA_2_LG_TRANSFORMATION


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
#include  "TGraphLocalAstronomicalRF.h"
#include  "TModifiedLocalGeodeticRF.h"
#include  "TAReferenceFrame.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! The transformation from a local astronomical reference frame to a local geodetic reference frame
class  TLA2LGTransformation : public TARefFrameTransformation //: public TObject  
{
public:
	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TLA2LGTransformation();

		/// Constructor taking pointers to the source reference frame and geoid model
		TLA2LGTransformation( TGraphLocalAstronomicalRF* from );

		/// Copy Constructor 
		TLA2LGTransformation(const  TLA2LGTransformation&);

		/// Destructor
		virtual  ~TLA2LGTransformation();
	//@}


	/**@name Member Functions */
	//@{
		/// Copy Assignment Operator 
		TLA2LGTransformation&				operator=( const TLA2LGTransformation& );

		/// Return a pointer to a clone of this reference frame
		virtual TLA2LGTransformation*	clone() const;

		/// Return a pointer to the inverse of this transformtion
		virtual TARefFrameTransformation*	inverse() const;

		/// Return the LA source frame
		virtual TAReferenceFrame*			getSourceFrame() const { return fFrom; }

		/// Return the LG destination frame
		virtual TAReferenceFrame*			getDestinationFrame() const { return fTo; }

		/// Set the LA source frame
		virtual void						setSourceFrame( TGraphLocalAstronomicalRF* );

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



	virtual TGraphLocalAstronomicalRF*	getLARF() const { return fFrom; }
	virtual TModifiedLocalGeodeticRF*	getLGRF() const { return fTo; }

	TCompositeAffTransform*			fTransform;
	TGraphLocalAstronomicalRF*		fFrom;
	TModifiedLocalGeodeticRF*		fTo;


	//ClassDef(TLA2LGTransformation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_LA_2_LG_TRANSFORMATION
