// TAObjectMeasurement.h
/*!
	Abstract base class for all measurements made to something other than a point

	Patterns:

 
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TAObjectMeasurement
#define SU_TAObjectMeasurement


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <list>
using namespace std;

#include "TSpatialPointName.h"
//
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Abstract base class for object measurements 
class  TAObjectMeasurement //:public TVChildMeasTreeNode, public TVMeasurableEntityLstnr //: public TObject  
{
public:
		/*!@name Member Functions */
		//@{
		/*! Copy Assignment Operator */
		TAObjectMeasurement& operator=( const TAObjectMeasurement& );
		/* Return to spatial point's name */
//		virtual TSpatialPointName	getTargetPoint() const;
		/* Set the point's name */
//		virtual bool				setTargetPoint(TSpatialPointName spn);
		//@}

protected:

		/*!@name Constructors and Destructors */
		//@{
		/*!Default constructor */
		TAObjectMeasurement();
		/*! Constructor */
//		TAObjectMeasurement(TSpatialPointName targetPt);
		/*! Copy Constructor */ 
		TAObjectMeasurement(const  TAObjectMeasurement&);
		/*! Destructor */
		virtual  ~TAObjectMeasurement();
		//@}

private:

//		TSpatialPointName		fTargetPtName; /*< target point's name */


};


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


#endif // SU_TAObjectMeasurement

/*@}*/

//class  TVMeasurementListener;
//class TVParentMeasTreeNode;
//#include  "TVChildMeasTreeNode.h"
//#include "TVMeasurableEntityLstnr.h"

	/*! Typedefs*/
	//@{
	//! Type of the container used to store the broadcast list
//	typedef list< TVMeasurementListener* >  MeasListnrList;
	//! Type of an iterator pointing to an element of the broadcast list
//	typedef MeasListnrList::iterator MeasListnrIterator;
	//@}

		/*!@return a boolean telling if the node is active or not*/
//		virtual bool		isActive() const;
		/*!@return the node's number of active measurements*/
//		virtual int			measurementsCount() const;
		/*!@return the node's number of active equations*/
//		virtual int			equationsCount() const;
		/*!@return the node's number of active unknowns*/
//		virtual int			unknownsCount() const;

//		virtual void		parentEnoughTotalEqns();
		//! Responds to the disactivation of the parent node
//		virtual void		parentDisactivated();
		//! Responds to the activation of the parent node
//		virtual void		parentActivated();

		//! Tells the parent node that the measurement is disactivated
//		virtual void		sendDisactivatedToParent() const;
		//! Tells the parent node that the measurement is activated
//		virtual void		sendActivatedToParent() const;

		//! Reacts to the disactivation of the measured entity
//		virtual void		measEntityDisactivated();
		//! Reacts to the activation of the measured entity
//		virtual void		measEntityActivated();
//		virtual void		measEntityUpdated(TVMeasurableEntityLstnr::EUpdateMessage);

		//! Disactivates the measurement
//		virtual void	disactivate();
		//! Activates the measurement
//		virtual void	activate();
		//! Adds a listener to the broadcast list
//		virtual void	addListener(TVMeasurementListener* measListnr);

//protected:
		//! Constructor 
//		TAObjectMeasurement(TVParentMeasTreeNode* parent);

//	int								fActiveMeasCount;
//	int								fActiveEqCount;
//	int								fActiveUnkCount;


//private:

//	bool							fUsedInCalc;
//	bool							fParentUsedInCalc;

//	MeasListnrList					fMeasListeners;

//	TVParentMeasTreeNode*			fParent;
	//ClassDef(TAObjectMeasurement, 1)

