//  TXYHeProjection.h
//
/** A projection on XY plane with ellipsoidal height 

  
    Patterns:
    A concrete STATE of a Spatial Position
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_XYHe_PROJECTION
#define SU_XYHe_PROJECTION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
//
class  TSpatialPosition;
class  TSpatialVector;
class  TReferenceEllipsoid;
class  TAModifiedLocalAstronomicalRF;
#include  "TA2DPlus1MapProjection.h"
using namespace std;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! A projection on XY plane with ellipsoidal height
class  TXYHeProjection : public TA2DPlus1MapProjection //: public TObject  
{
public:
	
	/**@name Constructors and Destructors */
	//@{
		/// Default constructor
		TXYHeProjection();

		/// Constructor taking the name, the reference ellipsoid and the
		/// modified local astronomical system corresponding to the reference frame
		TXYHeProjection( const string& name, TReferenceEllipsoid* ellipsoid,
			TAModifiedLocalAstronomicalRF* ccs);

		/// Destructor
		virtual  ~TXYHeProjection();
	//@}


	/**@name Member Functions */
	//@{

		//! return the modidfied local astronomical reference frame
		virtual TAModifiedLocalAstronomicalRF* getMLARefFrame() { return fMLASystem; }

		//! return the reference ellipsoid of the projection
		virtual TReferenceEllipsoid* getEllipsoid() { return fEllipsoid; }
	//@}


protected:


private:
	// We don't want these accessible
	// Copy Constructor 
	TXYHeProjection( const TXYHeProjection& );

	// Copy Assignment Operator 
	TXYHeProjection& operator=( const TXYHeProjection& );
	
	
private:

	
	TReferenceEllipsoid*		fEllipsoid; /*!< reference ellipsoid */
	TAModifiedLocalAstronomicalRF*		fMLASystem; /*!< MLA ref.frame */

};

#endif // SU_XYHe_PROJECTION
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


/// Return a pointer to a clone of this reference frame
		//virtual TVReferenceFrame*  clone() const;		
//! transforms a spatial position with an affine transformation, if possible
//		virtual bool transform( TSpatialPosition* sp, TVAffineTransformation* trans);
//! transforms a spatial vector with an affine transformation, if possible
//		virtual bool transform( TSpatialVector* sv, TVAffineTransformation* trans);
//ClassDef(TXYHeProjection, 1)






