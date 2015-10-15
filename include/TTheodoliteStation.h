// 
// TTheodoliteStation.h : header file
//
// Class for a theodolite instrument station
//
//  Patterns:
//  
//  
//  Copyright 2002 CERN EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////


#ifndef SU_THEODOLITE_STATION
#define SU_THEODOLITE_STATION

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

//////////////////////////////////////////
// Forward declarations
//
//class TWorkingStations;

#include <list>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;


#include "TAInstrumentStation.h"
#include <THorAngleROM.h>
#include <TZenithDistROM.h>
#include <TGyroOrientationROM.h>
#include <TLength.h>
#include "TAngleConstants.h"

//typedefs
typedef string InstNb;

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a theodolite instrument station
class TTheodoliteStation : public TAInstrumentStation{ 

public:

	/*!@name Constructors / Destructor*/
	//@{
	/*! Default Constructor*/
	TTheodoliteStation();
	/*! Constructor
		@param iterSetupPt an iterator pointing to the setup TSpatialPoint*/
	TTheodoliteStation(TSpatialPointName iterSetupPt);
	/*! Copy constructor */
	TTheodoliteStation(const TTheodoliteStation& source);
	/*! Destructor */
	~TTheodoliteStation();
	//@}

	/*@name Overloaded functions */
	//@{
	/*!copy assignement operator (shallow copy) */
	TTheodoliteStation& operator=(const TTheodoliteStation& source);
	//@}
	
	/*! Returns a non-const pointer to the object TTheodoliteStation */
	virtual TTheodoliteStation*				getPointer() const;
	/*! Returns the instrument's height of this TTheodoliteStation */
	virtual TLength							getInstrumentHeight() const;
	/*! Returns the instrument's height status of this TTheodoliteStation */
	virtual TAMeasurement::ECalcStatus		getInstHeightStatus() const;
	/*! Returns the instrument's angle constant of this TTheodoliteStation */
	virtual TAngleConstants					getAngleConst() const;
	/*! get the boolean indicating a corresponding TDistStation*/
	virtual bool							getAssociatedDistStForSpaDist() const;
	/*! get the boolean indicating a corresponding TDistStation*/
	virtual bool							getAssociatedDistStForOffset() const;

	
	/*! Sets the instrument's height of this TTheodoliteStation
	\param height instrument's height as a TLength*/
	virtual void							setInstrumentHeight(const TLength height);
	/*! Sets the variable/fixed status of the station's instrument height
	@param ihStatus one of the status defined in the ECalcStatus of the TAMEasurement class*/
	virtual void							setInstHeightStatus(const TAMeasurement::ECalcStatus ihstatus);
	/*! Sets the instrument's angle constant of this TTheodoliteStation
	@param angConst angle constant as a TAngleConstants*/
	virtual void							ifNotDoneSetAngleConst(const TAngleConstants angConst);
	/*! Sets the instrument's angle constant's name of this TTheodoliteStation
	@param constName angle constant as a string*/
	virtual void							setAngleConstName(const string constName);
	/*! Turn the boolean indicating a corresponding TDistStation to true*/
	virtual bool							setAssociatedDistStForSpaDist();
	/*! Turn the boolean indicating a corresponding TDistStation to true*/
	virtual bool							setAssociatedDistStForOffset();
	


	/*!@name Horizontal angle round of measurements related*/
	//@{
		/*! Returns the number of Hor. Dist. rom made in the container*/
		virtual int								getHorAngROMDimension() const {return fLastHAngleSeriesNbr;}
	
		/*! Adds a horizontal angle ROM to the station
		@param haROM a pointer to the horizontal angle ROM to be inserted. NB : a copy of the ROM pointed to will be made*/
		virtual THorAngleROM*					addHorAngleROM(THorAngleROM *haROM);

		/*! Returns a boolean indicating if the pointed to horizontal angle ROM is already stored by the station*/
		virtual bool							notInContainer(THorAngleROM *) const;
		
		/*! Returns an iterator pointing to the first hor. angle ROM of the station*/
		virtual HorAngROMIterator				getHorAngROMBeginIterator();
		
		/*! Returns a const iterator pointing to the first hor. angle ROM of the station*/
		virtual HorAngROMConstIter				getHorAngROMBeginIterator() const;

		/*! Returns an iterator pointing to one past the last hor. angle ROM of the station*/
		virtual HorAngROMIterator				getHorAngROMEndIterator();

		/*! Returns a const iterator pointing to one past the last hor. angle ROM of the station*/
		virtual HorAngROMConstIter				getHorAngROMEndIterator() const;

		/*!@param series the string representing the fSeries attribute of the THorAngleROM object looked for
		@return an iterator pointing to the hor. angle ROM identified by the series string*/
		virtual HorAngROMIterator				getHorAngROM(string series);

		/*!@param series the string representing the fSeries attribute of the THorAngleROM object looked for
		@return a const iterator pointing to the hor. angle ROM identified by the series string*/
		virtual HorAngROMConstIter				getHorAngROM(string series) const;
	//@}

	/*!@name Zenithal distance round of measurements related*/
	//@{
		/*!\returns the number of Hor. Dist. rom made in the container*/
		virtual int								getZenithDistROMDimension() const {return fLastZDistSeriesNbr;}
		
		/*!Adds a zenithal distance ROM to the station
		@param zdROM a pointer to the zenithal distance ROM to be inserted. NB : a copy of the ROM pointed to will be made*/
		virtual TZenithDistROM*					addZenithDistROM(TZenithDistROM *zdROM);
		
		/*!\returns a boolean indicating if the pointed to zenithal distance ROM is already stored by the station*/
		virtual bool							notInContainer(TZenithDistROM *) const;
		
		/*!\returns an iterator pointing to the first zenithal distance ROM of the station*/
		virtual ZenithDistROMIterator			getZenithDistROMBeginIterator();
		
		/*!\returns a const iterator pointing to the first zenithal distance ROM of the station*/
		virtual ZenithDistROMConstIter			getZenithDistROMBeginIterator() const;
		
		/*!\returns an iterator pointing to one past the last zenithal distance ROM of the station*/
		virtual ZenithDistROMIterator			getZenithDistROMEndIterator();
		
		/*!\returns a const iterator pointing to one past the last zenithal distance ROM of the station*/
		virtual ZenithDistROMConstIter			getZenithDistROMEndIterator() const;
		
		/*!\param series the string representing the fSeries attribute of the THorAngleROM object looked for
		\return an iterator pointing to the zenithal distance ROM identified by the series string*/
		virtual ZenithDistROMIterator			getZenithDistROM(string series);
		
		/*!\param series the string representing the fSeries attribute of the THorAngleROM object looked for
		\return a const iterator pointing to the zenithal distance ROM identified by the series string*/
		virtual ZenithDistROMConstIter			getZenithDistROM(string series) const;
	//@}

	/*!@name Gyro Orientation round of measurements related*/
	//@{
		/*!\returns the number of Orie ROM made in the container*/
		virtual int						getGyroOrieROMDimension() const {return fLastGyroOrieSeriesNbr;}

		/*!Adds a Orie ROM to the station
		\param orieROM a pointer to the Orie ROM to be inserted.
		 NB : a copy of the ROM pointed to will be made*/
		virtual TGyroOrientationROM*	addGyroOrieROM(TGyroOrientationROM* orieROM);

		/*!\returns a boolean indicating if the pointed to Orie ROM is already stored by the station*/
		virtual bool					notInContainer(TGyroOrientationROM*) const;
	
		/*!\returns an iterator pointing to the first orie ROM of the station*/
		virtual GyroOrieROMIterator		getGyroOrieROMBeginIterator();

		/*!\returns a const iterator pointing to the first orie ROM of the station*/
		virtual GyroOrieROMConstIter	getGyroOrieROMBeginIterator() const;
	
		/*!\returns an iterator pointing to one past the last orie ROM of the station*/
		virtual GyroOrieROMIterator		getGyroOrieROMEndIterator();
	
		/*!\returns a const iterator pointing to one past the last orie ROM of the station*/
		virtual GyroOrieROMConstIter	getGyroOrieROMEndIterator() const;

		/*!\param series the string representing the fSeries attribute of the TGyroOrientationROM object looked for
		\return an iterator pointing to the orie ROM identified by the series string*/
		virtual GyroOrieROMIterator		getGyroOrieROM(string series);
	
		/*!\param series the string representing the fSeries attribute of the TGyroOrientationROM object looked for
		\return a const iterator pointing to the orie ROM identified by the series string*/
		virtual GyroOrieROMConstIter	getGyroOrieROM(string series) const;
	//@}

protected :


private :
	int								fLastHAngleSeriesNbr;/*!<last number of horizontal angles series */
	int								fLastZDistSeriesNbr;/*!<last number of last zenital distances series */
	int								fLastGyroOrieSeriesNbr;/*!<last number of last orientation series */
	
	HorAngROMContainer				fMadeAngleROMs;/*!<list of made horizontal angles ROM*/
	ZenithDistROMContainer			fMadeZDistROMs;/*!<list of made zenital distance ROM*/
	GyroOrieROMContainer			fMadeGyroOrieROMs;/*!<list of made orientation ROM*/
	
	TLength							fInstrumentHeight; /*!< estimated instrument's height as a TLength */
	TAMeasurement::ECalcStatus		fInstHeightStatus; /*!< instrument's height status (variable or fixed) */

	TAngleConstants					fAngleConst;/*!< angle constant for the station (for horizontal angle measurements)*/

	bool							fDistStAssociatedForSpaDist;/*< boolean indicating if there is a corresponding distance measurement station*/
	bool							fDistStAssociatedForOffset;/*< boolean indicating if there is a corresponding distance measurement station*/

};

#endif

/*@}*/


/*!@name Typedefs*/
//@{
//!Type of the container used to store theodolite stations
typedef list< TTheodoliteStation > TheodStContainer;
//!Type of an iterator pointing to an element of the theodolite stations container
typedef TheodStContainer::iterator TheodStIterator;
//!Type of a const iterator pointing to an element of the theodolite stations container
typedef TheodStContainer::const_iterator TheodStConstIter;
//@}	













//#include "TUnknownLength.h"
//#include "UEOIndices.h"
//#include "TZenithDistROM.h"
	
	//virtual void							sendDisactivatedToChildren();
	//virtual void							sendActivatedToChildren();

//	TUnknownLength					fInstrumentHeight;



	//inherited from TVParentMeasTreeNode
	//virtual void							childReadyToActivate(int measCount, int eqCount, int unkCount);
	//virtual void							activateReadyChildren();
