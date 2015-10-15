// TACoordinateSystem.h
//
/** Abstract class for Coordonate System 
//
// Patterns:
//
// 
// Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_A_COORDINATE_SYSTEM
#define SU_A_COORDINATE_SYSTEM


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include "TVCoordinateSystem.h"
//using namespace std;
//
//
//typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup MathematicalConcepts 
	@{*/

//! Abstract class for Coordonate System
class  TACoordinateSystem : public  TVCoordinateSystem //: public TObject
{
public:
	/*!\name Functions for Vector*/
	//@{
		/*!get the H coordinate of a position vector, default def:
		\return TLength Null*/
		virtual TScalar getH(const TPositionVector*) const;

		/*!get the Lambda coordinate of a position vector, default def: 
		\return TLength Null*/
		virtual TAngle getLambdaEllipsoid(const TPositionVector*) const;

		/*!get the Phi coordinate of a position vector, default def: 
		\return TAngle Null*/
		virtual TAngle getPhiEllipsoid(const TPositionVector*) const;

		/*!get the X coordinate of a position vector or a free vector, default def: 
		\return TLength Null*/
		virtual TScalar getX(const TACoordSysVector*) const;

		/*!get the Y coordinate of a position vector or a free vector, default def: 
		\return TLength Null*/
		virtual TScalar getY(const TACoordSysVector*) const;

		/*!get the Z coordinate of a position vector or a free vector, default def: 
		\return TLength Null*/
		virtual TScalar getZ(const TACoordSysVector*) const;

		/*!set the H Coordinate of a position vector, default def:
		\return false*/
		virtual bool setH(TPositionVector*, const TScalar&);

		/*!set the Lambda Coordinate of a position vector, default def:
		\return false*/
		virtual bool setLambdaEllipsoid(TPositionVector*, const TAngle&);

		/*!set the Phi Coordinate of a position vector, default def:
		\return false*/
		virtual bool setPhiEllipsoid(TPositionVector*, const TAngle&);

		/*!set the X Coordinate of a position vector or a free vector, default def:
		\return false*/
		virtual bool setX(TACoordSysVector*, const TScalar&);

		/*!set the Y Coordinate of a position vector or a free vector, default def:
		\return false*/
		virtual bool setY(TACoordSysVector*, const TScalar&);

		/*!set the Z Coordinate of a position vector or a free vector, default def:
		\return false*/
		virtual bool setZ(TACoordSysVector*, const TScalar&);
	//@}

	/*!\name Functions for Matrix*/
	//@{
		using TVCoordinateSystem::getElt;

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

	/*!\name Member Functions*/
	//@{
		//! set the coordinate system identifier
		virtual void setCoordSysId(TCoordSysFactory::ECoordSys);

		//! get the coordinate system identifier
		virtual TCoordSysFactory::ECoordSys getCoordSysId() const;
	//@}

protected:

	/*!\name Member Protected Functions for Vector*/
	//@{
		//!get the Xi coordinate of a vector in all Coordinate System
		TReal	getX(const TACoordSysVector*, const int& ) const;


		//!set the Xi coordinate of a vector in all Coordinate System
		void	setX(TACoordSysVector*, const int&, const TReal&);
	//@}
		
	/*!\name Member Protected Functions for Matrix*/
	//@{
		//!get the (i,j) value of a matrix
		TReal getC(const TACoordSysMatrix*, const int& i, const int& j) const;

		//!set value to (i,j) of a matrix
		void setC(TACoordSysMatrix*, const int& i, const int& j, TReal value);
	//@}

private:
	
	TCoordSysFactory::ECoordSys			fCoordSysId; /*!< coordinate system identifier */
		
	

	//ClassDef(TACoordinateSystem, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


#endif // SU_A_COORDINATE_SYSTEM
