// TRotationMatrix.h
//
/** Class for the matrices of rotation used for points transformations 
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_ROTATION_MATRIX
#define SU_ROTATION_MATRIX


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
class TFreeVector;
class TPositionVector;
//using namespace std;
//
#include "TCoordSysFactory.h"
#include  "TAngle.h"
#include  "TACoordSysMatrix.h"
#include <string>
#include <iostream>

//using namespace std;

//
//typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Class for the matrices of rotation used for points transformations 
class  TRotationMatrix : public TACoordSysMatrix  
{
public:


	//constants
	enum		ERotationType {kRzyx, kRzxy};
	
	/**@name Constructors and Destructors */
	//@{
		//! Default Constructor 
		TRotationMatrix();

		
		/*! Constructor taking the radians value of the angles in the specified order
			\param RotationType enum type for the order of filling rotation matrix
			\param TReal omega radian value of the rotation angle around the X axis
			\param TReal phi radian value of the rotation angle around the Y axis
			\param TReal kappa radian value of the rotation angle around the Z axis
			\param TVCoordinateSystem* */
		TRotationMatrix(ERotationType kR, TReal, TReal, TReal);

		
		//! Copy Constructor 
		TRotationMatrix(const  TRotationMatrix&);

		//! Destructor
		virtual  ~TRotationMatrix();
	//@}


	/*!\name operator Functions */
	//@{
		//!Copy Assignment Operator 
		TRotationMatrix& operator=( const TRotationMatrix& );

		/*!Operator * for a TRotationMatrix
			\return a TRotationMatrix */
		TRotationMatrix operator*( const TRotationMatrix& ) const;

		/*!Operator * for a TFreeVector
			\return a TFreeVector */
		TFreeVector operator*(const TFreeVector& ) const;

		/*!Operator * for a TPositionVector
			\return a TPositionVector */
		TPositionVector operator*(const TPositionVector& ) const;
		//@}

		
		
		/*!\name member functions*/
		//@{
		//! sets the coordinate system
		void setCoordSys( TCoordSysFactory::ECoordSys);

		/*!change this into  the identity matrix
			\return true if is possible*/
		bool identity();

		/*! set the 3 rotations taking angles in the specified order
			\param RotationType enum for the rotation type
			\param omega radian value for the rotation angle around X axis
			\param phi radian value for the rotation angle around Y axis
			\param kappa radian value for the rotation angle around Z axis */
		bool setAllRotations(ERotationType kR, TReal&, TReal&, TReal&);

		
		
		//! Return the inverse of the matrix
		TRotationMatrix inverse();

		/*! computation of the angles from the matrix with choosen order for the rotations
			\return the 3 rotation angles as a struct */
		struct Angles getAngles(const ERotationType kR) const;

		/*!inverse and rewrite this
		\return true if possible*/
		bool	invert();
		
		/*!\return a matrix equal to the transposed of this one*/
		virtual TRotationMatrix		transposed() const;

		//!get a element of a rotation matrix
		TReal getElt(const int, const int) const;


	

private:
	
	//ClassDef(TRotationMatrix, 1)
};

//! structure for 3 rotation angles
struct Angles{
	TAngle omega;
	TAngle phi;
	TAngle kappa;
};

/*@}*/



#endif // SU_ROTATION_MATRIX
