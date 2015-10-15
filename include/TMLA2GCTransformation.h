//  TMLA2GCTransformation.h
//
/*! The direct transformation from a modified local astronomical reference frame to a
    geodetic reference frame.
	
    The coordinates of the origin, and the laplace correction at the
	origin of the reference frames define the transformation.

   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/



#ifndef SU_MLA_2_CG_TRANSFORMATION
#define SU_MLA_2_CG_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



// #include  <cmath>
#include  "TCompositeAffTransform.h"
#include  "TARefFrameTransformation.h"
#include  "TAModifiedLocalAstronomicalRF.h"
#include  "TGeodeticRefFrame.h"
#include  "TAReferenceFrame.h"


//using namespace std;

/*!\ingroup spatialobjects
	@{*/

//! The direct transformation from a modified local astronomical reference frame to a geodetic reference frame
class  TMLA2GCTransformation : public TARefFrameTransformation //: public TObject  
{
public:
	/**@name Constructors and Destructors */
	//@{
		/*! Default Constructor*/ 
		TMLA2GCTransformation();

		/*! Constructor taking pointers to the source and destination reference frames*/
		TMLA2GCTransformation( TAModifiedLocalAstronomicalRF* from, TAGeoidModel* IMLARFGeoid );

		/*! Copy Constructor */
		TMLA2GCTransformation(const  TMLA2GCTransformation&);

		/*! Destructor*/
		virtual  ~TMLA2GCTransformation();
	//@}


	/**@name Member Functions */
	//@{
		/*! Copy Assignment Operator */
		TMLA2GCTransformation&				operator=( const TMLA2GCTransformation& );

		/*! Return a pointer to a clone of this reference frame*/
		virtual TMLA2GCTransformation*	clone() const;

		/*! Return a pointer to the inverse of this transformtion*/
		virtual TARefFrameTransformation*	inverse() const;

		/*! Return the LA source frame*/
		virtual TAReferenceFrame*			getSourceFrame() const { return fFrom; }

		/*! Return the LG destination frame*/
		virtual TAReferenceFrame*			getDestinationFrame() const { return fTo; }

		/*! Set the LA source frame*/
		virtual void						setSourceFrame( TAModifiedLocalAstronomicalRF* );

		/*! Set the LG destination frame*/
		virtual void						setDestinationFrame( TGeodeticRefFrame* );

		/*! Set the geoid*/
		virtual void						setGeoid( TAGeoidModel* );

		/*! ckeck if the transformation is initialized or not */
		bool								isInitialised()  const {return ( 0 != fFrom  &&  0 != fTo &&  0 != fGeoid); } 

		/*! return the composite affine transformation of this reference frame transformation */
		TCompositeAffTransform*				getTransformer() const { return fTransform; }

	//@}


	/**@name Spatial Systems Transformation Functions   */
	//@{
		/*! transform a position vector*/
		virtual  bool  transform( TPositionVector& pv ) const;

		/*! transform a free vector*/
		virtual  bool  transform( TFreeVector& fv ) const;

		/*! transform a Rotation Matrix*/
		virtual  bool  transform( TRotationMatrix& rmx ) const;
	//@}


//protected:
	/**@name Member Functions */
	//@{
		/*! Initialise the transformation using the parameters of the two reference frames*/
		virtual  void  initialise();

	//@}


private:


	/**@name Protected Member Functions */
	//@{
		/*! return the modified local astronomical reference frame */
		virtual TAModifiedLocalAstronomicalRF* getMLARF() const { return fFrom; }

		/*! return the geodetic cartesian reference frame */
		virtual TGeodeticRefFrame* getGCRF() const { return fTo; }

		/*! return geoid */
		virtual TAGeoidModel* getGeoid() const { return fGeoid; }
	//@}


	TCompositeAffTransform*			fTransform;/*!< Composite Affine transformation attribute*/
	TAModifiedLocalAstronomicalRF*	fFrom;/*!< a pointer to the modified local astronomical source RF*/
	TGeodeticRefFrame*				fTo;/*!<  a pointer to the geodetic cartesian destination RF*/
	TAGeoidModel*					fGeoid;/*!< a pointer to the geoid*/

	//ClassDef(TMLA2GCTransformation, 1)
};

/*@}*/

#endif // SU_MLA_2_GC_TRANSFORMATION
