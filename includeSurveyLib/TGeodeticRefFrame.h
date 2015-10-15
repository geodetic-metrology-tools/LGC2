//  TGeodeticRefFrame.h
//
/** A Geodetic Ellisoidal Reference Frame. 
    Deals with requests for Spatial Position coordinates
    appropriate to a Geodetic reference frame.

  
    Patterns:
    A concrete STATE of a Spatial Position
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_GEODETIC_REFERENCE_FRAME
#define SU_GEODETIC_REFERENCE_FRAME


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
//using namespace std;

class  TSpatialPosition;
class  TSpatialVector;
class  TReferenceEllipsoid;
#include  "TA3DEuclideanRefFrame.h"
#include  "TPositionVector.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! A Geodetic Ellipsoidal Reference Frame
class  TGeodeticRefFrame : public TA3DEuclideanRefFrame //: public TObject  
{
public:
	
	/**@name Constructors and Destructors */
	//@{

		/// Constructor taking the name of the reference frame
		explicit TGeodeticRefFrame( const string& name );

		/// Constructor taking the name of the reference frame
		TGeodeticRefFrame( const string& name, TReferenceEllipsoid* ell);

		/// Destructor
		virtual  ~TGeodeticRefFrame();
	//@}


	/**@name Member Functions */
	//@{
		/// Return a pointer to a clone of this reference frame
		//virtual TVReferenceFrame*  clone() const;
		virtual TPositionVector getCoordinates(const TSpatialPosition* sp, TCoordSysFactory::ECoordSys) const;

		//! return the ellipsoid of the reference frame
		virtual TReferenceEllipsoid* getEllipsoid() const { return fEllipsoid; }

		//! set the coordinates of a tSpatialPosition as a TPositionVector
		virtual bool setCoordinates(TSpatialPosition* sp, const TPositionVector& pv);

		/*! \return the geodetic coordinate from a TSpatialPosition and the ellipsoid
		(sphere, GRS80....)*/
		///modif du 17/06/2003, pour permettre d avoir un systeme geodesique acceptant plusieurs ellipsoides
		TPositionVector  getGeodeticCoords( const TSpatialPosition* pv, const TReferenceEllipsoid* ellipsoid ) const;

	//@}

		// returns a single standard-case instance, DO NOT delete the result
		static TGeodeticRefFrame* instance();

private:

	/**@name Private Constructors*/
	//@{
		// We don't want these accessible
		/*!Copy Constructor */
		TGeodeticRefFrame( const TGeodeticRefFrame& );

		/*!Copy Assignment Operator */
		TGeodeticRefFrame& operator=( const TGeodeticRefFrame& );
	//@}
	
	// constants
	static const TReal precisionH;
	static const TReal precisionPhi;

	/**@name Private Functions*/
	//@{
		/*! \return the geodetic coordinate from a TPositionVector and the ellipsoid
		(sphere, GRS80....)*/
		TPositionVector  getGeodeticCoords( const TPositionVector pv, const TReferenceEllipsoid* ellipsoid ) const;

		/*! return the cartesian coordinates of a position vector in geodetic coordinate system*/
		TPositionVector  getCartesianCoords( TPositionVector posvec, const TReferenceEllipsoid* ellipsoid );
	//@}

private:

	/**@name Private Attributes*/
	//@{	
		TReferenceEllipsoid *fEllipsoid; /*!< pointer to the reference ellispoid */
	//@}

	//ClassDef(TGeodeticRefFrame, 1)
};

/*@}*/

#endif // SU_GEODETIC_REFERENCE_FRAME
