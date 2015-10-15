#include "TPtListBroadcaster.h"

TPtListBroadcaster::TPtListBroadcaster() 
{

}

TPtListBroadcaster::~TPtListBroadcaster()
{
 
}


void TPtListBroadcaster::headerChanged(PointIterator iter)
{
	PtListListnrIterator currIter = fListeners.begin();
	PtListListnrIterator endIter = fListeners.end();

	while (currIter != endIter)
	{
	
		(*currIter)->headerChanged(iter);

		currIter++;
	}

}

void TPtListBroadcaster::ptNameChanged(PointIterator iter)
{	
	PtListListnrIterator currIter = fListeners.begin();
	PtListListnrIterator endIter = fListeners.end();

	while (currIter != endIter)
	{
	
		(*currIter)->ptNameChanged(iter);

		currIter++;
	}

}

void TPtListBroadcaster::positionChanged(PointIterator iter)
{
	PtListListnrIterator currIter = fListeners.begin();
	PtListListnrIterator endIter = fListeners.end();

	while (currIter != endIter)
	{
	
		(*currIter)->positionChanged(iter);

		currIter++;
	}


}



void TPtListBroadcaster::distChanged(PointIterator iter)
{
	PtListListnrIterator currIter = fListeners.begin();
	PtListListnrIterator endIter = fListeners.end();

	while (currIter != endIter)
	{
	
		(*currIter)->distChanged(iter);

		currIter++;
	}
}

void TPtListBroadcaster::statusChanged(PointIterator iter)
{
	PtListListnrIterator currIter = fListeners.begin();
	PtListListnrIterator endIter = fListeners.end();

	while (currIter != endIter)
	{
	
		(*currIter)->statusChanged(iter);

		currIter++;
	}
}


bool TPtListBroadcaster::addListener(TVPtListListener* listener)
{
	bool ok = true;
	fListeners.push_back(listener);
	return ok;
}

bool TPtListBroadcaster::deleteListener(TVPtListListener* listener)
{
	bool ok = true;
	fListeners.remove(listener);
	return ok;
}
