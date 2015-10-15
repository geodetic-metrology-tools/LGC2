// 
// TWorkingStations.h : Header File
//

//

#ifndef SU_WORKING_STATIONS
#define SU_WORKING_STATIONS

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
//
#include "TTheodoliteStation.h"
#include "TDistMeasStation.h"
#include "TWPSStation.h"
#include "TLevelStation.h"
#include "TWTWDistStation.h"

#include "TSpatialPoint.h"

#include <list>
#include <string>
#include <sstream>
using namespace std;



#include "UEOIndices.h"


//!Class storing all the instrument stations
class	TWorkingStations{

public:

	
	/*!@name Constructors / Destructor*/
	//@{
	//!Default constructor
	TWorkingStations();
	//!Copy constructor
	TWorkingStations(const TWorkingStations& source);
	//!Destructor
	~TWorkingStations();
	//@}

	//void			CheckComputability();
	//! Adds a theodolite station to the working stations
	/*!@param thSt a pointer to the theodolite station which will added by copy into the container
	@return a pointer to the station stored in the container*/
	virtual TTheodoliteStation*	addTheodStation(TTheodoliteStation* thSt);
	/*!@return a true boolean if a copy of the pointed to station isn't stored in the container yet
	@param thSt a pointer to the station object a copy of which will be looked for*/
	virtual bool				notInContainer(TTheodoliteStation* thSt) const;
	//! Adds an distance measuring station to the working stations
	/*!@param dSt a pointer to the distance measuring station which will added by copy into the container
	@return a pointer to the station stored in the container*/
	virtual TDistMeasStation*		addDistStation(TDistMeasStation* dSt);
	/*!@return a true boolean if a copy of the pointed to station isn't stored in the container yet
	@param dSt a pointer to the station object a copy of which will be looked for*/
	virtual bool				notInContainer(TDistMeasStation* dSt) const;
	//! Adds a WPS station to the working stations
	/*!@param wpsSt a pointer to the WPS station which will added by copy into the container
	@return a pointer to the station stored in the container*/
	virtual TWPSStation*		addWPSStation(TWPSStation* wpsSt);
	/*!@return a true boolean if a copy of the pointed to station isn't stored in the container yet
	@param wpsSt a pointer to the station object a copy of which will be looked for*/
	virtual bool				notInContainer(TWPSStation* wpsSt) const;
	//! Adds a level station to the working stations
	/*!@param lSt a pointer to the level station which will added by copy into the container
	@return a pointer to the station stored in the container*/
	virtual TLevelStation*		addLevelStation(TLevelStation* lSt, bool isDLEV = false);
	/*!@return a true boolean if a copy of the pointed to station isn't stored in the container yet
	@param lSt a pointer to the station object a copy of which will be looked for*/
	virtual bool				notInContainer(TLevelStation* lSt, bool isDLEV = false) const;
	//! Adds a WTW station to the working stations
	/*!@param wtwSt a pointer to the WTW station which will added by copy into the container
	@return a pointer to the station stored in the container*/
	virtual TWTWDistStation*	addWTWStation(TWTWDistStation* wtwSt);
	/*!@return a true boolean if a copy of the pointed to station isn't stored in the container yet
	@param lSt a pointer to the station object a copy of which will be looked for*/
	virtual bool				notInContainer(TWTWDistStation* wtwSt) const;




	/*!@return the number of stored theodolite stations*/
	virtual int			numberOfTheodStations();
	/*!@return the number of stored distance measuring stations*/
	virtual int			numberOfDistMeasStations();
	/*!@return the number of stored WPS stations*/
	virtual int			numberOfWPSStations();
	/*!@return the number of stored level stations*/
	virtual int			numberOfLevelStations(bool isDLEV);
	/*!@return the number of stored WTW stations*/
	virtual int			numberOfWTWStations();

	/*!@name Access methods*/
	//@{
	/*!@return an iterator pointing to the first element of the theodolite stations container*/
	TheodStIterator		getTheodStBeginIterator();
	/*!@return a const iterator pointing to the first element of the theodolite stations container*/
	TheodStConstIter	getTheodStBeginIterator() const;
	/*!@return an iterator pointing one past the last element of the theodolite stations container*/
	TheodStIterator		getTheodStEndIterator();
	/*!@return a const iterator pointing one past the last element of the theodolite stations container*/
	TheodStConstIter	getTheodStEndIterator() const;
	/*!	@return an iterator pointing to a specific theodolite station (if not found the end iterator is returned)
	@param setupPt an iterator pointing to the station's setup point*/
	TheodStIterator		getTheodStation(TSpatialPointName setupPt);
	/*!	@return a const iterator pointing to a specific theodolite station (if not found the end iterator is returned)
	@param setupPt an iterator pointing to the station's setup point*/
	TheodStConstIter	getTheodStation(TSpatialPointName setupPt) const;
	
	/*!@return an iterator pointing to the first element of the distance meas. stations container*/
	DistStIterator		getDistStBeginIterator();
	/*!@return a const iterator pointing to the first element of the distance meas. stations container*/
	DistStConstIter		getDistStBeginIterator() const;
	/*!@return an iterator pointing one past the last element of the EDM stations container*/
	DistStIterator		getDistStEndIterator();
	/*!@return a const iterator pointing one past the last element of the EDM stations container*/
	DistStConstIter		getDistStEndIterator() const;
	/*!	@return a const iterator pointing to a specific dist. meas. station (if not found the end iterator is returned)
	@param setupPt an iterator pointing to the station's setup point*/
	DistStIterator		getDistStation(TSpatialPointName setupPt);
	/*!	@return a const iterator pointing to a specific dist. meas. station (if not found the end iterator is returned)
	@param setupPt an iterator pointing to the station's setup point*/
	DistStConstIter		getDistStation(TSpatialPointName setupPt) const;
	
	/*!@return an iterator pointing to the first element of the WPS stations container*/
	WPSStIterator		getWPSStBeginIterator();
	/*!@return a const iterator pointing to the first element of the WPS stations container*/
	WPSStConstIter		getWPSStBeginIterator() const;
	/*!@return an iterator pointing one past the last element of the WPS stations container*/
	WPSStIterator		getWPSStEndIterator();
	/*!@return a const iterator pointing one past the last element of the WPS stations container*/
	WPSStConstIter		getWPSStEndIterator() const;
	
	/*!@return an iterator pointing to the first element of the level stations container*/
	LevelStIterator		getLevelStBeginIterator(bool isDLEV);
	/*!@return a const iterator pointing to the first element of the level stations container*/
	LevelStConstIter	getLevelStBeginIterator(bool isDLEV) const;
	/*!@return an iterator pointing one past the last element of the level stations container*/
	LevelStIterator		getLevelStEndIterator(bool isDLEV);
	/*!@return a const iterator pointing one past the last element of the level stations container*/
	LevelStConstIter	getLevelStEndIterator(bool isDLEV) const;
	
	/*!@return an iterator pointing to the first element of the WTW stations container*/
	WTWStIterator		getWTWStBeginIterator();
	/*!@return a const iterator pointing to the first element of the WTW stations container*/
	WTWStConstIter		getWTWStBeginIterator() const;
	/*!@return an iterator pointing one past the last element of the WTW stations container*/
	WTWStIterator		getWTWStEndIterator();
	/*!@return a const iterator pointing one past the last element of the WTW stations container*/
	WTWStConstIter		getWTWStEndIterator() const;
	//@}



private :
	TheodStContainer	fTheodStations;
	int					fLastTheodNbr;
	DistStContainer		fDistStations;
	int					fLastDistNbr;
	WPSStContainer		fWPSStations;
	int					fLastWPSNbr;
	LevelStContainer	fLevelStations;
	int					fLastLevelNbr;
	LevelStContainer	fDigiLevelStations;
	int					fLastDigiLevelNbr;
	WTWStContainer		fWTWStations;
	int					fLastWTWNbr;


};

#endif