//TGeodeticCoordSys.cpp 
//
//Class for explain vector in a geodetic Coordinate system 
//
//Copyright 2002, CERN, EST/SU. All rights reserved.
/////////////////////////////////////////////////////////

//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
#include "TGeodeticCoordSys.h"
//
// other forward declarations

////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
///Member Functions for vector
////////////////////////////////////////////////////////////////
	
	TAngle TGeodeticCoordSys::getPhiEllipsoid(const TPositionVector* vec) const
	{//get the Phi coordinate of a position vector
		TAngle retVal;
		retVal.setRadiansValue(this->TACoordinateSystem::getX(vec, 0));
		return retVal;
	}
	// Error Message: c:\Workspace\SuSource\SurveyLib\source\SpatialObjects\mathematicalconcepts\TGeodeticCoordSys.cpp(22): error C2664: 'TAngle::TAngle(const TAngle &)' : cannot convert parameter 1 from 'TReal' to 'const TAngle &'
	/* Method Called:
	TLength TACoordinateSystem::getX(const TACoordSysVector*) const
	{//get the X coordinate of a position vector or a free vector, default def: \return TLength Null
	TLength null;
	null.setStatus(TVNumericValue::kNull);
	return null;
	}
	*/

	TAngle TGeodeticCoordSys::getLambdaEllipsoid(const TPositionVector* vec) const
	{//get the Lambda coordinate of a position vector 
		TAngle retVal;
		retVal.setRadiansValue(this->TACoordinateSystem::getX(vec, 1));
		return retVal;
	}

	TScalar TGeodeticCoordSys::getH(const TPositionVector* vec) const
	{//get the H coordinate of a position vector
		TScalar retVal;
		retVal.setValue(this->TACoordinateSystem::getX(vec, 2));
		return retVal;
	}


	bool TGeodeticCoordSys::setPhiEllipsoid(TPositionVector* vec, const TAngle& ang)
	{//set the phi  Coordinate in radian of a position vector 
	this->TACoordinateSystem::setX(vec,0,ang.getRadiansValue());
	return true; 
	}


	bool TGeodeticCoordSys::setLambdaEllipsoid(TPositionVector* vec, const TAngle& ang)
	{//set the lambda  Coordinate in radian of a position vector 
	this->TACoordinateSystem::setX(vec,1,ang.getRadiansValue());
	return true; 
	}


	bool TGeodeticCoordSys::setH(TPositionVector* vec, const TLength& len)
	{//set the H  Coordinate in meters of a position vector 
	this->TACoordinateSystem::setX(vec,2,len.getMetresValue());
	return true; 
	}


////////////////////////////////////////////////////////////////
///Member Functions for matrix
////////////////////////////////////////////////////////////////



bool TGeodeticCoordSys::setElt(T3DMatrix* mx, const int i, const int j, const TReal value)
{//set a element into a 3D matrix
setC(mx, i, j, value);
return true;
}


TReal TGeodeticCoordSys::getElt(const T3DMatrix* mx, const int i, const int j) const
{//get a element from a 3D matrix
return getC(mx, i, j);
}





//ClassImp(TGeodeticCoordSys)
