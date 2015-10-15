// TWorkingPoints.cpp: implementation of the TWorkingPoints class.
//




#include "TWorkingPoints.h"
//ClassImp(TWorkingPoints)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TWorkingPoints::TWorkingPoints():fLastPtNbr(0)
{
	fBroadcaster = new TPtListBroadcaster();
}

TWorkingPoints::~TWorkingPoints()
{
	if( fBroadcaster != 0 )
	{
		delete fBroadcaster;
	}
}

TWorkingPoints::TWorkingPoints(const TWorkingPoints & ori)
: fLastPtNbr(ori.fLastPtNbr)
, pointsMap(ori.pointsMap)
, fBroadcaster(new TPtListBroadcaster(*ori.fBroadcaster))
, fWorkingPoints(ori.fWorkingPoints)
{
	fLastPtNbr = ori.fLastPtNbr;
	fWorkingPoints = ori.fWorkingPoints;

}

/////////////////////////////////////////////////////////////
// Adding a Spatial point to the pointsSet member variable
/////////////////////////////////////////////////////////////
bool TWorkingPoints::addPoint(TSpatialPoint *sp){
	
	string empty("");
	bool insert_ok = false;

	// check that the point is not already in the list
	if (pointsMap.find(sp->getName().getName()) == pointsMap.end())
	{
		if (((sp->getName()).getName()) == empty){
			ostringstream oss;
			fLastPtNbr ++;
			oss << "SpPt" << fLastPtNbr;
			sp->setName(TSpatialPointName(oss.str()));
		}
		sp->setListener(this);
		fWorkingPoints.push_back(*sp);
		pointsMap[sp->getName().getName()] = --(fWorkingPoints.end());
			
		insert_ok = true;
	}

	return insert_ok;
}

/////////////////////////////////////////////////////////////
// Adding a Spatial point to the pointsSet member variable
/////////////////////////////////////////////////////////////
bool TWorkingPoints::insertPoint(TSpatialPoint *sp, int pos){
	
	string empty("");
	bool insert_ok = false;
	size_t origNumPoints = fWorkingPoints.size();
	
	// check that the point is not already in the list
	if (pointsMap.find(sp->getName().getName()) == pointsMap.end())
	{

		if (((sp->getName()).getName()) == empty){
			ostringstream oss;
			fLastPtNbr ++;
			oss << "SpPt" << fLastPtNbr;
			sp->setName(TSpatialPointName(oss.str()));
		}

		sp->setListener(this);
		
		PointIterator it = fWorkingPoints.begin();
		std::advance(it, pos);

		PointIterator inserted = fWorkingPoints.insert(it, *sp);
		pointsMap[sp->getName().getName()] = inserted;
			
		insert_ok = true;
	}

	/*FOR CHABA INTERFACE USE ONLY!!!*/
	//if time stamp happens to be the same if clicked in quick succesion
	else 
	{
	
		/*PUT HERE FOR NOW*/
		string name = sp->getName().getName(); 
		string s = name.substr(0,15);

		if (s == "Insert_New_Row_")
		{
			ostringstream oss;
			fLastPtNbr++;
			oss << name << fLastPtNbr;	
			sp->setDuplicatePtName(oss.str());
			sp->setListener(this);
		
			PointIterator iter = fWorkingPoints.insert(fWorkingPoints.end(), *sp);
			pointsMap[sp->getName().getName()] = iter;
				
			insert_ok = true;
		}


	}

	insert_ok = (fWorkingPoints.size() == (origNumPoints + 1));
	return insert_ok;
}


bool TWorkingPoints::deletePoint(PointIterator iter)
{
	std::string pName = iter->getName().getName();
	std::unordered_map<string, PointIterator>::iterator i = pointsMap.find(pName);
	if (i == pointsMap.end())
	{
		return false;
	}
	fWorkingPoints.erase(iter);
	pointsMap.erase(i);
	return true;
}

/////////////////////////////////////////////////////////////////
// Returns the number of Spatial Points stored in the pointsSet
/////////////////////////////////////////////////////////////////

int TWorkingPoints::numberOfPoints() const{

	return fWorkingPoints.size();
}


/////////////////////////////////////////////////////////////////
// Returns an iterator on the first element of the fWorkingPoints
/////////////////////////////////////////////////////////////////
PointIterator TWorkingPoints::getPointsBeginIterator() {

	return fWorkingPoints.begin();
}

//////////////////////////////////////////////////////////////////////
// Returns a const iterator on the first element of the fWorkingPoints
//////////////////////////////////////////////////////////////////////
PointConstIter TWorkingPoints::getPointsBeginIterator() const{

	return fWorkingPoints.begin();
}

////////////////////////////////////////////////////////////////////
// returns an iterator on one element past the end of fWorkingPoints
////////////////////////////////////////////////////////////////////
PointIterator TWorkingPoints::getPointsEndIterator() {

	return fWorkingPoints.end();
}

/////////////////////////////////////////////////////////////////////////
// returns a const iterator on one element past the end of fWorkingPoints
/////////////////////////////////////////////////////////////////////////
PointConstIter TWorkingPoints::getPointsEndIterator() const{

	return fWorkingPoints.end();
}

///////////////////////////////////////////////////////////////////
// returns an iterator on a point, given its TSpatialPointName
///////////////////////////////////////////////////////////////////
PointIterator TWorkingPoints::getPoint(string spn) {

	std::unordered_map<string, PointIterator>::iterator i = pointsMap.find(spn);
	if (i == pointsMap.end())
	{
		return fWorkingPoints.end();
	}
	return i->second;
}

PointIterator TWorkingPoints::getPoint(TSpatialPointName spn) {
	return getPoint(spn.getName());
}

///////////////////////////////////////////////////////////////////
// returns a const iterator on a point, given its TSpatialPointName
///////////////////////////////////////////////////////////////////
PointConstIter TWorkingPoints::getPoint(string spn) const{

	std::unordered_map<string, PointIterator>::const_iterator i = pointsMap.find(spn);
	if (i == pointsMap.end())
	{
		return fWorkingPoints.end();
	}
	return i->second;
}

PointConstIter TWorkingPoints::getPoint(TSpatialPointName spn) const {
	return getPoint(spn.getName());
}
///////////////////////////////////////////////////////////////////////////
PointIterator TWorkingPoints::getPoint(int pos)
{
	PointIterator it = fWorkingPoints.begin();
	std::advance(it, pos);
	return it;
}

PointConstIter TWorkingPoints::getPoint(int pos) const
{
	PointConstIter it = fWorkingPoints.begin();
	std::advance(it, pos);
	return it;
}


bool TWorkingPoints::renamePoint(const std::string & oldName, const std::string & newName)
{
	PointIterator p = getPoint(oldName);
	if(p==getPointsEndIterator())
		return false; // No such point
	if(getPoint(newName)!=getPointsEndIterator())
		return false; // Cannot rename - would create a duplicate
	std::unordered_map<string, PointIterator>::iterator iter = pointsMap.find(oldName);
	if(iter==pointsMap.end())
		return false; // Something wrong with this TWorkingPoints

	// Rename the point:
	p->setPtName(newName);

	// Update the mapping:
	pointsMap.erase(iter);
	pointsMap[newName] = p;
	return true;
}

//////////////////////////////////////////////////////////////////////////

void TWorkingPoints::headerChanged(TSpatialPointName name)
{
	PointIterator iter = getPoint(name);
	fBroadcaster->headerChanged(iter);

}


void TWorkingPoints::ptNameChanged(TSpatialPointName name)
{
	PointIterator iter = getPoint(name);
	fBroadcaster->ptNameChanged(iter);
}

void TWorkingPoints::positionChanged(TSpatialPointName name)
{
	PointIterator iter = getPoint(name);
	fBroadcaster->positionChanged(iter);
}
	
void TWorkingPoints::distChanged(TSpatialPointName name)
{
	PointIterator iter = getPoint(name);
	fBroadcaster->distChanged(iter);
}
	
void TWorkingPoints::statusChanged(TSpatialPointName name)
{
	PointIterator iter = getPoint(name);
	fBroadcaster->statusChanged(iter);
}

void TWorkingPoints::addListener(TVPtListListener* listener)
{

	fBroadcaster->addListener(listener);
}

void TWorkingPoints::deleteListener(TVPtListListener* listener)
{

	fBroadcaster->deleteListener(listener);
}


