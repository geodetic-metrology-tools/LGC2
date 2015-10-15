
#ifndef SU_PT_LIST_BROADCASTER
#define SU_PT_LIST_BROADCASTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TVPtListListener.h"
#include "TSpatialPoint.h"

class TPtListBroadcaster : public TVPtListListener
{

public:

	TPtListBroadcaster();
	virtual ~TPtListBroadcaster();


	void headerChanged(PointIterator iter);
	void ptNameChanged(PointIterator iter);
	void positionChanged(PointIterator iter);
	void distChanged(PointIterator iter);
	void statusChanged(PointIterator iter);
	
	bool addListener(TVPtListListener* listener);
	bool deleteListener(TVPtListListener* listener);	

private:
	PtListListnrContainer fListeners;


};

#endif

