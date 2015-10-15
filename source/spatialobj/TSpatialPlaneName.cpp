// TSpatialPlaneName.cpp
// class modelising a straight spatial line
/////////////////////////////////////////////////////////////////////////////


#include "TSpatialPlaneName.h"


/////////////////////////////////////////////////////////////////////////////
// constructor /desctructor
/////////////////////////////////////////////////////////////////////////////
TSpatialPlaneName::TSpatialPlaneName() : fName(""), fFirstPointName(), fSecondPointName(), fAngle()
{//default comstructor
}


TSpatialPlaneName::TSpatialPlaneName(const TSpatialPointName firstPointName, const TSpatialPointName secondPointName)
{//constructor for a vertical plane

	fFirstPointName = firstPointName;
	fSecondPointName = secondPointName;

	TAngle angle;
	fAngle = angle; //status kNull

	fName = firstPointName.getName() + secondPointName.getName();
}


TSpatialPlaneName::TSpatialPlaneName(const TSpatialPointName firstPointName, const TAngle angle)
{//constructor for a vertical plane

	fFirstPointName = firstPointName;
	std::string name = "";
	TSpatialPointName sPname (name);
	fSecondPointName = sPname;

	fAngle = angle;

	std::string str = firstPointName.getName()+"orientationGons";
	TReal i = angle.getGonsValue();
	std::ostringstream oss;
	oss << str << (double) i;
	fName = oss.str();
}


TSpatialPlaneName::TSpatialPlaneName(const TSpatialPlaneName& source)
{//copy constructor
	(*this) = source;
}


TSpatialPlaneName::~TSpatialPlaneName()
{//detructor
}

//////////////////////////////////////////////////////////////////////////////
//member functions
//////////////////////////////////////////////////////////////////////////////
bool TSpatialPlaneName::operator==(const TSpatialPlaneName& right) const
{
	if (fName == right.getName())
		return true;
	else
		return false;
}

void TSpatialPlaneName::operator=(const TSpatialPlaneName& right)
{
	fFirstPointName = right.fFirstPointName;
	fSecondPointName = right.fSecondPointName;
	fAngle = right.fAngle;
	fName = right.fName;
	return;
}


		
bool TSpatialPlaneName::operator<( const TSpatialPlaneName& right) const
{//!Less than operator
	bool answer = false;

	if (fName<right.fName)
	{
		answer=true;
	}

	return answer;
}

	
bool	TSpatialPlaneName::ifNotDoneDefinePlane(const TSpatialPointName firstPointName, const TSpatialPointName secondPointName)
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


bool	TSpatialPlaneName::ifNotDoneDefinePlane(const TSpatialPointName firstPointName, const TAngle angle)
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


TSpatialPointName TSpatialPlaneName::getFirstPointName() const
{//returns a const iterator to the first end point of the Plane
	return fFirstPointName;
}


TSpatialPointName TSpatialPlaneName::getSecondPointName() const
{//returns a const iterator to the second end point of the Plane

	return fSecondPointName;
}


std::string	TSpatialPlaneName::getName() const
{// gets the Plane's name
	return fName;
}


TAngle	TSpatialPlaneName::getAngle() const
{// gets the Plane's name
	return fAngle;
}


void	TSpatialPlaneName::buildErrorName()
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
bool	TSpatialPlaneName::isDefine() const
{//return true if the plane is define
	bool retVal = false;
	if(fName != "")
	{
		retVal = true;
	}
	return retVal;
}
