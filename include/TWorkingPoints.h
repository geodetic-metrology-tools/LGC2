// TWorkingPoints.h : Header File.
//////////////////////////////////////////////////////////////////////

#ifndef SU_WORKING_POINTS
#define SU_WORKING_POINTS

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif 


#include <list>
#include <algorithm>
#include <sstream>
#include <unordered_map>
using namespace std;


#include "TSpatialPoint.h"
#include "TPtListBroadcaster.h"
#include "TSpatialPointName.h"
#include "TVSpatialPtListener.h"
#include "TVPtListListener.h"



//! Class used to store a set of TSpatialPoints
class TWorkingPoints : public TVSpatialPtListener  
{
public:

	

	/*!@name Constructors/Destructor*/
	//@{
		//!Default constructor
		TWorkingPoints();

		TWorkingPoints(const TWorkingPoints & ori);

		//!Destructor
		virtual ~TWorkingPoints();
	//@}

	//!Adds a point to the container
	/*!@param sp a pointer to the point to be stored (the pointed to point object is copied)
	@return a true boolean if the point was added to the container (no point with the same name stored yet)*/
	bool				addPoint(TSpatialPoint* sp);


	bool insertPoint(TSpatialPoint *sp, int pos);
	/*!@deletes a point from the working point list, returning a true bool if the deletion was sucessful*/
	bool deletePoint(PointIterator iter);

	/*!@return the number of stored points*/
	int					numberOfPoints() const;

	/*!@return an iterator pointing to the first point of the container*/
	PointIterator		getPointsBeginIterator();

	/*!@return a cons iterator pointing to the first point of the container*/
	PointConstIter		getPointsBeginIterator() const;

	/*!@return an iterator pointing to one past the last point of the container*/
	PointIterator		getPointsEndIterator();

	/*!@return a cons iterator pointing to one past the last point of the container*/
	PointConstIter		getPointsEndIterator() const;

	/*!@return an iterator pointing to a point specified through its name*/
	PointIterator		getPoint(TSpatialPointName);

	/*!@return a cons iterator pointing to a point specified through its name*/
	PointConstIter		getPoint(TSpatialPointName) const;

	/*!@return an iterator pointing to a point specified through its name*/
	PointIterator		getPoint(string);

	/*!@return a cons iterator pointing to a point specified through its name*/
	PointConstIter		getPoint(string) const;

	//////////////////////////////////////////////////////////////////////////////
	PointIterator getPoint(int pos);
	PointConstIter getPoint(int pos) const;

	bool renamePoint(const std::string & oldName, const std::string & newName);

	/*!@initiates broadcaster if the header of a spatial point is changed*/
	void headerChanged(TSpatialPointName name);
	/*!@initiates broadcaster if the name of a spatial point is changed*/
	void ptNameChanged(TSpatialPointName name);
	/*!@initiates broadcaster if the position of a spatial point is changed*/
	void positionChanged(TSpatialPointName name);
	/*!@initiates broadcaster if the cumulative distance of a spatial point is changed*/
	void distChanged(TSpatialPointName name);
	/*!@initiates broadcaster if the status of a spatial point is changed*/
	void statusChanged(TSpatialPointName name);
	

	/*!@Adds a listener to the broadcast list*/
	void addListener(TVPtListListener* listener);
	/*!@Deletes a listener to the broadcast list*/
	void deleteListener(TVPtListListener* listener);
	

private:
	
	int						fLastPtNbr;
	std::unordered_map<string, PointIterator> pointsMap;
	TPtListBroadcaster*	fBroadcaster;
	SpatialPointContainer fWorkingPoints;

	
};
#endif 
