// TRadialOffsetCnstr.cpp
/*
Class for a radial constraint 

Patterns:

Copyright 2003 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////
//forward declarations

#include  "TRadialOffsetCnstr.h"
////////////////////////////////////////////////////////////////


//ClassImp(TRadialOffsetCnstr)


//////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TRadialOffsetCnstr::TRadialOffsetCnstr():
fConstraintBearing(LITERAL(0.0)), fConstraintWidth(LITERAL(0.0)), fPoint()
{//Default constructor	
	fName = "";
	fIdentifier = 0;
	fComment = ""; 
	fHeaderComment = "";

}


TRadialOffsetCnstr::TRadialOffsetCnstr(TSpatialPointName cPtName, TLength cWidth, TAngle cBearing):
fConstraintBearing(cBearing), fConstraintWidth(cWidth)
{//Constructor
	fPoint = cPtName;
	fName = cPtName.getName(); 
	fIdentifier = 0;
	fComment = ""; 
	fHeaderComment = "";
}


TRadialOffsetCnstr::TRadialOffsetCnstr( const  TRadialOffsetCnstr& source)
{//Copy constructor	
	(*this) = source;
}


TRadialOffsetCnstr&  TRadialOffsetCnstr::operator=(const TRadialOffsetCnstr& right)
{//Copy Assignment operator	
	if (this != &right)
	{
		fPoint	= right.fPoint;
		fName = right.fName;
		fConstraintBearing = right.fConstraintBearing;
		fConstraintWidth = right.fConstraintWidth;
		fIdentifier = right.fIdentifier;
		fComment = right.fComment; 
		fHeaderComment = right.fHeaderComment;
	}
	return *this;
}


TRadialOffsetCnstr::~TRadialOffsetCnstr()
{//destructor
}


////////////////////////////////////////////////////////////////
//OVERLOAD OPERATORS
////////////////////////////////////////////////////////////////
bool TRadialOffsetCnstr::operator<( const TRadialOffsetCnstr& right) const
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


bool TRadialOffsetCnstr::operator==( const TRadialOffsetCnstr& right) const
{//Equality operator
	if (fPoint == (right.fPoint) &&
		fName == (right.fName) &&
		fConstraintBearing == (right.fConstraintBearing) &&
		fConstraintWidth == (right.fConstraintWidth) &&
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
TAngle TRadialOffsetCnstr::getCnstrBearing() const
{
	return fConstraintBearing;
}


TLength TRadialOffsetCnstr::getCnstrSigma() const
{
	return fConstraintWidth;
}
// Previous return value: TSpatialPointName
TSpatialPointName TRadialOffsetCnstr::getPoint() const
{
	return fPoint;
}

string TRadialOffsetCnstr::getName() const
{
	return fName;
}


//////////////////////////////////////////////////////////////////////
//SETTING METHODS
//////////////////////////////////////////////////////////////////////
void	TRadialOffsetCnstr::ifNotDoneSetSigma(TLength cWidth)
{//Sets the constraint's width if it isn't done yet

	if ( fConstraintWidth.getMetresValue() == LITERAL(0.0))
		fConstraintWidth = cWidth;
}


void	TRadialOffsetCnstr::ifNotDoneSetCnstrBearing(TAngle cBear)
{//Sets the constraint's width if it isn't done yet

	if ( fConstraintBearing.getGonsValue() == LITERAL(0.0))
		fConstraintBearing = cBear;
}


void	TRadialOffsetCnstr::ifNotDoneSetName(string name)
{//Sets the constraint's width if it isn't done yet

	if ( fName == "")
		fName = name;
}


/*!adds this constraint to the relevant dist. meas. station
@param	wp a pointer to the working points object
@param	ws a pointer to the working stations object*/
/*bool TRadialOffsetCnstr::addToStation(TWorkingPoints* wp, TWorkingStations* ws TSpatialPointName point){



	//check if the constrained point exists
	TWorkingPoints::PointIterator noSuchPoint = wp->getPointsEndIterator();
	TWorkingPoints::PointIterator cPoint = wp->getPoint(fName);

	if ( cPoint == noSuchPoint)
		return false;
	else{
		TWorkingStations::DistStIterator noSuchStation = ws->getDistStEndIterator();
		TWorkingStations::DistStIterator searchedStation = ws->getDistStation(cPoint);
		//station doesn't already exist
		if (searchedStation == noSuchStation){

			TDistMeasStation* newDSt = new TDistMeasStation(ws, cPoint);
			delete newDSt;
			searchedStation = ws->getDistStation(cPoint);

			fParentStation = &(*searchedStation);

			fActiveMeasCount = 0;
			fActiveEqCount = 1;
			fActiveUnkCount = 0;

			
			TRadialOffsetCnstr* ro = fParentStation->addRadOffCnstr(this);
			//cPoint->addListener(static_cast<TVSpatialPtListener*>(ha));

			return true;
		}
		//station already exists
		else{

			fParentStation = &(*searchedStation);

			fActiveMeasCount = 0;
			fActiveEqCount = 1;
			fActiveUnkCount = 0;

			
			TRadialOffsetCnstr* ro = fParentStation->addRadOffCnstr(this);
			//cPoint->addListener(static_cast<TVSpatialPtListener*>(ha));

			return true;
		}
	}
}
*/



		