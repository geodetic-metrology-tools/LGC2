//
// TAMeasurement.h : header file
// abstract base class holding the behaviour common to all
// measurements
//
//  Patterns:
//  
//  
//  Copyright 2003 CERN EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////


#ifndef SU_TAMEASUREMENT
#define SU_TAMEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
using namespace std;

/*! \ingroup spatialmeasurements
	@{*/

//! Abstract Base Class. Defines the methods common to all survey measurements
class TAMeasurement
{
public:

	/*!@name Enum Type Definition*/
	//@{
	enum ECalcStatus {kVariable, kFixed};
	//@}


	/*!@return the identifier of the measurement*/
	virtual int					getId() const { return fIdentifier;}
	/*!sets the identifier of the measurement
	@param id an int*/
	virtual void				setId(int id);

	/*!@return the comment of the measurement*/
	virtual string				getComment() const { return fComment;}
	/*!sets the comment of the measurement
	@param com a string*/
	virtual void				setComment(string com) { fComment = com; return;}
	
	/*!@return the head comment of the measurement*/
	virtual string				getHeaderComment() const { return fHeaderComment;}
	
	/*!sets the head comment of the measurement
	@param com the comment line as a string*/
	virtual void				setHeaderComment(string com) { fHeaderComment = com; return;}

	/*! Indicate if the id is frome Geode or not*/
	virtual bool				isGeodeIdUsed() const { return fHasAGeodeId; }


protected:

	/*!@name Constructors / Destructor*/
	//@{
	//!Default constructor
	TAMeasurement();
	/*! Copy constructor */
	TAMeasurement(const TAMeasurement& source);
	/*! Destructor */
	~TAMeasurement();
	//@}

	
	
	
	int								fIdentifier; /*< identifier from geode */
	bool							fHasAGeodeId; /*< indicating if it use a Geode identifier or not */
	string							fComment; /*< comment from geode*/
	string							fHeaderComment; /*< head comment line (starting with % in input file)*/
	

};
#endif

/*@}*/











//#include "TWorkingPoints.h"

//#include "TVChildMeasTreeNode.h"
//#include "TVParentMeasTreeNode.h"
//#include "TASpatialPtListener.h"
//#include "TVMeasurementListener.h"

/*!@name Typedefs*/
	//@{
	//!type of the container used to store the broadcast list
	//typedef list< TVMeasurementListener* >  MeasListnrList;
	//!type of an iterator pointing to an element of the container storing the measurement listeners
	//typedef MeasListnrList::iterator MeasListnrIterator;
	//@}




/*!@return a true boolean if the measurement is active*/
	//virtual bool		isActive() const;
	/*!@return the measurement's number of active measurements*/
	//virtual int			measurementsCount() const;
	/*!@return the measurement's number of active equations*/
	//virtual int			equationsCount() const;
	/*!@return the measurement's number of active unknowns*/
	//virtual int			unknownsCount() const;

	/*!@name inherited from TVParentMeasTreeNode*/
	//@{
	//!Responds to the disactivation of the parent tree node
	//virtual void		parentDisactivated();
	//!Responds to the activation of the parent tree node
	//virtual void		parentActivated();
	//@}

	//virtual void		parentEnoughTotalEqns();

	/*!@name Inherited from TASpatialPointListener*/
	//@{
	//!Responds to the disactivation of the target point
	//virtual void	pointDisactivated();
	//!Responds to the activation of the target point
	//virtual void	pointActivated();
	/*!Responds to another change of the target point
	@param um one of the possible state changes of the EUpdateMessage enumeration)*/
	//virtual void	pointUpdated(TVSpatialPtListener::EUpdateMessage um);
	//@}


//virtual bool	readyToActivate() const;

	//!Disactivates the measurement
	//virtual void	disactivate();
	//!Activates the measurement
	//virtual void	activate();
	/*! Adds a listener of this measurement to its broadcast list
	@param measListnr a pointer to the object listening to this measurement*/
	//virtual void	addListener(TVMeasurementListener* measListnr);







//virtual void					sendInitialMessages();

	//TWorkingPoints::PointIterator	fTargetPoint;

	
	//int								fActiveMeasCount;
	//int								fActiveEqCount;
	//int								fActiveUnkCount;

	//bool							fParentEnoughEqnsBeforeMe;

	//TVParentMeasTreeNode*			fParent;


//private:

	//bool							fUsedInCalc;
	//bool							fParentUsedInCalc;
	//bool							fReadyToActivate;

	//MeasListnrList					fMeasListeners;
