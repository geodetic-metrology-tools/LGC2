// TOffsetToVerLineMeasurement.cpp 
/*!
	Class for a horizontal distance between a point and a vertical line
*/


#include "TOffsetToVerLineMeasurement.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TOffsetToVerLineMeasurement::TOffsetToVerLineMeasurement():TALineMeasurement()
{//default constructor
}


TOffsetToVerLineMeasurement::TOffsetToVerLineMeasurement(const TSpatialLineName targetName, const TLength obsDist, const TLength sigma):
TALineMeasurement(targetName, obsDist, sigma)
{//constructor
}


TOffsetToVerLineMeasurement::TOffsetToVerLineMeasurement(const TOffsetToVerLineMeasurement& source)
{//copy constructor
	(*this) = source;
}


TOffsetToVerLineMeasurement::~TOffsetToVerLineMeasurement()
{//destructor
}




////////////////////////////////////////////////////////////////////////////////////
//Member function
////////////////////////////////////////////////////////////////////////////////////
TOffsetToVerLineMeasurement&	TOffsetToVerLineMeasurement::operator=(const TOffsetToVerLineMeasurement& source)
{//copy assignement operator
	this->TALineMeasurement::operator =(source);
	return (*this);
}


string				TOffsetToVerLineMeasurement::getMeasKind() const
{//returns the string representing the measurement kind
	string s("ECVE measurement");
	return s;
}




////////////////////////////////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////////////////////////////////



