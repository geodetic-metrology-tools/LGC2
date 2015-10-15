//  TGC2MLATransformation.h
//
/** The direct transformation from a modified local astronomical reference frame to a
    geodetic reference frame.
	
    The cooerdinates of the origin, and the laplace correction at the
	origin of the reference frames define the transformation.


    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_CG_2_IMLA_TRANSFORMATION
#define SU_CG_2_IMLA_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <##classname##>
//
#include  "TCompositeAffTransform.h"
#include  "TARefFrameTransformation.h"
#include  "TAModifiedLocalAstronomicalRF.h"
#include  "TGeodeticRefFrame.h"
#include  "TVReferenceFrame.h"


//using namespace std;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup spatialobjects
	@{*/

//! The direct transformation from a modified local astronomical reference frame to a geodetic reference frame
class  TGC2MLATransformation : public TARefFrameTransformation //: public TObject  
{
public:
	/**@name Constructors and Destructors */
	//@{
		/*! Default Constructor */
		TGC2MLATransformation();

		/*! Constructor taking pointers to the source and destination reference frames*/
		TGC2MLATransformation( TAModifiedLocalAstronomicalRF* to, TAGeoidModel* IMLARFGeoid );

		/*! Copy Constructor */
		TGC2MLATransformation(const  TGC2MLATransformation&);

		/*! Destructor*/
		virtual  ~TGC2MLATransformation();
	//@}


	/**@name Member Functions */
	//@{
		/*! Copy Assignment Operator */
		TGC2MLATransformation&				operator=( const TGC2MLATransformation& );

		/*! Return a pointer to a clone of this reference frame*/
		virtual TGC2MLATransformation*	clone() const;

		/*! Return a pointer to the inverse of this transformtion*/
		virtual TARefFrameTransformation*	inverse() const;

		/*! Return the LA source frame*/
		virtual TAReferenceFrame*			getSourceFrame() const { return fFrom; }

		/*! Return the LG destination frame*/
		virtual TAReferenceFrame*			getDestinationFrame() const { return fTo; }

		/*! Return the geoid*/
		virtual TAGeoidModel*				getGeoid() const { return fGeoid; }

		/*! Set the LA source frame*/
		virtual void						setSourceFrame( TGeodeticRefFrame* );

		/*! Set the LG destination frame*/
		virtual void						setDestinationFrame( TAModifiedLocalAstronomicalRF* );

		/*! Set the geoid*/
		virtual void						setGeoid( TAGeoidModel* );

		/*! test if the transformation is initialized*/
		bool								isInitialised()  const {return ( 0 != fFrom  &&  0 != fTo  &&  0 != fGeoid); } 

		/*! return the transformer*/
		TCompositeAffTransform*				getTransformer() const { return fTransform; }

	//@}



	/**@name Spatial System Transformation Functions */
	//@{
		/*! transform a position vector*/
		virtual  bool  transform( TPositionVector& pv ) const;

		/*! transform a free vector*/
		virtual  bool  transform( TFreeVector& fv ) const;

		/*! transform a Rotation Matrix*/
		virtual  bool  transform( TRotationMatrix& rmx ) const;
	//@}

		
//protected:
	/**@name  Member Functions */
	//@{
		/*! Initialise the transformation using the parameters of the two reference frames*/
		virtual  void  initialise();

	//@}


private:


	/**@name Protected Member Functions */
	//@{

		/*! return a pointer to the geodetic reference frame*/
		virtual TGeodeticRefFrame*				getGCRF() const { return fFrom; }
		/*! return a pointer to the modified reference frame*/
		virtual TAModifiedLocalAstronomicalRF*	getMLARF() const { return fTo; }

	//@}
	

	TCompositeAffTransform*			fTransform;/*!< Composite Affine transformation attribute*/
	TGeodeticRefFrame*				fFrom;/*!<  a pointer to the geodetic cartesian source RF*/
	TAModifiedLocalAstronomicalRF*	fTo;/*!< a pointer to the modified local astronomical destination RF*/
	TAGeoidModel*					fGeoid;/*!< a pointer to the geoid*/

	//ClassDef(TGC2MLATransformation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_LA_2_LG_TRANSFORMATION
