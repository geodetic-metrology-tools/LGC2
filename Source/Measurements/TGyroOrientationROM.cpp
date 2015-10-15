// 
// TGyroOrientationROM.cpp : implementation File
//
// Class for a orientation round of measurements


#include "TSpatialPosition.h"
#include "TGyroOrientationROM.h"

/////////////////////////////////////////////////////////////////////////////////////
// constructor / destructor
/////////////////////////////////////////////////////////////////////////////////////
TGyroOrientationROM::TGyroOrientationROM():
TAUnreferencedROM()
{//constructor
}

TGyroOrientationROM::~TGyroOrientationROM()
{//destructor 
}

////////////////////////////
//Member functions
////////////////////////////
TGyroOrientationROM& TGyroOrientationROM::operator=(const TGyroOrientationROM& source)
{//copy assignement operator
	fMeasuredAngles = source.fMeasuredAngles;
	fSeries = source.fSeries;
	return (*this);
}


TGyroOrientationROM* TGyroOrientationROM::getPointer() const
{//returns a non-const pointer to this object
	return const_cast<TGyroOrientationROM*>(this);
}


THorAngleMeasurement* TGyroOrientationROM::addGyroOrientation(THorAngleMeasurement* horang)
{//adds a horizontal angle to the set
//	if (notInContainer(horang))
//	{
		fMeasuredAngles.push_back(*horang);
		return &(*(--fMeasuredAngles.end()));
/*	}
	else
	{
		cerr << "GyroOrientationMeasurement not inserted : already stored\n";
		return 0;
	}*/
}


bool TGyroOrientationROM::notInContainer(THorAngleMeasurement* ha) const
{//checks if the horizontal angle is already in the container

	HorAngMeasConstIter iter = getGyroOrieMeasBeginIterator();
	HorAngMeasConstIter iterEnd = getGyroOrieMeasEndIterator();

	bool notPresent = true;

	while (iter != iterEnd)
	{
		if ((*ha) == (*iter))
		{
			notPresent = false;
			iter  =iterEnd;
		}
		else
		{
			iter ++;
		}
	}
	return notPresent;
}


HorAngMeasIterator TGyroOrientationROM::getGyroOrieMeas( TSpatialPointName iterTg)
{//returns an iterator pointing to the angle meas made at the specified point

	HorAngMeasIterator iter = fMeasuredAngles.begin();
	HorAngMeasIterator iterEnd = fMeasuredAngles.end();

	bool notFound = true;
	while ((iter != iterEnd) && notFound){
		if ((iter->getTargetPoint()) == iterTg)
			notFound = false;
		else
			iter++;
	}
	return iter;
}


HorAngMeasConstIter TGyroOrientationROM::getGyroOrieMeas(TSpatialPointName iterTg) const
{//returns a const iterator pointing to the angle meas made at the specified point

	HorAngMeasConstIter iter = fMeasuredAngles.begin();
	HorAngMeasConstIter iterEnd = fMeasuredAngles.end();

	bool notFound = true;
	while ((iter != iterEnd) && notFound){
		if ((iter->getTargetPoint()) == iterTg)
			notFound = false;
		else
			iter++;
	}
	return iter;
}


HorAngMeasIterator TGyroOrientationROM::getGyroOrieMeasBeginIterator()
{//returns an iterator pointing to the first angle of the set
	return fMeasuredAngles.begin();
}


HorAngMeasConstIter TGyroOrientationROM::getGyroOrieMeasBeginIterator() const
{//returns a const iterator pointing to the first angle of the set
	return fMeasuredAngles.begin();
}


HorAngMeasIterator TGyroOrientationROM::getGyroOrieMeasEndIterator()
{//returns an iterator pointing one past the last angle of the set
	return fMeasuredAngles.end();
}


HorAngMeasConstIter TGyroOrientationROM::getGyroOrieMeasEndIterator() const
{//returns a const iterator pointing one past the last angle of the set
	return fMeasuredAngles.end();
}



