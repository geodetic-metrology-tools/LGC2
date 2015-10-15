#include "T3DLocalRefFrame.h"

//////////////////////////////////////
// constructor
///////////////////////////////////////
T3DLocalRefFrame::T3DLocalRefFrame(const string& name) 
: TA3DEuclideanRefFrame( name ) 
{
}

/* NOT IMPLEMENTED
//////////////////////////////////////
// Copy constructor
//////////////////////////////////////
T3DLocalRefFrame::T3DLocalRefFrame( const T3DLocalRefFrame& )
{
}
*/

///////////////////////////////////////
// destructor
////////////////////////////////////////
T3DLocalRefFrame::~T3DLocalRefFrame()
{
}

/* NOT IMPLEMENTED
////////////////////////////////////////
// Copy Assignment Operator 
////////////////////////////////////////
T3DLocalRefFrame& T3DLocalRefFrame::operator=( const T3DLocalRefFrame& )
{
	if (this != &right)
	{
	}
	return *this;
}
*/
//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

TPositionVector T3DLocalRefFrame::getCoordinates(const TSpatialPosition* sp, TCoordSysFactory::ECoordSys coordsys) const
{
	// Gets only the right position vector if the coord. system is 3D-Cartesian
	if(coordsys == TCoordSysFactory::k3DCartesian) {
		return getPositionVector(sp);
	}
	else 
	{
		TPositionVector pv(TCoordSysFactory::k3DCartesian);
		return pv;
	}
}



bool T3DLocalRefFrame::setCoordinates(TSpatialPosition* sp, const TPositionVector& pv)
{
	// Returns true only if the position vector is expressed in the 3D-Cartesian
	if(pv.getCoordSys() == TCoordSysFactory::k3DCartesian)
	{
		return setPositionVector(sp,pv);
	}
	else
	{
		return false;
	}
}
