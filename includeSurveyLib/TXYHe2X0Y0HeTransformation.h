//  TXYHe2X0Y0HeTransformation.h
//
/** The transformation from a XYHe projection to a X0Y0He reference frame.
	
    
    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_XYHE_2_X0Y0He_TRANSFORMATION
#define SU_XYHE_2_X0Y0He_TRANSFORMATION


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
//#include  "TCompositeAffTransform.h"
#include  "TARefFrameTransformation.h"
#include  "TXYHeProjection.h"
#include  "TAReferenceFrame.h"
#include  "TX0Y0HeProjection.h"


using namespace std;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects	
	@{*/

//! The transformation from a XYHe projection to a X0Y0He reference frame
class  TXYHe2X0Y0HeTransformation : public TARefFrameTransformation //: public TObject  
{
public:
	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TXYHe2X0Y0HeTransformation();

		/// Constructor taking pointers to the source and destination reference frames
		TXYHe2X0Y0HeTransformation( TX0Y0HeProjection* to );

		/// Copy Constructor 
		TXYHe2X0Y0HeTransformation(const  TXYHe2X0Y0HeTransformation&);

		/// Destructor
		virtual  ~TXYHe2X0Y0HeTransformation();
	//@}


	/**@name Member Functions */
	//@{
		using TARefFrameTransformation::transform;

		/// Copy Assignment Operator 
		TXYHe2X0Y0HeTransformation&			operator=( const TXYHe2X0Y0HeTransformation& );

		/// Return a pointer to a clone of this reference frame
		virtual TXYHe2X0Y0HeTransformation*	clone() const;

		/// Return a pointer to the inverse of this transformation
		virtual TARefFrameTransformation*	inverse() const;

		/// Return the source frame
		virtual TAReferenceFrame*			getSourceFrame() const { return fFrom; }

		/// Return the destination frame
		virtual TAReferenceFrame*			getDestinationFrame() const { return fTo; }

		/// Return the ellipsoid of the transformation
		virtual TReferenceEllipsoid*		getEllipsoid() const { return fEllipsoid; }

		/// Set the XYHe source frame
		virtual void						setSourceFrame( TXYHeProjection* xyhe) {fFrom = xyhe; return;}

		//! transformation of a position vector between reference frames
		virtual bool						transform(TPositionVector& pv) const;
	//@}


protected:
	

private:

	/// Set the XYHe destination frame
	virtual void				setDestinationFrame( TX0Y0HeProjection* x0y0he) {fTo = x0y0he; return;}

	/// Set the ellipsoid
	virtual void				setEllipsoid( TReferenceEllipsoid* ellipsoid) {fEllipsoid = ellipsoid; return;}

	virtual TXYHeProjection*	getXYHe() const { return fFrom; }
	virtual TX0Y0HeProjection*	getX0Y0He() const { return fTo; }
	

	TXYHeProjection*					fFrom;
	TX0Y0HeProjection*					fTo;
	TReferenceEllipsoid*				fEllipsoid;

	//ClassDef(TXYHe2X0Y0HeTransformation, 1)
};

/*@}*/
//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_XYHE_2_X0Y0He_TRANSFORMATION
