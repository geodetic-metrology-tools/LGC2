// TOffsetToVerPlaneMeasurement.cpp 
/*!
	Class for a horizontal distance between a point and a line
*/


#include "TOffsetToVerPlaneMeasurement.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TOffsetToVerPlaneMeasurement::TOffsetToVerPlaneMeasurement():TAPlaneMeasurement()
{//default constructor
}


TOffsetToVerPlaneMeasurement::TOffsetToVerPlaneMeasurement(const TSpatialPlaneName targetName, const TLength obsDist, const TLength sigma):
TAPlaneMeasurement(targetName, obsDist, sigma)
{//constructor
}


TOffsetToVerPlaneMeasurement::TOffsetToVerPlaneMeasurement(const TOffsetToVerPlaneMeasurement& source)
{//copy constructor
	(*this) = source;
}


TOffsetToVerPlaneMeasurement::~TOffsetToVerPlaneMeasurement()
{//destructor
}




////////////////////////////////////////////////////////////////////////////////////
//Member function
////////////////////////////////////////////////////////////////////////////////////
TOffsetToVerPlaneMeasurement&	TOffsetToVerPlaneMeasurement::operator=(const TOffsetToVerPlaneMeasurement& source)
{//copy assignement operator
	this->TAPlaneMeasurement::operator =(source);
	return (*this);
}


string				TOffsetToVerPlaneMeasurement::getMeasKind() const
{//returns the string representing the measurement kind
	string s("ECHO measurement");
	return s;
}

