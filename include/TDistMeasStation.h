// 
// TDistMeasStation.h : Header File
/*!
	Class for a distance measuring instrument station

	Copyright 2000 CERN EST/SU. All rights reserved.
*/
 

#ifndef SU_DIST_STATION
#define SU_DIST_STATION

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
//
//class TWorkingStations;

#include <list>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

#include "TAInstrumentStation.h"
#include "TTheodoliteStation.h"
#include "THorizontalDistROM.h"
#include "T2DOffsetROM.h"
#include "TSpatialDistROM.h"
#include "TOffsetToTheoPlaneMeasurement.h"
#include "TOffsetToVerPlaneMeasurement.h"
#include "TOffsetToSpaLineMeasurement.h"
#include "TOffsetToVerLineMeasurement.h"
/////////////////////////////////////////////////////
/*! typedefs */
typedef string InstNb;

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a station measuring all kinds of distances
class TDistMeasStation : public TAInstrumentStation{ 

public:

	/*!@name Constructors / Destructor*/
	//@{
	/*!Default Constructor*/
	TDistMeasStation();
	/*!Constructor 
	\param setupPt station's setup point's name */
	TDistMeasStation(TSpatialPointName setupPt);
	/*!Destructor */
	~TDistMeasStation();
	//@}

	/*!Copy assignement operator */
	TDistMeasStation&	operator=(const TDistMeasStation& source);

	/*!Equality operator*/
	bool				operator==(const TDistMeasStation& right) const;

	/*! Returns the instrument's height of this TDistMeasStation */
	virtual TLength							getInstrumentHeight() const;
	/*! Returns the instrument's height status of this TDistMeasStation */
	virtual TAMeasurement::ECalcStatus		getInstHeightStatus() const;
	/*! Sets the instrument's height of this TDistMeasStation
	\param height instrument's height as a TLength*/
	virtual void							setInstrumentHeight(TLength height);
	/*! Sets the variable/fixed status of the station's instrument height
	@param ihStatus one of the status defined in the ECalcStatus of the TAMEasurement class*/
	virtual void							setInstHeightStatus(TAMeasurement::ECalcStatus ihstatus);

	bool					isAssociatedTheoSt() const;

	TheodStIterator	getAssociatedTheoSt() const;

	TheodStIterator	setAssociatedTheoSt(const TheodStIterator);	
	
	
	/*!@name Horizontal distance measurements related*/
	//@{
	/*! Returns the number of Hor. Dist. rom made in the container*/
	virtual int								getHorDistROMDimension() const {return fLastHorDistSeriesNbr;}
	/*! Returns an iterator pointing to the first Hor. Dist. rom of the container*/
	virtual HorDistROMIterator				getHorDistROMBeginIterator();
	/*! Returns an const iterator pointing to the first Hor. Dist. rom of the container*/
	virtual HorDistROMConstIter				getHorDistROMBeginIterator() const;
	/*! Returns  an iterator pointing to one past the last Hor. Dist. rom of the container*/
	virtual HorDistROMIterator				getHorDistROMEndIterator();
	/*! Returns  an const iterator pointing to one past the last Hor. Dist. rom of the container*/
	virtual HorDistROMConstIter				getHorDistROMEndIterator() const;
	/*! Returns an iterator pointing to the looked for Hor. Dist. rom
	\param fSeries a string identifying the requested rom */
	virtual HorDistROMIterator				getHorDistROM(string fSeries) ;
	/*! Returns an const iterator pointing to the looked for Hor. Dist. rom
	\param fSeries a string identifying the requested rom */
	virtual HorDistROMConstIter				getHorDistROM(string fSeries) const;
	/*!Adds a hor. dist. rom to the container 
	\param hdr a pointer to the rom to be copied into the container */
	virtual THorizontalDistROM*				addHorDistROM(THorizontalDistROM* hdrom);
	/*! Checks if the hor. dist. rom is in the container or not
	\param hdr a pointer to the hor. dist. rom whose presence has to be checked 
	\return a true boolean if the rom isn't in the container yet */
	virtual bool							notInContainer(THorizontalDistROM* hdr) const;
	//@}

	/*!@name Spatial distance mesaurements related */
	//@{
	/*! Returns the number of Spatial Dist. rom made in the container*/
	virtual int								getSpatialDistROMDimension() const {return fLastSpatialDistSeriesNbr;}
		/*! Returns an iterator pointing to the first Spatial Dist. rom of the container */
	virtual SpatialDistROMIterator				getSpatialDistROMBeginIterator();
	/*! Returns an const iterator pointing to the first Spatial. Dist. rom of the container*/
	virtual SpatialDistROMConstIter				getSpatialDistROMBeginIterator() const;
	/*! Returns  an iterator pointing to one past the last Spatial. Dist. rom of the container*/
	virtual SpatialDistROMIterator				getSpatialDistROMEndIterator();
	/*! Returns  an const iterator pointing to one past the last Spatial. Dist. rom of the container*/
	virtual SpatialDistROMConstIter				getSpatialDistROMEndIterator() const;
	/*! Returns an iterator pointing to the looked for Spatial. Dist. rom
	\param fSeries a string identifying the requested rom */
	virtual SpatialDistROMIterator				getSpatialDistROM(string fSeries) ;
	/*! Returns an const iterator pointing to the looked for Spatial. Dist. rom
	\param fSeries a string identifying the requested rom */
	virtual SpatialDistROMConstIter				getSpatialDistROM(string fSeries) const;
	/*! Adds a Spatial. dist. rom to the container 
	\param hdr a pointer to the rom to be copied into the container */
	virtual TSpatialDistROM*				addSpatialDistROM(TSpatialDistROM* sdrom);
	/*! Checks if the spatial. dist. rom is in the container or not
	\param hdr a pointer to the spatial. dist. rom whose presence has to be checked 
	\return a true boolean if the rom isn't in the container yet*/
	virtual bool							notInContainer(TSpatialDistROM* sdr) const;
	//@}

	/*!@name 2D offset measurements related*/
	//@{
	/*! Returns an iterator pointing to the first 2D offset ROM of the container*/
	virtual Off2DROMIterator				get2DOffsetROMBeginIterator();
	/*! Returns an iterator pointing to the first 2D offset ROM of the container*/
	virtual Off2DROMConstIter				get2DOffsetROMBeginIterator() const;
	/*! Returns an iterator pointing to one past the last 2D offset ROM of the container*/
	virtual Off2DROMIterator				get2DOffsetROMEndIterator();
	/*! Returns an iterator pointing to one past the last 2D offset ROM of the container*/
	virtual Off2DROMConstIter				get2DOffsetROMEndIterator() const;
	/*! Returns an iterator pointing to the looked for 2D offset rom
	\param fSeries a string identifying the requested rom */
	virtual Off2DROMIterator				get2DOffsetROM(string fSeries);
	/*! Returns an iterator pointing to the looked for 2D offset rom
	\param fSeries a string identifying the requested rom */
	virtual Off2DROMConstIter				get2DOffsetROM(string fSeries) const;
	/*!Adds a 2D offset rom to the container
	\param ror a pointer to the rom to be copied into the container*/
	virtual T2DOffsetROM*					add2DOffsetROM(T2DOffsetROM* ror);
	/*! Checks if the 2D offset rom is in the container or not
	\param ror a pointer to the 2D offset rom whose presence has to be checked
	\return a true boolean if the rom isn't in the container yet*/
	virtual bool							notInContainer(T2DOffsetROM* ror) const;
	//@}


	/*!@name ECVE (offset to ver. line) measurements related */
	//@{
		/*! Returns an iterator pointing to the first OffsetToVerLine Measurement of the container */
		virtual OffsetToVerLineMeasIterator		getOffsetToVerLineMeasBeginIterator();
		/*! Returns an const iterator pointing to the first OffsetToVerLine Measurement of the container*/
		virtual OffsetToVerLineMeasConstIter	getOffsetToVerLineMeasBeginIterator() const;
		/*! Returns  an iterator pointing to one past the last OffsetToVerLine Measurement of the container*/
		virtual OffsetToVerLineMeasIterator		getOffsetToVerLineMeasEndIterator();
		/*! Returns  an const iterator pointing to one past the last OffsetToVerLine Measurement of the container*/
		virtual OffsetToVerLineMeasConstIter	getOffsetToVerLineMeasEndIterator() const;
		/*! Adds a OffsetToVerLine Measurement to the container
		\param meas a pointer to the measurement to be copied into the container*/
		virtual TOffsetToVerLineMeasurement*	addOffsetToVerLineMeas(TOffsetToVerLineMeasurement* meas);
		/*! Checks if the OffsetToVerLine Measurement is in the container or not
		\param meas a pointer to the radial constraint whose presence has to be checked
		\return a true boolean if the constraint isn't in the container yet*/
		virtual bool							notInContainer(TOffsetToVerLineMeasurement* meas) const;
	//@}


	/*!@name ECSP (offset to spa. line) measurements related */
	//@{
		/*! Returns an iterator pointing to the first OffsetToVerLine Measurement of the container */
		virtual OffsetToSpaLineMeasIterator		getOffsetToSpaLineMeasBeginIterator();
		/*! Returns an const iterator pointing to the first OffsetToVerLine Measurement of the container*/
		virtual OffsetToSpaLineMeasConstIter	getOffsetToSpaLineMeasBeginIterator() const;
		/*! Returns  an iterator pointing to one past the last OffsetToVerLine Measurement of the container*/
		virtual OffsetToSpaLineMeasIterator		getOffsetToSpaLineMeasEndIterator();
		/*! Returns  an const iterator pointing to one past the last OffsetToVerLine Measurement of the container*/
		virtual OffsetToSpaLineMeasConstIter	getOffsetToSpaLineMeasEndIterator() const;
		/*! Adds a OffsetToVerLine Measurement to the container
		\param meas a pointer to the measurement to be copied into the container*/
		virtual TOffsetToSpaLineMeasurement*	addOffsetToSpaLineMeas(TOffsetToSpaLineMeasurement* meas);
		/*! Checks if the OffsetToVerLine Measurement is in the container or not
		\param meas a pointer to the radial constraint whose presence has to be checked
		\return a true boolean if the constraint isn't in the container yet*/
		virtual bool							notInContainer(TOffsetToSpaLineMeasurement* meas) const;
	//@}


	/*!@name ECHO (offset to ver. Plane) measurements related */
	//@{
		/*! Returns an iterator pointing to the first OffsetToVerLine Measurement of the container */
		virtual OffsetToVerPlaneMeasIterator	getOffsetToVerPlaneMeasBeginIterator();
		/*! Returns an const iterator pointing to the first OffsetToVerLine Measurement of the container*/
		virtual OffsetToVerPlaneMeasConstIter	getOffsetToVerPlaneMeasBeginIterator() const;
		/*! Returns  an iterator pointing to one past the last OffsetToVerLine Measurement of the container*/
		virtual OffsetToVerPlaneMeasIterator	getOffsetToVerPlaneMeasEndIterator();
		/*! Returns  an const iterator pointing to one past the last OffsetToVerLine Measurement of the container*/
		virtual OffsetToVerPlaneMeasConstIter	getOffsetToVerPlaneMeasEndIterator() const;
		/*! Adds a OffsetToVerLine Measurement to the container
		\param meas a pointer to the measurement to be copied into the container*/
		virtual TOffsetToVerPlaneMeasurement*	addOffsetToVerPlaneMeas(TOffsetToVerPlaneMeasurement* meas);
		/*! Checks if the OffsetToVerLine Measurement is in the container or not
		\param meas a pointer to the radial constraint whose presence has to be checked
		\return a true boolean if the constraint isn't in the container yet*/
		virtual bool							notInContainer(TOffsetToVerPlaneMeasurement* meas) const;
	//@}


	/*!@name ECTH (offset to theodolite Plane) measurements related */
	//@{
		/*! Returns an iterator pointing to the first OffsetToVerLine Measurement of the container */
		virtual OffsetToTheoPlaneMeasIterator	getOffsetToTheoPlaneMeasBeginIterator();
		/*! Returns an const iterator pointing to the first OffsetToVerLine Measurement of the container*/
		virtual OffsetToTheoPlaneMeasConstIter	getOffsetToTheoPlaneMeasBeginIterator() const;
		/*! Returns  an iterator pointing to one past the last OffsetToVerLine Measurement of the container*/
		virtual OffsetToTheoPlaneMeasIterator	getOffsetToTheoPlaneMeasEndIterator();
		/*! Returns  an const iterator pointing to one past the last OffsetToVerLine Measurement of the container*/
		virtual OffsetToTheoPlaneMeasConstIter	getOffsetToTheoPlaneMeasEndIterator() const;
		/*! Adds a OffsetToVerLine Measurement to the container
		\param meas a pointer to the measurement to be copied into the container*/
		virtual TOffsetToTheoPlaneMeasurement*	addOffsetToTheoPlaneMeas(TOffsetToTheoPlaneMeasurement* meas);
		/*! Checks if the OffsetToVerLine Measurement is in the container or not
		\param meas a pointer to the radial constraint whose presence has to be checked
		\return a true boolean if the constraint isn't in the container yet*/
		virtual bool							notInContainer(TOffsetToTheoPlaneMeasurement* meas) const;
	//@}




protected:


private :

	int									fLastHorDistSeriesNbr; /*!< last number of hor.dist. series */
	HorDistROMContainer					fMadeHorDistROMs; /*!< list of made hor. dist. ROMs */

	int									fLastSpatialDistSeriesNbr; /*!< last number of spatial dist. series */
	SpatialDistROMContainer				fMadeSpatialDistROMs; /*!< list of made spatial dist. ROMs */

	int									fLast2DOffsetSeriesNbr; /*!< last number of 2D offsets series */
	Off2DROMContainer					fMade2DOffsetROMs; /*!< list of made 2D offsets ROMs */

	OffsetToVerLineContainer			fECVE;
	OffsetToSpaLineContainer			fECSP;
	OffsetToVerPlaneContainer			fECHO;
	OffsetToTheoPlaneContainer			fECTH;

	TLength								fInstrumentHeight; /*!< estimated instrument's height as a TLength */
	TAMeasurement::ECalcStatus			fInstHeightStatus; /*!< instrument's height status (variable or fixed) */

	TheodStIterator						fAssociatedTheoSt;
	bool								fIsAssociatedTheoSt;
};

#endif

/*@}*/


/*!@name Typedefs*/
//@{
//!Type of the container used to store EDM stations
typedef list< TDistMeasStation > DistStContainer;
//!Type of an iterator pointing to an element of the EDM stations container
typedef DistStContainer::iterator DistStIterator;
//!Type of a const iterator pointing to an element of the EDM stations container
typedef DistStContainer::const_iterator DistStConstIter;		
//@}









//#include "UEOIndices.h"

/*	\param parent a pointer to the TWorkingStations
	\param setupPt an iterator pointing to the setup point*/
//	TDistMeasStation(TWorkingStations* parent, TWorkingPoints::PointIterator setupPt);

/*
	//inherited from TVParentMeasTreeNode
	virtual void							childReadyToActivate(int measCount, int eqCount, int unkCount);
	virtual void							activateReadyChildren();

	virtual void							sendEnoughTotalEqns();
	virtual void							sendDisactivatedToChildren();
	virtual void							sendActivatedToChildren();
*/
