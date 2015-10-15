//  TXYHe2XYHeTransformation.h
//
/** The transformation from a X0Y0He projection to XYHe projection.
	
    
    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_X0Y0HE_2_XYHe_TRANSFORMATION
#define SU_X0Y0HE_2_XYHe_TRANSFORMATION


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
#include  "TX0Y0HeProjection.h"


using namespace std;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/


//! The transformation from a XYHe projection to XYHe projection
class  TX0Y0He2XYHeTransformation : public TARefFrameTransformation //: public TObject  
{
public:
	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TX0Y0He2XYHeTransformation();

		/// Constructor taking pointers to the source and destination reference frames
		TX0Y0He2XYHeTransformation( TX0Y0HeProjection* from );

		/// Copy Constructor 
		TX0Y0He2XYHeTransformation(const  TX0Y0He2XYHeTransformation&);

		/// Destructor
		virtual  ~TX0Y0He2XYHeTransformation();
	//@}


	/**@name Member Functions */
	//@{
		using TARefFrameTransformation::transform;

		/// Copy Assignment Operator 
		TX0Y0He2XYHeTransformation &  operator=( const TX0Y0He2XYHeTransformation& );

		/// Return a pointer to a clone of this reference frame
		virtual TX0Y0He2XYHeTransformation *  clone() const;

		/// Return a pointer to the inverse of this transformation
		virtual TARefFrameTransformation *  inverse() const;

		/// Return the source frame
		virtual TAReferenceFrame *  getSourceFrame() const { return fFrom; }

		/// Return the destination frame
		virtual TAReferenceFrame *  getDestinationFrame() const { return fTo; }

		/// Return the ellipsoid of the transformation
		virtual TReferenceEllipsoid *  getEllipsoid() const { return fEllipsoid; }

		/// Set the XYHe source frame
		virtual void  setSourceFrame( TX0Y0HeProjection* );

		//! tranformation of a position vector between reference frames
		virtual bool  transform(TPositionVector& pv) const;
	//@}

private:

	/// Set the XYHe destination frame
	virtual void				setDestinationFrame( TXYHeProjection* );

	/// Set the ellipsoid
	virtual void				setEllipsoid( TReferenceEllipsoid* );

	virtual TXYHeProjection*	getXYHe() const { return fTo; }
	virtual TX0Y0HeProjection*	getX0Y0He() const { return fFrom; }
	

	
	TX0Y0HeProjection*		fFrom;
	TXYHeProjection*		fTo;
	TReferenceEllipsoid*	fEllipsoid;

	//ClassDef(TX0Y0He2XYHeTransformation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_X0Y0HE_2_XYHe_TRANSFORMATION
