//
// TAUnreferencedROM.h : header file
// abstract base class holding the behaviour common to all
// unreferenced rounds of measurements
//
//  Patterns:
//  
//  
//  Copyright 2002 CERN EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////


#ifndef SU_TAUNREFERENCEDROM
#define SU_TAUNREFERENCEDROM

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//include files
#include <list>
#include <string>
using namespace std;


//typedefs
typedef string Series;

/*!\ingroup spatialmeasurements
	@{*/

//! Abstract Base Class. Defines the methods common to all unreferenced rounds of measurements
class TAUnreferencedROM //: public TVChildMeasTreeNode, public TVParentMeasTreeNode 
{

public:

	
	/*!@name Overloaded operators*/
	//@{
	/*! Less than operator */
	virtual bool	operator<(const TAUnreferencedROM& right) const;
	/*!Equality operator */
	virtual bool	operator==(const TAUnreferencedROM& right) const;
	//@}


	
	/*!@return a string representing the station's characterizing series attribute*/
	virtual string	getSeries() const;
	/*!Sets the station's series attribute */
	virtual void	setSeries(string series);


protected:

	/*!@name Constructors / Destructor*/
	//@{
	/*!Constructor
	@param parent a pointer to the parent meas tree node (instrument station)*/
	TAUnreferencedROM();
	/*!Copy constructor */
	TAUnreferencedROM(const TAUnreferencedROM& source);
	/*!Destructor */
	virtual ~TAUnreferencedROM();
	//@}

	string							fSeries; /*!< series that identify the ROM */

	
};
#endif
/*@}*/


//#include "TWorkingPoints.h"

//#include "TVChildMeasTreeNode.h"
//#include "TVParentMeasTreeNode.h"
//#include "TVRoundOfMeasListener.h"



/*!@name Typedefs*/
	//@{
	//!type of the container used to store the broadcast list
	//typedef list< TVRoundOfMeasListener* >  ROMListnrList;
	//!type of an iterator pointing to an element of the container storing the ROM listeners
	//typedef ROMListnrList::iterator ROMListnrIterator;
	//@}


	/*!@return a true boolean if the ROM is active*/
	//virtual bool		isActive() const;
	/*!@return the rom's number of active measurements*/
	//virtual int			measurementsCount() const;
	/*!@return the rom's number of active equations*/
	//virtual int			equationsCount() const;
	/*!@return the rom's number of active unknowns*/
	//virtual int			unknownsCount() const;


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


	//virtual bool	enoughTotalEqns() const;
	//virtual void	childReadyToActivate(int measCount, int eqCount, int unkCount);
	//virtual void	activateReadyChildren() = 0;

	/*!@name Inherited from TVChildMeasTreeNode*/
	//@{
	//! Reacts to the disactivation of the parent station
	//virtual void	parentDisactivated();
	//! Reacts to the activation of the parent station
	//virtual void	parentActivated();
	//@}

	//virtual void	parentEnoughTotalEqns() = 0;


	//virtual bool	readyToActivate() const;

	//!Disactivates the ROM
	//virtual void	disactivate();
	//!Activates the ROM
	//virtual void	activate();
	/*! Adds a listener of this ROM to its broadcast list
	@param romListnr a pointer to the object listening to this ROM*/
	//virtual void	addListener(TVRoundOfMeasListener* romListnr);



	//int								fTotalEqnNumber;

	//int								fActiveMeasCount;
	//int								fActiveEqCount;
	//int								fActiveUnkCount;

	//ROMListnrList					fROMListeners;
	
//private:

	//bool							fUsedInCalc;
	//bool							fParentUsedInCalc;
	//bool							fReadyToActivate;


	//TVParentMeasTreeNode*			fParentStation;