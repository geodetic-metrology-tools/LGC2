// TAInstrument.h
/*!
	Abstract Base Class. Defines the methods common to all instrument stations
	
	Pattern:

	Copyright 2002 CERN EST/SU. All rights reserved.
*/

#ifndef SU_TA_INSTRUMENT
#define SU_TA_INSTRUMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//include files
#include <string>
//using namespace std;

#include "TAFreeInstrument.h"

//! An abstract base class for free instrument stations
class TAInstrument : public TAFreeInstrument //: public TVParentMeasTreeNode{
{
public:

	const TLength* getInstrumentHeight() const { return instrumentHeight; }
	const TLength* getInstrumentHeightSigma() const { return instrumentHeightSigma; }

	virtual ~TAInstrument()
	{
		delete instrumentHeight;
		delete instrumentHeightSigma;
	}

protected:

	TAInstrument(const std::string& id, const TLength* ics, const TLength* ih, const TLength* ihs);

	const TLength* instrumentHeight;
	const TLength* instrumentHeightSigma;		
};
#endif

/*@{*/

//#include "TWorkingPoints.h"

//#include "TVParentMeasTreeNode.h"
//#include "TVStationListener.h"


	/*@name Typedefs*/
	//{
	//Defines the container for the station listeners as a type
//	typedef list< TVStationListener* >  StListnrContainer;
	//Defines an iterator to a station listener as a type
//	typedef StListnrContainer::iterator StListnrIterator;
	//@}

	
	/*@name Inherited from TVParentMeasTreeNode*/
	//{
	/* Reacts to the disactivation of a child measurements tree node
	@param measCount the number of disactivated measurements of the child
	@param eqCount the number of disactivated equations of the child
	@param unkCount the number of disactivated unknowns of the child
	*/
//	virtual void	childDisactivated(int measCount, int eqCount, int unkCount);
	/* Reacts to the activation of a child measurements tree node
	@param measCount the number of disactivated measurements of the child
	@param eqCount the number of disactivated equations of the child
	@param unkCount the number of disactivated unknowns of the child
	*/
//	virtual void	childActivated(int measCount, int eqCount, int unkCount);

/*	virtual void	childReadyToActivate(int measCount, int eqCount, int unkCount) = 0;
	virtual void	activateReadyChildren() const = 0;*/

	//Disactivates the station
//	virtual void		disactivate();
	//Activates the station
//	virtual void		activate();
	/*@return a true boolean if the station is active*/
//	virtual bool		isActive() const;
	/*@return the station's number of active measurements*/
//	virtual int			measurementsCount() const;
	/*@return the station's number of active equationss*/
//	virtual int			equationsCount() const;
	/*@return the station's number of active unknowns*/
//	virtual int			unknownsCount() const;
	/*Updates the station's counts.
	@param measUpdate the update to the station's active measurements count
	@param eqUpdate the update to the station's active equations count
	@param unkUpdate the update to the station's active unknowns count
	*/
//	virtual void		updateCounts(int measUpdate, int eqUpdate, int unkUpdate);
	//Adds a listener to the station's broadcast list
//	virtual void		addListener(TVStationListener* stListnr);
	
		


/*	virtual void		updateTotalEqCount(int totEqCountUpdate);
	virtual void		sendEnoughTotalEqns();
	virtual void		childReadyToActivate(int, int, int);
	virtual void		activateReadyChildren();


	virtual bool	enoughTotalEqns() const;
*/

/*private:
	int								fTotalEqnNumber;

	int								fActiveMeasCount;
	int								fActiveEqCount;
	int								fActiveUnkCount;

	int								fReadyToActivateEqns;

	bool							fUsedInCalc;

	StListnrContainer				fStListeners;
*/
