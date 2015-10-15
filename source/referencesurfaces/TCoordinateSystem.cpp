// TCoordinateSystem 
//
// Abstract Base Class for a Coordinate System.
//
// Used by a TReferenceFrame to identify valid coordinate systems
// for a given reference frame, and thereby valid coordinates for a 
// spatial position
// 
// Patterns:
//
// Copyright 1999,2000, Mark Jones, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#include  "TCoordinateSystem.h"

//////////////////////////////////////////////////////////////////////


//ClassImp(TCoordinateSystem)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TCoordinateSystem::TCoordinateSystem(ECoordinateSystem nameToken) : fNameToken(nameToken)
{
}

TCoordinateSystem::TCoordinateSystem(const TCoordinateSystem& tcs)
{
	fDimension=tcs.fDimension;
	fNameToken=tcs.fNameToken;
	fCoordinateTokens[3]=tcs.fCoordinateTokens[3];
}

TCoordinateSystem::~TCoordinateSystem()
{
}


//////////////////////////////////////////////////////////////////////
// Operator = 
//////////////////////////////////////////////////////////////////////
TCoordinateSystem& TCoordinateSystem::operator=(const TCoordinateSystem& tcs)
{
	if (this != &tcs)
	{
		fDimension=tcs.fDimension;
		fNameToken=tcs.fNameToken;
		fCoordinateTokens[3]=tcs.fCoordinateTokens[3];
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

bool TCoordinateSystem::validCoord(TSpatialPosition::ECoordToken a)
{//test if a coordinate type is valid
	if (a != fCoordinateTokens[0] && a != fCoordinateTokens[1] && a != fCoordinateTokens[2]) 
		{
			return false;
		}
	else
		return true;
}


void  TCoordinateSystem::setCoordTokens(TSpatialPosition::ECoordToken a)
{//1D CS type
	fCoordinateTokens[0] = a;
	fCoordinateTokens[1] = TSpatialPosition::kNull;
	fCoordinateTokens[2] = TSpatialPosition::kNull;
	fDimension = 1;
	return;
}

void  TCoordinateSystem::setCoordTokens(TSpatialPosition::ECoordToken a, TSpatialPosition::ECoordToken b)
{//2D CS type
	fCoordinateTokens[0] = a;
	fCoordinateTokens[1] = b;
	fCoordinateTokens[2] = TSpatialPosition::kNull;
	fDimension = 2;
	return;
}

void  TCoordinateSystem::setCoordTokens(TSpatialPosition::ECoordToken a, TSpatialPosition::ECoordToken b, TSpatialPosition::ECoordToken c)
{//3D CS type
	fCoordinateTokens[0] = a;
	fCoordinateTokens[1] = b;
	fCoordinateTokens[2] = c;
	fDimension = 3;
	return;
}


//////////////////////////////////////////////////////////////////////
// End 
//////////////////////////////////////////////////////////////////////

