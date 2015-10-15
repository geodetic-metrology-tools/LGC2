// 
// TAInstrumentAStation.h : header file
/*! 
	abstract base class holding what the behaviour common to all
	instrument stations

	Patterns:
  
	Copyright 2002 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////

#ifndef SU_TAINSTRUMENTSTATION
#define SU_TAINSTRUMENTSTATION

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//include files
#include <list>
using namespace std;

#include "TSpatialPointName.h"


//typedefs
typedef string InstNb;

/*!\ingroup spatialmeasurements
	@{*/

//! Abstract Base Class. Defines the methods common to all instrument stations 
class TAInstrumentStation //: public TVParentMeasTreeNode, public TASpatialPtListener 
{
public:

	
	/*!@name Overloaded operators*/
	//@{
	/*!Less than operator*/
	virtual	bool		operator<(const TAInstrumentStation& right) const;
	/*!Equality operator*/
	virtual	bool		operator==(const TAInstrumentStation& right) const;
	//@}

	/*!@name Access methods */
	//@{
	/*! Returns the setup attribute characterizing a specific station*/
	virtual string	getSetup() const;
	/*! Returns an iterator to the stationed TSpatialPoint */
	virtual TSpatialPointName	getStationedPoint() const;
	/* Returns the instruments number */
	virtual InstNb		getInstrumentNumber() const;
	//@}

	/*! Sets the setup attribute */
	virtual void	setSetup(string setup);


protected:

	/*!@name Constructors / Destructor*/
	//@{
	/*! Default Constructor*/
	TAInstrumentStation();
	/*! Constructor 
		\param setupPt station's setup point's name*/
	explicit TAInstrumentStation(TSpatialPointName setupPt);
	/*! Constructor 
		\param setupPt station's setup point's name
		\param in instrument's number */
	TAInstrumentStation(TSpatialPointName setupPt, InstNb in);
	/*!Copy constructor */
	TAInstrumentStation(const TAInstrumentStation& source);
	/*!Destructor */
	virtual ~TAInstrumentStation();
	//@}


	InstNb							fInstrumentNumber;/*!<instrument number*/
	string							fSetup;/*!<setup attribute*/
	TSpatialPointName				fStationedPoint;/*!<name of the stationed point*/

};
#endif

/*@}*/
//#include "TWorkingPoints.h"
//#include "TVParentMeasTreeNode.h"
//#include "TASpatialPtListener.h"
//#include "TVStationListener.h"

/*!@name Typedefs*/
	//@{
	//!Defines the container for the station listeners as a type
	//typedef list< TVStationListener* >  StListnrContainer;
	//!Defines an iterator to a station listener as a type
	//typedef StListnrContainer::iterator StListnrIterator;
	//@}

	
	/*!@name Inherited from TVParentMeasTreeNode*/
	//@{
	/*! Reacts to the disactivation of a child measurements tree node
	@param measCount the number of disactivated measurements of the child
	@param eqCount the number of disactivated equations of the child
	@param unkCount the number of disactivated unknowns of the child
	*/
	//virtual void	childDisactivated(int measCount, int eqCount, int unkCount);
	/*! Reacts to the activation of a child measurements tree node
	@param measCount the number of disactivated measurements of the child
	@param eqCount the number of disactivated equations of the child
	@param unkCount the number of disactivated unknowns of the child
	*/
	//virtual void	childActivated(int measCount, int eqCount, int unkCount);
	//@}

	//virtual void	childReadyToActivate(int measCount, int eqCount, int unkCount) = 0;
	//virtual void	activateReadyChildren() = 0;

	/*!@name Inherited from TASpatialPtListener*/
	//@{
	//!Reacts to the disactivation of the setup point
	//virtual void	pointDisactivated();
	//!Reacts to the activation of the setup point
	//virtual void	pointActivated();
	//@}


	//!Disactivates the station
	//virtual void		disactivate();
	//!Activates the station
	//virtual void		activate();
	/*!@return a true boolean if the station is active*/
	//virtual bool		isActive() const;
	/*!@return the station's number of active measurements*/
	//virtual int			measurementsCount() const;
	/*!@return the station's number of active equationss*/
	//virtual int			equationsCount() const;
	/*!@return the station's number of active unknowns*/
	//virtual int			unknownsCount() const;
	/*!Updates the station's counts.
	@param measUpdate the update to the station's active measurements count
	@param eqUpdate the update to the station's active equations count
	@param unkUpdate the update to the station's active unknowns count
	*/
	//virtual void		updateCounts(int measUpdate, int eqUpdate, int unkUpdate);
	//!Adds a listener to the station's broadcast list
	//virtual void		addListener(TVStationListener* stListnr);
	
		

	


	//virtual void		updateTotalEqCount(int totEqCountUpdate);
	//virtual void		sendEnoughTotalEqns() = 0;


	//virtual bool	enoughTotalEqns() const;





//int								fTotalEqnNumber;

	//int								fActiveMeasCount;
	//int								fActiveEqCount;
	//int								fActiveUnkCount;

	//int								fReadyToActivateEqns;

	//bool							fUsedInCalc;

	//StListnrContainer				fStListeners;