#include "TA2DPlus1MapProjection.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
///////////////////////////////////////////////////////////////////////////////////////////////////
TA2DPlus1MapProjection::TA2DPlus1MapProjection(const string& name)
	: TAReferenceFrame( name )
{// Default constructor
}


TA2DPlus1MapProjection::~TA2DPlus1MapProjection()
{// Destructor
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//DEFAULT DEFINITION OF TVReferenceFrame's DATA EXTRACTION METHODS
///////////////////////////////////////////////////////////////////////////////////////////////////
TPositionVector TA2DPlus1MapProjection::getCoordinates(const TSpatialPosition* sp, TCoordSysFactory::ECoordSys coordsys) const
{//Default code for accessing the position vector of a spatial position 
//see definition of TSpatialPosition
	if(coordsys == TCoordSysFactory::k2DPlusH)
	{
		return getPositionVector(sp);
	}
	else 
	{ 
		TPositionVector pv(TCoordSysFactory::k2DPlusH);
		return pv;
	}
}


TFreeVector TA2DPlus1MapProjection::getElements(const TSpatialVector* sv, TCoordSysFactory::ECoordSys coordsys) const
{//Default code for accessing the free vector of a spatial vector 
//see definition of TSpatialVector
	TFreeVector fv(TCoordSysFactory::k2DPlusH);
	if(coordsys == TCoordSysFactory::k2DPlusH) {
		fv = getVector(sv);
	}
	return fv;
}


TRotationMatrix TA2DPlus1MapProjection::getElements(const TSpatialOrientation* so, TCoordSysFactory::ECoordSys coordsys) const
{//Default code for accessing the rotation matrix of a spatial orientation 
//see definition of TSpatialOrientation
	TRotationMatrix rm;//(TCoordSysFactory::k2DPlusH);
	if(coordsys == TCoordSysFactory::k2DPlusH) {
		rm = getOrientation(so);
	}
	return rm;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//DEFAULT DEFINITION OF TVReferenceFrames's SETTINGS METHODS
///////////////////////////////////////////////////////////////////////////////////////////////////
bool TA2DPlus1MapProjection::setCoordinates(TSpatialPosition* sp, const TPositionVector& pv)
{//Default code for setting the position vector of a spatial position 
//see definition of TSpatialPosition
	bool result;
	result = setPositionVector(sp, pv);
	return result;
}


bool TA2DPlus1MapProjection::setElements(TSpatialVector* sv, const TFreeVector& fv)
{//Default code for setting the free vector of a spatial vector 
//see definition of TSpatialVector
	bool result;
	result = setVector(sv, fv);
	return result;
}



bool TA2DPlus1MapProjection::setElements(TSpatialOrientation* so, const TRotationMatrix& rm)
{//Default code for setting the rotation matrix of a spatial orientation 
//see definition of TSpatialOrientation
	bool result;
	result = setOrientation(so, rm);
	return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//END
///////////////////////////////////////////////////////////////////////////////////////////////////

