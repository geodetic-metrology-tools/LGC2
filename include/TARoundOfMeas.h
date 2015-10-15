// TARoundOfMeas.h
/*!
	Abstract Base Class. Defines the methods common to all rounds of measurements referenced from a point
	
	Pattern:

	Copyright 2000 CERN EST/SU. All rights reserved.
*/

#ifndef SU_TAROUNDOFMEAS
#define SU_TAROUNDOFMEAS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//include files
#include <list>
using namespace std;

#include "TSpatialPointName.h"

//typedefs
typedef string Series;

/*! \ingroup spatialmeasurements
	@{ */

//! Abstract Base Class for a round of measurements referenced from a point
class TARoundOfMeas /*: public TVParentMeasTreeNode, public TVChildMeasTreeNode,
					public TASpatialPtListener */
{
public:

	/*!@name Overloaded operators*/
	//@{
	/*! Less than operator */
	virtual bool	operator<(const TARoundOfMeas& right) const;
	/*! Equality operator */
	virtual bool	operator==(const TARoundOfMeas& right) const;
	//@}

	/*!@name Access methods */
	//@{
	/*!@return a string representing the station's characterizing series attribute*/
	virtual string	getSeries() const;
	/*! return the reference point's name */
	virtual TSpatialPointName  getRefPtName() const;
	//@}

	//!Sets the station's series attribute
	virtual void	setSeries(string series);

protected:

	/*!@name Constructors / Destructor*/
	//@{
	/*! Default constructor */
	TARoundOfMeas();
	/*!constructor */
	TARoundOfMeas(TSpatialPointName refPt);
	/*! Copy constructor */
	TARoundOfMeas(const TARoundOfMeas& source);
	/*! Destructor */
	virtual ~TARoundOfMeas();
	//@}

	string							fSeries; /*!< identifies the serie of measurement involved */
	TSpatialPointName				fRefPtName; /*!< name of the reference point */ 

private:
	/*! Copy assignment operator */
	//virtual TARoundOfMeas	operator=(const TARoundOfMeas& right) const;


};
#endif
/*@}*/

//#include "TWorkingPoints.h"

//#include "TVParentMeasTreeNode.h"
//#include "TVChildMeasTreeNode.h"
//#include "TASpatialPtListener.h"
//#include "TVRoundOfMeasListener.h"

	/*!@name Typedefs*/
	//@{
	//!type of the container used to store the broadcast list
//	typedef list< TVRoundOfMeasListener* >  ROMListnrList;
	//!type of an iterator pointing to an element of the container storing the ROM listeners
//	typedef ROMListnrList::iterator ROMListnrIterator;
	//@}

	/*!@return a true boolean if the ROM is active*/
//	virtual bool		isActive() const;
	/*!@return the rom's number of active measurements*/
//	virtual int			measurementsCount() const;
	/*!@return the rom's number of active equations*/
//	virtual int			equationsCount() const;
	/*!@return the rom's number of active unknowns*/
//	virtual int			unknownsCount() const;


	/*!@name Inherited from TVParentMeasTreeNode*/
	//@{
	/*! Reacts to the disactivation of a child measurements tree node
	@param measCount the number of disactivated measurements of the child
	@param eqCount the number of disactivated equations of the child
	@param unkCount the number of disactivated unknowns of the child
	*/
//	virtual void	childDisactivated(int measCount, int eqCount, int unkCount);
	/*! Reacts to the activation of a child measurements tree node
	@param measCount the number of disactivated measurements of the child
	@param eqCount the number of disactivated equations of the child
	@param unkCount the number of disactivated unknowns of the child
	*/
//	virtual void	childActivated(int measCount, int eqCount, int unkCount);
	//@}

	/*!@name Inherited from TVChildMeasTreeNode*/
	//@{
	//! Reacts to the disactivation of the parent station
//	virtual void	parentDisactivated();
	//! Reacts to the activation of the parent station
//	virtual void	parentActivated();
	//@}

	/*!@name Inherited from TASpatialPointListener*/
	//@{
	//! Reacts to the disactivation of the reference point
//	virtual void	pointDisactivated();
	//! Reacts to the activation of the reference point
//	virtual void	pointActivated();
	//@}


	//!Disactivates the ROM
//	virtual void	disactivate();
	//!Activates the ROM
//	virtual void	activate();

	/*! Adds a listener of this ROM to its broadcast list
	@param romListnr a pointer to the object listening to this ROM*/
//	virtual void	addListener(TVRoundOfMeasListener* romListnr);

	/*!@return an iterator pointing to the reference TSpatialPoint*/
//	virtual TWorkingPoints::PointIterator	getRefPoint() const;


//	virtual void			childReadyToActivate(int, int, int);
//	virtual bool			enoughTotalEqns() const;
//	virtual void			parentEnoughTotalEqns();
//	virtual void			activateReadyChildren();


//	TWorkingPoints::PointIterator	fReferencePoint;

//	int								fActiveMeasCount;
//	int								fActiveEqCount;
//	int								fActiveUnkCount;

//	bool							fUsedInCalc;
//	bool							fParentUsedInCalc;

//	ROMListnrList					fROMListeners;

//	TVParentMeasTreeNode*			fParentStation;

	/*!Constructor
	@param parent a pointer to the parent meas tree node (instrument station)
	@param refPt an iterator pointing to the reference TSpatialPoint*/
//	TARoundOfMeas(TVParentMeasTreeNode* parent, TWorkingPoints::PointIterator refPt);

