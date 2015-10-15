// TCoordinateSystem.h: 
//
// Coordinate System.
//
// Used by a TReferenceFrame to identify valid coordinate systems
// for a given reference frame, and thereby valid coordinates for a 
// spatial position
// 
// Patterns:
//
// Copyright 1999,2000, CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////

#ifndef SU_COORDINATE_SYSTEM
#define SU_COORDINATE_SYSTEM

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
///////////////////////////////////////////////////////////////
#include  <string>
using namespace std;
//
#include  "TSpatialPosition.h"
//
//typedef

////////////////////////////////////////////////////////////////



//Class definition
class TCoordinateSystem //: public TObject  
{
public:
	//constants
	enum  ECoordinateSystem {kNull, k3DCartesian, kGeodetic, k2DCartesian} ;
	

	// constructors and destructors
	TCoordinateSystem(ECoordinateSystem);
	TCoordinateSystem(const TCoordinateSystem&);
	virtual ~TCoordinateSystem();
	TCoordinateSystem& operator=(const TCoordinateSystem&);

	//member functions
	bool  operator==( TCoordinateSystem & ) const;//test of equality by name
	bool  operator<( TCoordinateSystem & ) const;//test of order by name
	ECoordinateSystem  getName()const;//return CS name
	int  getDimension() const;//return dimension of the CS

	bool  validCoord(TSpatialPosition::ECoordToken);//verify the wanted coordinate is valid

	void  setCoordTokens(TSpatialPosition::ECoordToken);//set 1d CS type
	void  setCoordTokens(TSpatialPosition::ECoordToken,TSpatialPosition::ECoordToken);//set 2d CS type
	void  setCoordTokens(TSpatialPosition::ECoordToken,TSpatialPosition::ECoordToken,TSpatialPosition::ECoordToken);//set 3d CS type

	TSpatialPosition::ECoordToken getFirstCoordToken() const;//return first coord type
	TSpatialPosition::ECoordToken getSecondCoordToken() const;//return second coord type
	TSpatialPosition::ECoordToken getThirdCoordToken() const;//return third coord type
	

protected:
//	static string  fName;

private:
	
	// member attributes
	int  fDimension;//dimension of the system
	ECoordinateSystem  fNameToken;//name of CS
	TSpatialPosition::ECoordToken  fCoordinateTokens[3];// type of CS

	//ClassDef(TCoordinateSystem, 1)
};




//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


inline  bool  TCoordinateSystem::operator==( TCoordinateSystem &coorSys ) const
{	return ( this->getName() == coorSys.getName() );
}


inline  bool  TCoordinateSystem::operator<( TCoordinateSystem &coorSys ) const
{	return ( this->getName() < coorSys.getName() );
}



inline  TCoordinateSystem::ECoordinateSystem  TCoordinateSystem::getName() const
{	return fNameToken;
}

inline  int  TCoordinateSystem::getDimension() const
{
	return fDimension;
}

inline 	TSpatialPosition::ECoordToken TCoordinateSystem::getFirstCoordToken() const
{
	return fCoordinateTokens[0];
}


inline 	TSpatialPosition::ECoordToken TCoordinateSystem::getSecondCoordToken() const
{
	return fCoordinateTokens[1];
}


inline 	TSpatialPosition::ECoordToken TCoordinateSystem::getThirdCoordToken() const
{
	return fCoordinateTokens[2];
}
#endif // SU_COORDINATE_SYSTEM



