//TGeodeticCoordSys.h 
/*!
//Class for explain vector in a geodetic coordinate system 
//
//Copyright 2002, CERN, EST/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////

#ifndef SU_GEO_COORDSYS
#define SU_GEO_COORDSYS

#if _MSC_VER >= 1000
#pragma once
#endif //! _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////

//!Includes Files
#include "TACoordinateSystem.h"
#include "TLength.h"
#include "TAngle.h"
//!Type Definition
//typedef		
//
////////////////////////////////////////////////////////////////

/*! \ingroup MathematicalConcepts
	@{*/

//! Class for explain vector in a geodetic coordinate system
class	TGeodeticCoordSys : public  TACoordinateSystem //: public TObject
{

public:

	/*!\name Member Functions for vector*/
	//@{
	
		//!get the Phi coordinate of a position vector 
		virtual TAngle getPhiEllipsoid(const TPositionVector*) const;
		
		//!get the Lambda coordinate of a position vector
		virtual TAngle getLambdaEllipsoid(const TPositionVector*) const;
		
		//!get the H coordinate of a position vector
		virtual TScalar getH(const TPositionVector*) const;
		
		//!set the Phi Coordinate of a position vector
		virtual bool setPhiEllipsoid(TPositionVector*, const TAngle&);

		//!set the Lambda Coordinate of a position vector
		virtual bool setLambdaEllipsoid(TPositionVector*, const TAngle&);
		
		//!set the H Coordinate of a position vector
		virtual bool setH(TPositionVector*, const TLength&);
	//@}


	/*!\name Functions for Matrix*/
	//@{
		using TACoordinateSystem::getElt;

		//!set a element into a 3D matrix
		virtual	bool setElt(T3DMatrix*, const int, const int, const TReal);

		//!get a element from a 3D matrix
		virtual	TReal getElt(const T3DMatrix*, const int, const int) const;
	//@}

private:

	//ClassDef(TGeodeticCoordSys, 1)
};
/*@}*/

#endif // SU_GEO_COORDSYS
