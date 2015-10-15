// TOffsetToTheoPlaneMeasurement.cpp 
/*!
	Class for a horizontal distance between a point and a theodolite's
	sighting line
*/


#include "TOffsetToTheoPlaneMeasurement.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TOffsetToTheoPlaneMeasurement::TOffsetToTheoPlaneMeasurement():TAPlaneMeasurement()
{//default constructor
}


TOffsetToTheoPlaneMeasurement::TOffsetToTheoPlaneMeasurement(const TSpatialPlaneName targetName, const TLength obsDist, const TLength sigma):
TAPlaneMeasurement(targetName, obsDist, sigma)
{//constructor
}


TOffsetToTheoPlaneMeasurement::TOffsetToTheoPlaneMeasurement(const TOffsetToTheoPlaneMeasurement& source)
{//copy constructor
	(*this) = source;
}


TOffsetToTheoPlaneMeasurement::~TOffsetToTheoPlaneMeasurement()
{//destructor
}




////////////////////////////////////////////////////////////////////////////////////
//Member function
////////////////////////////////////////////////////////////////////////////////////
TOffsetToTheoPlaneMeasurement&	TOffsetToTheoPlaneMeasurement::operator=(const TOffsetToTheoPlaneMeasurement& source)
{//copy assignement operator
	this->TAPlaneMeasurement::operator =(source);
	return (*this);
}


string				TOffsetToTheoPlaneMeasurement::getMeasKind() const
{//returns the string representing the measurement kind
	string s("ECTH measurement");
	return s;
}

