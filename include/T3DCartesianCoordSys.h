//T3DCartesianCoordSys.h 
/*!
//Class for explain matrix and vector in a 3D cartesian
//Coordinate system 
//
//Copyright 2002, CERN, EST/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////

#ifndef SU_T3D_CART_COORDSYS
#define SU_T3D_CART_COORDSYS

#if _MSC_VER >= 1000
#pragma once
#endif //! _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////

//!Includes Files
#include "TACoordinateSystem.h"
#include "TLength.h"

//!Type Definition
//typedef		
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Class for explain matrix and vector in a 3D cartesian coordinate system 
class	T3DCartesianCoordSys : public  TACoordinateSystem //: public TObject
{

public:
	/*!\name Functions for Vector*/
	//@{
	//!get the X coordinate of a position vector or a free vector	
	virtual TScalar getX(const TACoordSysVector*) const;

	//!get the Y coordinate of a position vector or a free vector
	virtual TScalar getY(const TACoordSysVector*) const;

	//!get the Z coordinate of a position vector or a free vector
	virtual TScalar getZ(const TACoordSysVector*) const;

	//!set the X Coordinate in meters of a position vector or a free vector
	virtual bool setX(TACoordSysVector*, const TScalar&);

	//!set the Y Coordinate in meters of a position vector or a free vector
	virtual bool setY(TACoordSysVector*, const TScalar&);

	//!set the Z Coordinate in meters of a position vector or a free vector
	virtual bool setZ(TACoordSysVector*, const TScalar&);
	//@}

	/*!\name Functions for Matrix*/
	//@{
	/*!create a TRotationMatrix default def:
	\return false*/
	virtual bool setAllRotations(TRotationMatrix*, TRotationMatrix::ERotationType kR, TReal &om, TReal &p, TReal &k);

	/*!get angles of a TRotationMatrix return a structure (omega, phi, kappa), default def
	\return false*/
	virtual	struct Angles getAngles(const TRotationMatrix*, const TRotationMatrix::ERotationType kR) const;
	
	//!change a rotation matrix into the identity matrix, default def: return false
	virtual bool identity(TRotationMatrix*);

	//!set a element into a 3D matrix
	virtual	bool setElt(T3DMatrix*, const int, const int, const TReal);

	//!get a element from a 3D matrix
	virtual	TReal getElt(const T3DMatrix*, const int, const int) const;

	//!get a element from a rotation matrix
	virtual	TReal getElt(const TRotationMatrix*, const int, const int) const;
	//@}


private:



	//ClassDef(T3DCartesianCoordSys, 1)
};

/*@}*/




#endif // SU_T3D_CART_COORDSYS
