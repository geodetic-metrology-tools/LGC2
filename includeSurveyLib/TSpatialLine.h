// TSpatialLine.h : header file
/*!	  
	Class modelising a line	

	Copyright 2003, CERN, EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////


#ifndef SU_SPATIALLINE
#define SU_SPATIALLINE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////
//forward declarations and includes

#include <list>
#include <sstream>
//using namespace std;

#include "TAngle.h"
#include "TSpatialPointName.h"
/////////////////////////////////////////////////////////////

/*!\ingroup SpatialObjectsAndSystems
	@{*/


class TSpatialLine //: public
{

public:



	/*!@name Constructors / Destructor*/
	//@{
		/*!Default Constructors */
		TSpatialLine();
		/*!Constructors of a vertical line
		\param firstEndName the name of the line's first end point*/
		TSpatialLine(const TSpatialPointName firstEndName);
		/*!Constructors line
		\param firstEndName the name of the line's first end point
		<param angle the line's orientation*/
		TSpatialLine(const TSpatialPointName firstEndName, const TSpatialPointName secondEndName);
		//!Copy constructor
		TSpatialLine(const TSpatialLine& source);
		//!Destructor
		~TSpatialLine();
	//@}

	/*!@member function*/
	//@{
		//!Equality operator
		virtual bool				operator==(const TSpatialLine& right) const;
		//!operator =
		virtual void				operator=(const TSpatialLine& right);

		/*!\return true if the line is define with the new arguments*/
		virtual bool					ifNotDoneDefinePlane(const TSpatialPointName firstPointName);
		/*!\return true if the line is define with the new arguments*/
		virtual bool					ifNotDoneDefinePlane(const TSpatialPointName firstPointName, const TSpatialPointName secondEndName);

		/*!\return the name of the line's first end point*/
		virtual	TSpatialPointName	getFirstEndName() const;
		/*!\rreturn the name of the line's second end point*/
		virtual	TSpatialPointName	getSecondEndName() const;
		/*!\return the line's name*/
		virtual std::string				getName() const;

		/*!build the name (used only for TWorkingLines
		if the line which is added to the list is not defined)*/
		virtual void					buildErrorName();
	//@}


private:

	/*!@name Member function*/
	//@{
	/*!\return true if the line is define*/
		bool								isDefine() const;
	//@}


	TSpatialPointName			fFirstEndName;
	TSpatialPointName			fSecondEndName;
	std::string					fName;

};
/*@}*/


/*!@name Typedefs*/
//@{
/*! Type of the container used to store points */
typedef std::list<TSpatialLine> SpatialLineContainer;
/*! Type of an iterator pointing to an element of the container */
typedef SpatialLineContainer::iterator LineIterator;
/*! Type of a const iterator pointing to an element of the container */
typedef SpatialLineContainer::const_iterator LineConstIter;
//@}

#endif






/////////////////////////////////////////////////////////////////
//stock	

/*
	attributs
	virtual void							updateNeededEquations();
	PointConstIter							fFirstEnd;
	PointConstIter							fSecondEnd;
	bool									fUsedInCalc;
	bool									fEndsActive;
	int										fEqCount;
	int										fNeededEquations;
	LineListnrContainer						fLineListeners;
*/
	//!Less than operator
/*	virtual bool							operator<(const TSpatialLine& right) const;
	!@name Typedefs
	//@{
	//! Type of the container used for the broadcast list
	typedef list< TVMeasurableEntityLstnr* > LineListnrContainer;
	//! Type of an iterator pointing to an element of the broadcast list
	typedef LineListnrContainer::iterator LineListnrIterator;
	//! Type of a const iterator pointing to an element of the broadcast list
	typedef LineListnrContainer::iterator LineListnrConstIter;
	//@}

	//!Sets the end points using their names
	!@param wp a pointer to the TWorkingPoints object
	virtual bool							setEndPoints(TWorkingPoints* wp);

	!@name Active / Inactive related
	//@{
	//!Activates the line
	virtual void							activate();
	//!Disactivates the line
	virtual void							disactivate();
	//!Tells all the conerned objects that the line is activated
	virtual void							sendActivatedToAll();
	//!Tells all the conerned objects that the line is disactivated
	virtual void							sendDisactivatedToAll();
	!@return a true boolean if the line is active
	virtual bool							isActive() const;
	//@}


	//!Update thw line's equation count
	!@param eqCountUpdt the update to the equation count
	virtual void							updateEqCount(int eqCountUpdt);
	//!Adds a listener to the broadcast list
	! mel a pointer to the listener to be added. The pointer will be copied into the broadcast list
	virtual void							addListener(TVMeasurableEntityLstnr*  mel);

	//inherited from TVSpatialPtListener
	//!Reacts to the activation of one of the end points
	virtual void							pointActivated();
	//!Reacts to the disactivation of one of the end points
	virtual void							pointDisactivated();
	//!Reacts to the change of state of one of the end points (other than active/inactive)
	!@param um an element of the point update messages enumeration
	virtual void							pointUpdated(TVSpatialPtListener::EUpdateMessage um);
*/
