//T2DCartesianPlusHSys.h 
//
//Class for explain vector in a 2D+H cartesian
//Coordinate system 
//
//Copyright 2002, CERN, EST/SU. All rights reserved.
/////////////////////////////////////////////////////////

//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
#include "T2DCartesianPlusHSys.h"
//
// other forward declarations

////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
///Members Functions for vector
//////////////////////////////////////////////////////////////////


TScalar T2DCartesianPlusHSys::getX(const TACoordSysVector* vec) const
{//get the X coordinate of a position vector or a free vector
	TScalar retVal;
	retVal.setValue(this->TACoordinateSystem::getX(vec, 0));
	return retVal;
}

TScalar T2DCartesianPlusHSys::getY(const TACoordSysVector* vec) const
{//get the Y coordinate of a position vector or a free vector
	TScalar retVal;
	retVal.setValue(this->TACoordinateSystem::getX(vec, 1));
	return retVal;
}


TScalar T2DCartesianPlusHSys::getH(const TPositionVector* vec) const
{//get the H coordinate of a position vector
	TScalar retVal;
	retVal.setValue(this->TACoordinateSystem::getX(vec, 2));
	return retVal;
}

bool T2DCartesianPlusHSys::setX(TACoordSysVector* vec, const TScalar& len)
{//set the X in meters Coordinate of a position vector or a free vector 
	this->TACoordinateSystem::setX(vec,0,len.getValue());
return true; 
}

bool  T2DCartesianPlusHSys::setY(TACoordSysVector* vec, const TScalar& len)
{//set the Y Coordinate of a position vector or a free vector, default def: return false
this->TACoordinateSystem::setX(vec,1,len.getValue());
return true;
}

bool T2DCartesianPlusHSys::setH(TPositionVector* vec, const TScalar& len)
{//set the H  Coordinate in meters of a position vector 
this->TACoordinateSystem::setX(vec,2,len.getValue());
return true; 
}


////////////////////////////////////////////////////////////////
///Member Functions for matrix
////////////////////////////////////////////////////////////////



bool T2DCartesianPlusHSys::setElt(T3DMatrix* mx, const int i, const int j, const TReal value)
{//set a element into a 3D matrix
setC(mx, i, j, value);
return true;
}


TReal T2DCartesianPlusHSys::getElt(const T3DMatrix* mx, const int i, const int j) const
{//get a element from a 3D matrix
return getC(mx, i, j);
}





//ClassImp(T2DCartesianPlusHSys)
