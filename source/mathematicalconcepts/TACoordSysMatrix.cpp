// TACoordSysMatrix.cpp
//
/** Abstract class use for explain T3DMatrix and TRotationMatrix in a Coordinate System */
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
#include  "TACoordSysMatrix.h"
////////////////////////////////////////////////////////////////


//ClassImp(TACoordSysMatrix)



//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
	
bool TACoordSysMatrix::testCoordSysCart(TCoordSysFactory::ECoordSys en) const
{//test if two object have the same cartesian coordinate system
if (getCoordSys()== en && (en==TCoordSysFactory::k2DCartesian || en==TCoordSysFactory::k3DCartesian))
	{return true;}
else
	{return false;}
}

bool TACoordSysMatrix::testCoordSysGen(TCoordSysFactory::ECoordSys en) const
{//test if two object have the same general coordinate system
if (getCoordSys()== en)
	{return true;}
else
	{return false;}
}


TVCoordinateSystem* TACoordSysMatrix::getCoordSysPtr() const
{//get the *TVCoordinateSystem
	return fCoordSys;
}

TCoordSysFactory::ECoordSys TACoordSysMatrix::getCoordSys() const
{//get the ECoordSys
	return fCoordSys->getCoordSysId();
}


void TACoordSysMatrix::setCoordSys(TCoordSysFactory::ECoordSys en)
{//sets the coordinate system
fCoordSys=TCoordSysFactory::getCoordSysFactoryPtr()->getCoordSys(en);
return;
}


void TACoordSysMatrix::setCoordSysTRotation(TRotationMatrix* mx, TCoordSysFactory::ECoordSys en)
{//set the coordinate system for a rotation matrix
	if (en==TCoordSysFactory::k3DCartesian)
	{fCoordSys=TCoordSysFactory::getCoordSysFactoryPtr()->getCoordSys(en);}
	else
	{
		for (int i=0; i<3; i++)
		{
			for (int j=0; j<3; j++)
			{
			mx->setC(i,j,0);
			}
		}
	mx->setStatus(TVNumericValue::kNull);
	}
}	


//////////////////////////////////////////////////////////////////////
// Member Protected Functions
//////////////////////////////////////////////////////////////////////

TReal TACoordSysMatrix::getC(const int& i, const int& j) const
{//get the (i,j) value
	return mx[i][j];
}


void	TACoordSysMatrix::setC(const int& i, const int& j, TReal value)
{//!set value to (i,j)
	mx[i][j]=value;
}













