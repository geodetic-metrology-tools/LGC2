//  TXYHg2XYHeTransformation.h
//
/** The transformation from a XYHg reference frame to a XYHe reference frame.
	
    
    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_XYHE_2_XYHg_TRANSFORMATION
#define SU_XYHE_2_XYHg_TRANSFORMATION


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

/*!\ingroup spatialobjects
	@{*/

//! The transformation from a XYHg reference frame to a XYHe reference frame
class  TXYHg2XYHeTransformation : public TARefFrameTransformation //: public TObject  
{
public:
	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TXYHg2XYHeTransformation();

		/// Constructor taking pointers to the source and destination reference frames
		TXYHg2XYHeTransformation( TXYHgProjection* from );

		/// Copy Constructor 
		TXYHg2XYHeTransformation(const  TXYHg2XYHeTransformation&);

		/// Destructor
		virtual  ~TXYHg2XYHeTransformation();
	//@}


	/**@name Member Functions */
	//@{
		using TARefFrameTransformation::transform;

		/// Copy Assignment Operator 
		TXYHg2XYHeTransformation&			operator=( const TXYHg2XYHeTransformation& );

		/// Return a pointer to a clone of this reference frame
		virtual TXYHg2XYHeTransformation*	clone() const;

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
		virtual void						setSourceFrame( TXYHgProjection* );

		/// tranformation of a position vector between reference frames
		virtual  bool						transform( TPositionVector& pv ) const;
	//@}


private:

	/// Set the XYHe destination frame
	virtual void				setDestinationFrame( TXYHeProjection* xyhe) {fTo = xyhe; return;}

	/// Set the ellipsoid
	virtual void				setEllipsoid( TReferenceEllipsoid* ellipsoid) {fEllipsoid = ellipsoid; return;} 
		
	/// Set the Geoid
	virtual void				setGeoid( TAGeoidModel* geoid) {fGeoid = geoid; return;}

	virtual TXYHgProjection*	getXYHg() const { return fFrom; }
	virtual TXYHeProjection*	getXYHe() const { return fTo; }
	

	TXYHgProjection*					fFrom;
	TXYHeProjection*					fTo;
	TAGeoidModel*						fGeoid;
	TReferenceEllipsoid*				fEllipsoid;

	//ClassDef(TXYHg2XYHeTransformation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_XYHE_2_XYHg_TRANSFORMATION
