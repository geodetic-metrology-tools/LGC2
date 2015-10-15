//  TXYHe2XYHgTransformation.h
//
/** The transformation from a XYHe reference frame to a XYHg reference frame.
	
    
    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_XYHg_2_XYHe_TRANSFORMATION
#define SU_XYHg_2_XYHe_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <##classname##>
//
//using namespace system;
// #include  <cmath>
#include  "TCompositeAffTransform.h"
#include  "TARefFrameTransformation.h"
#include  "TXYHeProjection.h"
#include  "TAReferenceFrame.h"
#include  "TXYHgProjection.h"


using namespace std;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/


//! The transformation from a XYHe reference frame to a XYHg reference frame
class  TXYHe2XYHgTransformation : public TARefFrameTransformation //: public TObject  
{
public:
	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TXYHe2XYHgTransformation();

		/// Constructor taking pointers to the source and destination reference frames
		TXYHe2XYHgTransformation( TXYHgProjection* to );

		/// Copy Constructor 
		TXYHe2XYHgTransformation(const  TXYHe2XYHgTransformation&);

		/// Destructor
		virtual  ~TXYHe2XYHgTransformation();
	//@}


	/**@name Member Functions */
	//@{
		using TARefFrameTransformation::transform;

		/// Copy Assignment Operator 
		TXYHe2XYHgTransformation&			operator=( const TXYHe2XYHgTransformation& );

		/// Return a pointer to a clone of this reference frame
		virtual TXYHe2XYHgTransformation*	clone() const;

		/// Return a pointer to the inverse of this transformation
		virtual TARefFrameTransformation*	inverse() const;

		/// Return the source frame
		virtual TAReferenceFrame*			getSourceFrame() const { return fFrom; }

		/// Return the destination frame
		virtual TAReferenceFrame*			getDestinationFrame() const { return fTo; }

		/// Return the ellipsoid of the transformation
		virtual TReferenceEllipsoid*		getEllipsoid() const { return fEllipsoid; }

		/// Return the geoid of the transformation
		virtual TAGeoidModel*				getGeoid() const { return fGeoid; }

		/// Set the XYHe source frame
		virtual void						setDestinationFrame( TXYHgProjection* xyhg) {fTo = xyhg; return; }

		/// tranformation of a position vector between reference frames
		virtual  bool						transform( TPositionVector& pv ) const;
	//@}


protected:
	

private:

	/// Set the XYHe destination frame
	virtual void				setSourceFrame( TXYHeProjection* xyhe) {fFrom = xyhe; return;}

	/// Set the ellipsoid
	virtual void				setEllipsoid( TReferenceEllipsoid* ellipsoid) {fEllipsoid = ellipsoid; return;} 
		
	/// Set the Geoid
	virtual void				setGeoid( TAGeoidModel* geoid) {fGeoid = geoid;	return;}

	virtual TXYHeProjection*	getXYHe() const { return fFrom; }
	virtual TXYHgProjection*	getXYHg() const { return fTo; }

	

	TXYHeProjection*		fFrom;
	TXYHgProjection*		fTo;
	TAGeoidModel*			fGeoid;
	TReferenceEllipsoid*	fEllipsoid;

	//ClassDef(TXYHe2XYHgTransformation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_XYHg_2_XYHe_TRANSFORMATION
