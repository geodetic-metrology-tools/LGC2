// TACoordSysMatrix.h
//
/** Abstract class use for explain T3DMatrix and TRotationMatrixin a Coordonate System 
//
// Patterns:
//
// 
// Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_A_COORDSYS_MATRIX
#define SU_A_COORDSYS_MATRIX


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
class TVCoordinateSystem;
#include "TANumericValue.h"
#include  "TCoordSysFactory.h"
class TRotationMatrix;
//using namespace std;
//
//
//typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup MathematicalConcepts
	@{*/

//! Abstract class use for explain T3DMatrix and TRotationMatrix
class  TACoordSysMatrix : public  TANumericValue //: public TObject
{

public:

	friend class TACoordinateSystem;

	/*!\name member public functions*/
	//@{
	/*!test if two object have the same Cartesian coordinate system
	\return false if it s wrong*/
	bool testCoordSysCart(TCoordSysFactory::ECoordSys) const;

	/*!test if two object have the same general coordinate system
	\return false if it s wrong*/
	bool testCoordSysGen(TCoordSysFactory::ECoordSys) const;
	
	//!get the *TVCoordinateSystem
	TVCoordinateSystem* getCoordSysPtr() const;

	//!get the ECoordSys
	TCoordSysFactory::ECoordSys getCoordSys() const;

	//! sets the coordinate system
	void setCoordSys( TCoordSysFactory::ECoordSys);
	
	//!set the coordinate system for a rotation matrix
	void setCoordSysTRotation(TRotationMatrix*, TCoordSysFactory::ECoordSys);
	//@}
	
	//@{
	//!get the (i,j) value
	TReal getC(const int& i, const int& j) const;

	//!set value to (i,j)
	void setC(const int& i, const int& j, TReal value);
	//@}


private:
	
	TReal					mx[3][3]; /*!< élément of the 3*3 matrix */
	TVCoordinateSystem*		fCoordSys; /*!< coordinate system */
		
	

	//ClassDef(TACoordSysMatrix, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


#endif // SU_A_COORDSYS_MATRIX
