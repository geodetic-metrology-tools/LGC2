// TOrientationCnstr.cpp
/*
Class for a radial constraint 

Patterns:

Copyright 2003 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////
//forward declarations

#include  "TOrientationCnstr.h"
////////////////////////////////////////////////////////////////


//ClassImp(TOrientationCnstr)


//////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TOrientationCnstr::TOrientationCnstr():
fConstraintBearing(0.0), fRefPoint()
{//Default constructor	
	fName = "";
	fIdentifier = 0;
	fComment = ""; 
	fHeaderComment = "";

}


TOrientationCnstr::TOrientationCnstr(int obsID, TSpatialPointName refPtName, TAngle cBearing):
fConstraintBearing(cBearing)
{//Constructor
	fRefPoint = refPtName;
	fName = "Bearing_"+ refPtName.getName(); 
	fIdentifier = 0;
	fComment = ""; 
	fHeaderComment = "";

	observationID = obsID;
}


TOrientationCnstr::TOrientationCnstr( const  TOrientationCnstr& source)
{//Copy constructor	
	(*this) = source;
}


TOrientationCnstr&  TOrientationCnstr::operator=(const TOrientationCnstr& right)
{//Copy Assignment operator	
	if (this != &right)
	{
		fRefPoint	= right.fRefPoint;
		fName = right.fName;
		fConstraintBearing = right.fConstraintBearing;
		fIdentifier = right.fIdentifier;
		fComment = right.fComment; 
		fHeaderComment = right.fHeaderComment;
	}
	return *this;
}


TOrientationCnstr::~TOrientationCnstr()
{//destructor
}


////////////////////////////////////////////////////////////////
//OVERLOAD OPERATORS
////////////////////////////////////////////////////////////////
bool TOrientationCnstr::operator<( const TOrientationCnstr& right) const
{//Less than operator
	if (fName < (right.fName))
	{
		return true;
	}
	else
	{
		return false;
	}

}


bool TOrientationCnstr::operator==( const TOrientationCnstr& right) const
{//Equality operator
	if (fRefPoint == (right.fRefPoint) &&
		fName == (right.fName) &&
		fConstraintBearing == (right.fConstraintBearing) &&
		fIdentifier == (right.fIdentifier) &&
		fComment == (right.fComment) && 
		fHeaderComment == (right.fHeaderComment))
	{
		return true;
	}
	else
	{
		return false;
	}

}



//////////////////////////////////////////////////////////////////////
//ACCESS METHODS
//////////////////////////////////////////////////////////////////////
TAngle TOrientationCnstr::getSigma() const
{
	TAngle sigma;
	sigma.setGonsValue(0.00000001);
	return sigma;
}


TAngle TOrientationCnstr::getCnstrBearing() const
{
	return fConstraintBearing;
}

// Previous return value: TSpatialPointName
TSpatialPointName TOrientationCnstr::getPoint() const
{
	return fRefPoint;
}


string TOrientationCnstr::getName() const
{
	return fName;
}


//////////////////////////////////////////////////////////////////////
//SETTING METHODS
//////////////////////////////////////////////////////////////////////
void	TOrientationCnstr::ifNotDoneSetCnstrBearing(TAngle cBear)
{//Sets the constraint's width if it isn't done yet

	if ( fConstraintBearing.getGonsValue() == 0.0)
		fConstraintBearing = cBear;
}


void	TOrientationCnstr::ifNotDoneSetName(string name)
{//Sets the constraint's width if it isn't done yet

	if ( fName == "")
		fName = name;
}
