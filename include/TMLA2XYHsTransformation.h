///////////////////////////////////////////////////////////////////////
//TMLA2XYHsTransformation.h
/*
The transformation from a modified local
astronomical reference frame to a XYHs projection.

Patterns:
   
Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_MLA_2_XYHS_TRANSFORMATION
#define SU_MLA_2_XYHS_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
// #include  <cmath>
#include  "TARefFrameTransformation.h"
#include  "TAModifiedLocalAstronomicalRF.h"
#include  "TXYHeProjection.h"
#include  "TAReferenceFrame.h"

//using namespace std;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects

@{*/

//!The transformation from a modified local astronomical reference frame to a XYHs projection
class  TMLA2XYHsTransformation : public TARefFrameTransformation //: public TObject  
{
public:
	/*!@name Constructors and Destructors */
	//@{
		//!Default Constructor 
		TMLA2XYHsTransformation();

		//!Constructor taking pointers to the source and destination reference frames
		TMLA2XYHsTransformation( TXYHeProjection* to );

		//!Copy Constructor 
		TMLA2XYHsTransformation(const  TMLA2XYHsTransformation&);

		//!Destructor
		virtual  ~TMLA2XYHsTransformation();
	//@}


	/*!@name Member Functions */
	//@{
		using TARefFrameTransformation::transform;
		//using TARefFrameTransformation::inverse;

		//!Copy Assignment Operator 
		TMLA2XYHsTransformation&			operator=( const TMLA2XYHsTransformation& );

		//!Return a pointer to a clone of this reference frame
		virtual TMLA2XYHsTransformation *	clone() const;

		//!Return a pointer to the inverse of this transformtion
		virtual TARefFrameTransformation *  inverse() const;

		//!Return the source frame
		virtual TAReferenceFrame*			getSourceFrame() const { return fFrom; }

		//!Return the destination frame
		virtual TAReferenceFrame*			getDestinationFrame() const { return fTo; }

		//!Return the ellipsoid of the transformation
		virtual TReferenceEllipsoid*		getEllipsoid() const { return fEllipsoid; }

		//!Set the XYHs source frame
		virtual void						setDestinationFrame( TXYHeProjection* XYHs) {fTo = XYHs; return;}

		//! tranformation of a position vector between reference frames
		virtual bool						transform(TPositionVector& pv) const;
	//@}


private:
	/*!@name Private Member Functions */
	//@{
		//!Set the MLA destination frame
		virtual void							setSourceFrame( TAModifiedLocalAstronomicalRF* LA) {fFrom = LA; return;}

		//!Set the ellipsoid
		virtual void							setEllipsoid( TReferenceEllipsoid* ellipsoid) {fEllipsoid = ellipsoid; return;}

		//!get the MLA destination frame
		virtual TAModifiedLocalAstronomicalRF*	getMLARF() const { return fFrom; }

		//!get the MLA destination projection
		virtual TXYHeProjection*				getXYHs() const { return fTo; }
	//@}	
	
	/*!@name Private Attributs */
	//@{
		//!pointer to a TAModifiedLocalAstronomicalRF object
		TAModifiedLocalAstronomicalRF*		fFrom;

		//!pointer to a TXYHeProjection object
		TXYHeProjection*					fTo;

		//!pointer to a TReferenceEllipsoid object
		TReferenceEllipsoid*				fEllipsoid;
	//@}

	//ClassDef(TMLA2XYHsTransformation, 1)
};

/*@}*/

#endif // SU_MLA_2_XYHS_TRANSFORMATION
