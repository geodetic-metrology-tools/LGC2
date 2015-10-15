//T2DCartesianPlusHSys.h 

/*!
	Class for explain vector in a 2D+H cartesian
	Coordinate system 

	Copyright 2002, CERN, EST/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////

#ifndef SU_T2D_PLUSH_COORDSYS
#define SU_T2D_PLUSH_COORDSYS

#if _MSC_VER >= 1000
#pragma once
#endif //! _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////

//!Includes Files
#include "TACoordinateSystem.h"
#include "TLength.h"
#include "TPositionVector.h"

//!Type Definition
//typedef		
//
////////////////////////////////////////////////////////////////

/*! \ingroup MathematicalConcepts
	@{*/

//! Class for explain vector in a 2D+H cartesian coordinate system
class	T2DCartesianPlusHSys : public  TACoordinateSystem //: public TObject
{

public:
	/*!\name Functions for Vector*/
	//@{
		//!get the X coordinate of a position vector or a free vector	
		virtual TScalar getX(const TACoordSysVector*) const;

		//!get the Y coordinate of a position vector or a free vector
		virtual TScalar getY(const TACoordSysVector*) const;

		//!get the H coordinate of a position vector
		virtual TScalar getH(const TPositionVector*) const;

		//!set the X Coordinate in meters of a position vector or a free vector
		virtual bool setX(TACoordSysVector*, const TScalar&);

		//!set the Y Coordinate in meters of a position vector or a free vector
		virtual bool setY(TACoordSysVector*, const TScalar&);
		
		//!set the H Coordinate of a position vector, default def:
		virtual bool setH(TPositionVector*, const TScalar&);
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



	//ClassDef(T2DCartesianPlusHSys, 1)
};

/*@}*/



#endif // SU_T2D_PLUSH_COORDSYS
