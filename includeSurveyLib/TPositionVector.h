// TPositionVector.h
//
/** 3D Vector used to define position in a space
//
// Patterns:
//
// 
// Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_POS_VECTOR
#define SU_POS_VECTOR


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <iostream>
//using namespace std;
//
#include "TACoordSysVector.h"
#include "TFreeVector.h"
#include "TDouble.h"
#include "TScalar.h"
#include "TLength.h"
//
//#include  "**classname**.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup MathematicalConcepts
	@{*/

//! 3D Vector used as a position vector
class  TPositionVector : public TACoordSysVector//: public TObject  

{
public:

	/*!\name Constructors and Destructors */
	//@{
		//!Default Constructor 
		TPositionVector(TCoordSysFactory::ECoordSys);
	
		//!Constructor taking 3 TLength
		//TPositionVector(const TLength&, const TLength&, const TLength&);

		//!Constructor taking 3 TReal
		TPositionVector(const TReal&, const TReal&, const TReal&,TCoordSysFactory::ECoordSys);

		//!Copy Constructor 
		TPositionVector( const TPositionVector&);

		//!Destructor
		virtual  ~TPositionVector();
	//@}

		
	/*!\name operator Functions */
	//@{
		//!Equivalence Operator 
		bool operator==( const TPositionVector& ) const;
		
		//!add a PositionVector  and FreeVector, return a PositionVector
		TPositionVector operator+(const TFreeVector& );
		TPositionVector operator-(const TFreeVector& );
		
		//!add a PositionVector  and FreeVector, replace this
		TPositionVector& operator+=(const TFreeVector& );
		//!substract a FreeVector from aPositionVector and replace this
		TPositionVector& operator-=(const TFreeVector& );

		//!substract two TPositionVector objects and return a FreeVector
		TFreeVector operator-(const TPositionVector& );
		
		//!Multiply a TPositionVector object by a TDouble
		TPositionVector operator*(const TDouble&);

		//!Multiply a TPositionVector object by a TSCalar
		TPositionVector operator*(const TScalar&);

		//!Multiply a TPositionVector object by a TReal 
		TPositionVector operator*(const TReal&);

		//!Multiply this vector by a TScalar
		TPositionVector & operator *= ( const TScalar& );

		//!Multiply this vector by a TReal 
		TPositionVector & operator *= ( const TReal& );

		//!Copy Assignment Operator 
		TPositionVector& operator=(const TPositionVector& );

	//@}

	/*!\name member Functions */
	//@{
		//!Give the distance between this and a second position vector
		TScalar dist(const TPositionVector& ) const;

		//!get the H value of a TPositionVector if is defined
		TScalar getH() const;

		//!get the lambda value of a TPositionVector if is defined
		TAngle getLambdaEllipsoid() const;

		//!get the phi value of a TPositionVector if is defined
		TAngle getPhiEllipsoid() const;

		//!set the H coordinate of a vector in a specific Coordinate System return true if X is defined
		bool setH(const TScalar&);

		//!set the Lambda coordinate of a vector in a geodetic Coordinate System return true if X is defined
		bool setLambdaEllipsoid(const TAngle&);

		//!set the Phi coordinate of a vector in a geodetic Coordinate System return true if X is defined
		bool setPhiEllipsoid(const TAngle&);
	//@}
	
	
private:

		//!Default Constructor 
		TPositionVector();
						
	//ClassDef(TPositionVector, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions 
//////////////////////////////////////////////////////////////////////



#endif // SU_POS_VECTOR
