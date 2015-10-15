// TOffsetToSpaLineMeasurement.cpp 
/*!
	Class for a distance between a point and a spacial line
*/


#include "TOffsetToSpaLineMeasurement.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TOffsetToSpaLineMeasurement::TOffsetToSpaLineMeasurement():TALineMeasurement()
{//default constructor
}


TOffsetToSpaLineMeasurement::TOffsetToSpaLineMeasurement(const TSpatialLineName targetName, const TLength obsDist, const TLength sigma):
TALineMeasurement(targetName, obsDist, sigma)
{//constructor
}


TOffsetToSpaLineMeasurement::TOffsetToSpaLineMeasurement(const TOffsetToSpaLineMeasurement& source)
{//copy constructor
	(*this) = source;
}


TOffsetToSpaLineMeasurement::~TOffsetToSpaLineMeasurement()
{//destructor
}




////////////////////////////////////////////////////////////////////////////////////
//Member function
////////////////////////////////////////////////////////////////////////////////////
TOffsetToSpaLineMeasurement&	TOffsetToSpaLineMeasurement::operator=(const TOffsetToSpaLineMeasurement& source)
{//copy assignement operator
	this->TALineMeasurement::operator =(source);
	return (*this);
}


string				TOffsetToSpaLineMeasurement::getMeasKind() const
{//returns the string representing the measurement kind
	string s("ECSP measurement");
	return s;
}

