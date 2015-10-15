//  TXYHgProjection.h
//
/** A projection on XY plane with height above the Geoid

  
    Patterns:
    A concrete STATE of a Spatial Position
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_XYHg_PROJECTION
#define SU_XYHg_PROJECTION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
//
class		TSpatialPosition;
class		TSpatialVector;
class		TAGeoidModel;
class		TXYHeProjection;
#include	"TA2DPlus1MapProjection.h"
using namespace std;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects 
	@{*/

//!A projection on XY plane with height above the Geoid
class  TXYHgProjection : public TA2DPlus1MapProjection //: public TObject  
{
public:
	
	/**@name Constructors and Destructors */
	//@{
		/*! Default constructor */
		TXYHgProjection();

		/*! Constructor taking the name, the geoid and the
		 ellipsoid projection */
		TXYHgProjection( const string& name, TAGeoidModel* geoid,
			TXYHeProjection* xyhe); //TAModifiedLocalAstronomicalRF* ccs); modified

		/*! Destructor */
		virtual  ~TXYHgProjection();
	//@}


	/**@name Member Functions */
	//@{
		/*! return the ellipsoid projection reference frame */
		virtual TXYHeProjection* getXYHe() { return fXYHe; }
		/*! return the geoid of the projection */
		virtual TAGeoidModel* getGeoid() { return fGeoid; }
	//@}


protected:


private:
	// We don't want these accessible
	// Copy Constructor 
	TXYHgProjection( const TXYHgProjection& );

	// Copy Assignment Operator 
	TXYHgProjection& operator=( const TXYHgProjection& );
	
	
private:

	
	TAGeoidModel*		fGeoid; /*!< geoid model */

	TXYHeProjection*	fXYHe; /*!< ellipsoid projection */

};

#endif // SU_XYHg_PROJECTION
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////
	//ClassDef(TXYHgProjection, 1)


		//virtual TLength getN(const TSpatialPosition&) const ;
		/// Return a pointer to a clone of this reference frame
		//virtual TVReferenceFrame*  clone() const;
		//! transforms a spatial position with an affine transformation, if possible
//		virtual bool transform( TSpatialPosition* sp, TVAffineTransformation* trans);

		//! transforms a spatial vector with an affine transformation, if possible
//		virtual bool transform( TSpatialVector* sv, TVAffineTransformation* trans);


	
		//! return the modidfied local astronomical reference frame
		//virtual TAModifiedLocalAstronomicalRF* getMLARefFrame() { return fMLASystem; }
		







