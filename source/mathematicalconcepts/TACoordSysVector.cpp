// TACoordSysVector.cpp
//
/** Abstract class use for explain TPositionVector and TFreeVector in a Coordonate System */
//
// Patterns:
//
// 
// Copyright 2002 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "TVCoordinateSystem.h"
#include  "TACoordSysVector.h"
////////////////////////////////////////////////////////////////


//ClassImp(TACoordSysVector)


TACoordSysVector::TACoordSysVector(): TANumericValue()
{
	int i = 0;
	while(i<3)
	{
		fVector[i] = LITERAL(0.0);
		i++;
	}
	fCoordSys = 0;
}


//Equivalence Operator 
bool TACoordSysVector::operator==( const TACoordSysVector& right) const
{
	bool equal = false;

	if(fVector[0] == right.fVector[0] 
		&& fVector[1] == right.fVector[1] 
		&& fVector[2] == right.fVector[2]
		&& fCoordSys == right.fCoordSys
			&& getStatus() == right.getStatus()
		) 
	{
		equal = true;
	}

	return equal;
}


//////////////////////////////////////////////////////////////////////
// Member Public Functions
//////////////////////////////////////////////////////////////////////

	
bool TACoordSysVector::testCoordSysCart(TCoordSysFactory::ECoordSys en)
{//test if two object have the same coordinate system
	if (getCoordSys()== en  && (en==TCoordSysFactory::k3DCartesian || en==TCoordSysFactory::k2DCartesian))
	{return true;}
else
	{return false;}
}


TScalar TACoordSysVector::getX()const
{//!get the X coordinate of a vector in a specific Coordinate System
	return fCoordSys->getX(this);
}

TScalar TACoordSysVector::getY()const
{//!get the Y coordinate of a vector in a specific Coordinate System
	return fCoordSys->getY(this);
}

TScalar TACoordSysVector::getZ()const
{//!get the Z coordinate of a vector in a specific Coordinate System
	return fCoordSys->getZ(this);
}

bool TACoordSysVector::setX(const TScalar& xc)
{//!set the X coordinate of a vector in a specific Coordinate System return true if X is defined
return fCoordSys->setX(this, xc);
}


bool TACoordSysVector::setY(const TScalar& yc)
{//!set the Y coordinate of a vector in a specific Coordinate System return true if Y is defined
return fCoordSys->setY(this, yc); 
}

bool TACoordSysVector::setZ(const TScalar& zc)
{//!set the Z coordinate of a vector in a specific Coordinate System return true if Z is defined
return fCoordSys->setZ(this, zc);
}

TVCoordinateSystem* TACoordSysVector::getCoordSysPtr() const
{//!get fCoordSys
	return fCoordSys;
}

TCoordSysFactory::ECoordSys TACoordSysVector::getCoordSys() const
{//!get ECoordSys
return fCoordSys->getCoordSysId();
}

void TACoordSysVector::setCoordSys(const TCoordSysFactory::ECoordSys en )
{//! sets the coordinate system
	fCoordSys=TCoordSysFactory::getCoordSysFactoryPtr()->getCoordSys(en);
return;
}

T3DMatrix TACoordSysVector::transposed() const
{//!return the tranposed column vector as a 3D matrix
	T3DMatrix result = T3DMatrix(this->getCoordSys());
	for (int i=0; i<3; i++)
	{
		result.setElt(0, i,this->getX(i));
	}
	return result;
}

//////////////////////////////////////////////////////////////////////
// Member Protected Functions
//////////////////////////////////////////////////////////////////////

TReal TACoordSysVector::getX(const int& i) const
{//!get the Xi coordinate of a vector in all Coordinate System
	return fVector[i];
}


void TACoordSysVector::setX(const int& i, const TReal& value)
{//!set the Xi coordinate of a vector in all Coordinate System
	fVector[i]=value;
	return;
}





