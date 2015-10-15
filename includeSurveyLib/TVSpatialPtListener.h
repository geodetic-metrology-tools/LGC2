//
// TVSpatialPtListener.h : header file
/////////////////////////////////////////////////////////////////////////

#ifndef SU_TVSPPTLISTENER
#define SU_TVSPPTLISTENER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include "TSpatialPointName.h"
using namespace std;

//! Pure abstract base class defininig an interface implemented by every object listening to a spatial point
class TVSpatialPtListener{

public:

	virtual			~TVSpatialPtListener() = 0;

	//! Enumeration of the update messages (other than active/inactive) the listener can receive
	enum			EUpdateMessage{kNameChange, kPositionChange, kStatusChange,
		kNeededEqChange, kEqCountChange, kReadyToActivate};	
/*
	//! Reacts to the disactivation of the listened to point
	virtual void	pointDisactivated() =0;
	//! Reacts to the activation of the listened to point
	virtual void	pointActivated() =0;
	//! Reacts to an update of the listened to point
	virtual void	pointUpdated(EUpdateMessage) =0;*/

	/*!@initiates broadcaster if the header of a spatial point is changed*/
	virtual void headerChanged(TSpatialPointName name) = 0;
	/*!@initiates broadcaster if the name of a spatial point is changed*/
	virtual	void ptNameChanged(TSpatialPointName name) = 0;
	/*!@initiates broadcaster if the position of a spatial point is changed*/
	virtual void positionChanged(TSpatialPointName name) = 0;
	/*!@initiates broadcaster if the cumulative distance of a spatial point is changed*/
	virtual void distChanged(TSpatialPointName name) = 0;
	/*!@initiates broadcaster if the status of a spatial point is changed*/
	virtual void statusChanged(TSpatialPointName name) = 0;

};

/*!@name Typedefs*/
//@{
//! Type of the container used to store the broadcast list
typedef list<TVSpatialPtListener*> PtListnrContainer;
//! Type of an iterator pointing to an element of the broadcast list
typedef PtListnrContainer::iterator PtListnrIterator;
//! Type of a const iterator pointing to an element of the broadcast list
typedef PtListnrContainer::const_iterator PtListnrConstIter;
//@}

#endif
