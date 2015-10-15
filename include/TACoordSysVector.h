// TACoordSysVector.h
//
/** Abstract class use for explain TPositionVector and TFreeVector in a Coordonate System 
//
// Patterns:
//
// 
// Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_A_COORDSYS_VECTOR
#define SU_A_COORDSYS_VECTOR


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//
class TVCoordinateSystem;
//

#include "TANumericValue.h"
#include "TLength.h"
#include "TScalar.h"
#include "TDouble.h"
#include  "TCoordSysFactory.h"
#include "Global.h"

//using namespace std;
//
//
//typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Abstract class use for explain TPositionVector and TFreeVector
class  TACoordSysVector : public  TANumericValue //: public TObject
{
public:

	friend class TACoordinateSystem;

	TACoordSysVector();
	virtual ~TACoordSysVector() { }
	
	
	/**@name Member Public Functions */
	//@{
		//!Equivalence Operator 
		bool operator==( const TACoordSysVector& ) const;

		/*!test if two object have the same coordinate system
		\return false if it s wrong*/
		bool testCoordSysCart(TCoordSysFactory::ECoordSys);
		
		//!get the X coordinate of a vector in a specific Coordinate System
		TScalar getX() const;

		//!get the Y coordinate of a vector in a specific Coordinate System
		TScalar getY() const;

		//!get the Z coordinate of a vector in a specific Coordinate System
		TScalar getZ() const;

		//!set the X coordinate of a vector in a specific Coordinate System return true if X is defined
		bool setX(const TScalar&);

		//!set the Y coordinate of a vector in a specific Coordinate System return true if Y is defined
		bool setY(const TScalar&);

		//!set the Z coordinate of a vector in a specific Coordinate System return true if Z is defined
		bool setZ(const TScalar&);

		//!return fCoordSys
		TVCoordinateSystem* getCoordSysPtr() const;

		//!return ECoordSys
		TCoordSysFactory::ECoordSys getCoordSys() const;

		//! sets the coordinate system
		void setCoordSys(const TCoordSysFactory::ECoordSys );

		//!transpose a FreeVector or a PositionVector in a T3DMatrix
		virtual T3DMatrix transposed() const;
	//@}

protected:


		/**@name Member Protected Functions */
	//@{
	
	//!get the Xi coordinate of a vector in all Coordinate System
	TReal	getX(const int& ) const;


	//!set the Xi coordinate of a vector in all Coordinate System
	void	setX(const int&, const TReal&);
			
	//@}
		


private:
	
			
	/**@name Member Attributes */
		//@{
				//int				fNbRows;
				TReal					fVector[3]; /*!< element of the 3*1 vector */
				TVCoordinateSystem*		fCoordSys; /*!< coordinate system */

		//@}

	//ClassDef(TACoordSysVector, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


#endif // SU_A_COORDSYS_VECTOR
