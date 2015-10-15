//
// TSpatialPlane.cpp
// class modelising a straight spatial line
/////////////////////////////////////////////////////////////////////////////


#include "TSpatialPlane.h"


/////////////////////////////////////////////////////////////////////////////
// constructor /desctructor
/////////////////////////////////////////////////////////////////////////////
TSpatialPlane::TSpatialPlane()
{//default comstructor

	std::string name = "";
	TSpatialPointName sPname (name);
	fFirstPointName = sPname;
	fSecondPointName = sPname;

	TAngle angle;
	fAngle = angle; //status kNull

	fName = "";
}


TSpatialPlane::TSpatialPlane(const TSpatialPointName firstPointName, const TSpatialPointName secondPointName)
{//constructor for a vertical plane

	fFirstPointName = firstPointName;
	fSecondPointName = secondPointName;

	TAngle angle;
	fAngle = angle; //status kNull

	fName = firstPointName.getName() + secondPointName.getName();
}


TSpatialPlane::TSpatialPlane(const TSpatialPointName firstPointName, const TAngle angle)
{//constructor for a vertical plane

	fFirstPointName = firstPointName;
	std::string name = "";
	TSpatialPointName sPname (name);
	fSecondPointName = sPname;

	fAngle = angle;

	std::string str = firstPointName.getName()+"orientationRadians";
	TReal i = angle.getRadiansValue();
	std::ostringstream oss;
	oss << str << (double) i;
	fName = oss.str();
}


TSpatialPlane::TSpatialPlane(const TSpatialPlane& source)
{//copy constructor
	(*this) = source;
}


TSpatialPlane::~TSpatialPlane()
{//detructor
}

//////////////////////////////////////////////////////////////////////////////
//member functions
//////////////////////////////////////////////////////////////////////////////
bool TSpatialPlane::operator==(const TSpatialPlane& right) const
{
	if (fName == right.getName())
		return true;
	else
		return false;
}

void TSpatialPlane::operator=(const TSpatialPlane& right)
{
	fFirstPointName = right.fFirstPointName;
	fSecondPointName = right.fSecondPointName;
	fAngle = right.fAngle;
	fName = right.fName;
	return;
}


bool	TSpatialPlane::ifNotDoneDefinePlane(const TSpatialPointName firstPointName, const TSpatialPointName secondPointName)
{
	bool retVal = false;
	if(!(isDefine()))
	{	
		fFirstPointName = firstPointName;
		fSecondPointName = secondPointName;

		TAngle angle;
		fAngle = angle; //status kNull

		fName = firstPointName.getName() + secondPointName.getName();
		retVal = true;
	}
	return retVal;
}


bool	TSpatialPlane::ifNotDoneDefinePlane(const TSpatialPointName firstPointName, const TAngle angle)
{
	bool retVal = false;
	if(!(isDefine()))
	{
		fFirstPointName = firstPointName;
		std::string name = "";
		TSpatialPointName sPname (name);
		fSecondPointName = sPname;

		fAngle = angle;

		std::string str = firstPointName.getName()+"orientationRadians";
		TReal i = angle.getRadiansValue();
		std::ostringstream oss;
		oss << str << (double) i;
		fName = oss.str();

		retVal = true;
	}
	return retVal;
}


TSpatialPointName TSpatialPlane::getFirstPointName() const
{//returns a const iterator to the first end point of the Plane
	return fFirstPointName;
}


TSpatialPointName TSpatialPlane::getSecondPointName() const
{//returns a const iterator to the second end point of the Plane

	return fSecondPointName;
}


std::string	TSpatialPlane::getName() const
{// gets the Plane's name
	return fName;
}


TAngle	TSpatialPlane::getAngle() const
{// gets the Plane's name
	return fAngle;
}


void	TSpatialPlane::buildErrorName()
{/*!build the name (used only for TWorkingPlanes
	if the plane which is added to the list is not defined)*/
	if(!(isDefine()))
	{
		fName = "PlaneNotDefine";
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
//Private Member Function
///////////////////////////////////////////////////////////////////////////////////////////
bool	TSpatialPlane::isDefine() const
{//return true if the plane is define
	bool retVal = false;
	if(fName != "")
	{
		retVal = true;
	}
	return retVal;
}
