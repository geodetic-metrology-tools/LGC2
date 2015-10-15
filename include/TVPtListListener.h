
#ifndef SU_PT_LIST_LISTENER
#define SU_PT_LIST_LISTENER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "TSpatialPoint.h"
#include "TPointFormat.h"
//#include "TWorkingPoints.h"

//Class definition
class  TVPtListListener
{

	
public:
	virtual void headerChanged(PointIterator iter) = 0;
	virtual void ptNameChanged(PointIterator iter)= 0;
	virtual void positionChanged(PointIterator iter)= 0;
	virtual void distChanged(PointIterator iter)= 0;
	virtual void statusChanged(PointIterator iter)= 0;

	virtual ~TVPtListListener() { };
	
};
	
	//! Type of the container used to store the broadcast list
typedef list<TVPtListListener*> PtListListnrContainer;
//! Type of an iterator pointing to an element of the broadcast list
typedef PtListListnrContainer::iterator PtListListnrIterator;
//! Type of a const iterator pointing to an element of the broadcast list
typedef PtListListnrContainer::const_iterator PtListListnrConstIter;
		
#endif
