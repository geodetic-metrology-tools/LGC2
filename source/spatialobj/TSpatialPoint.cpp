// 
// TSpatialPoint
//
// Class for a spatial point as defined for survey purposes
//
// Copyright 1999,2000, CERN, EST/SU. All rights reserved.
//
////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
// other forward declarations
//#include	"TGeodeticData.h"
#include  "TANumericValue.h"
#include  "TSpatialPosition.h"
#include  "TSpatialPoint.h"
//#include "TWorkingPoints.h"
//#include "TRoundOfMeasNetworkListnr.h"
////////////////////////////////////////////////////////////////




//ClassImp(TSpatialPoint)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TSpatialPoint::TSpatialPoint() : fPtListeners(),fPointDist(),fHeaderComment(), fEOLComment()
{	//default constructor

	fName = new TSpatialPointName();
	fPosition = 0;
	fUsedInCalc = true;
	fEqCount = 0;
	flistener = 0;
	fPointStatus = TSpatialPoint::kActive;

	fNameSet = 0;
	fXSet = 0;
	fYSet = 0;
	fZSet = 0;

}

TSpatialPoint::TSpatialPoint( const string& ptName ) : fPtListeners(),fPointDist(), fHeaderComment(), fEOLComment()
{
	fName = new TSpatialPointName( ptName );
	fPosition = 0;
	fUsedInCalc = true;
	fEqCount = 0;
	flistener = 0;
	fPointStatus = TSpatialPoint::kActive;

	fNameSet = 0;
	fXSet = 0;
	fYSet = 0;
	fZSet = 0;
}


TSpatialPoint::TSpatialPoint( const TSpatialPointName&  name, TSpatialPosition*  position ) 
: fPtListeners(),fPointDist(), fHeaderComment()
{	
	fName = new TSpatialPointName( name );
	fPosition = position;
	fUsedInCalc = true;
	fEqCount = 0;
	flistener = 0;
	fPointStatus = TSpatialPoint::kActive;
//	updateNeededEquations();

	fNameSet = 0;
	fXSet = 0;
	fYSet = 0;
	fZSet = 0;


}


TSpatialPoint::TSpatialPoint(const TSpatialPoint& tsp) 
: fPtListeners(/*tsp.fPtListeners*/),
  fPosition(new TSpatialPosition(*(tsp.fPosition)))
{
	fName = new TSpatialPointName( *(tsp.fName) );

	fPointDist = tsp.fPointDist;
	fHeaderComment = tsp.fHeaderComment;
	fEOLComment = tsp.fEOLComment;
	fUsedInCalc = tsp.fUsedInCalc;
	fEqCount = tsp.fEqCount;
	flistener = NULL;/*tsp.flistener;*/
	fPointStatus = tsp.fPointStatus;
//	updateNeededEquations();
	fNameSet = tsp.fNameSet;
	fXSet = tsp.fXSet;
	fYSet = tsp.fYSet;
	fZSet = tsp.fZSet;
}


/*TSpatialPoint::TSpatialPoint(const string& name, const char point, const string& classifier)
	: fName( name, point, classifier )
{
	fPosition = 0;
	fUsedInCalc = true;

}*/


/*TSpatialPoint::TSpatialPoint(const string& name, const char point, const string& classifier, TSpatialPosition* sp) 
	: fName( name, point, classifier )
{
	fPosition = sp;
	fUsedInCalc = true;

}*/


TSpatialPoint::~TSpatialPoint()
{
	delete fName;

	if( 0 != fPosition )
		delete fPosition;
}


//////////////////////////////////////////////////////////////////////
// copy assignement operator
//////////////////////////////////////////////////////////////////////
TSpatialPoint&  TSpatialPoint::operator=(const TSpatialPoint& tsp)
{
	TSpatialPoint copy(tsp);
	swap(copy);
	return *this;
}

void TSpatialPoint::swap(TSpatialPoint & other) throw() {
	using std::swap;
	swap(fUsedInCalc, other.fUsedInCalc);
	swap(fName, other.fName);
	swap(fPointDist, other.fPointDist);
	swap(fHeaderComment, other.fHeaderComment);
	swap(fEOLComment, other.fEOLComment);
	swap(fPosition, other.fPosition);
	swap(fEqCount, other.fEqCount);
	swap(fNeededEquations, other.fNeededEquations);
	swap(fPtListeners, other.fPtListeners);
	swap(flistener, other.flistener);
	swap(fPointStatus, other.fPointStatus);
	swap(fNameSet, other.fNameSet);
	swap(fXSet, other.fXSet);
	swap(fYSet, other.fYSet);
	swap(fZSet, other.fZSet);
}



//////////////////////////////////////////////
// Overloaded == operator definition
//////////////////////////////////////////////

bool TSpatialPoint::operator==(const TSpatialPoint &right) const {

	if (*fName==right.getName())
		return true;
	else
		return false;
}

////////////////////////////////////
// Adds a point listener to the list
////////////////////////////////////
void TSpatialPoint::addListener(TVSpatialPtListener* spl){

	flistener = spl;
}


//////////////////////////////////////////////
// Overloaded < operator definition
//////////////////////////////////////////////
bool TSpatialPoint::operator<(const TSpatialPoint &right) const {

	if (*fName<right.getName())
		return true;
	else return false;
}


void  TSpatialPoint::setPosition( TSpatialPosition *position )
{	// Set the point's position

	if( fPosition != 0 ) 
	{	// delete the existing position
		delete fPosition;
	}

	fPosition = position;  
/*
	PtListnrIterator iter = fPtListeners.begin();
	PtListnrIterator iterEnd = fPtListeners.end();

	while (iter != iterEnd){
		(*iter)->pointUpdated(TVSpatialPtListener::EUpdateMessage::kPositionChange);
		iter++;
	}
*/
//	updateNeededEquations();

	if (flistener != 0)
	{
		flistener->positionChanged(*fName);
	} 

	return;
}


void  TSpatialPoint::setPosition( TSpatialPosition *position, int column, bool isNull )
{	// Set the point's position

	if( fPosition != 0 ) 
	{	// delete the existing position
		delete fPosition;
	}

	fPosition = position;  
/*
	PtListnrIterator iter = fPtListeners.begin();
	PtListnrIterator iterEnd = fPtListeners.end();

	while (iter != iterEnd){
		(*iter)->pointUpdated(TVSpatialPtListener::EUpdateMessage::kPositionChange);
		iter++;
	}
*/
//	updateNeededEquations();

	if (fPointStatus == kNew && isNull == false)
	{
		switch (column)
		{
		case 2:
			fXSet = 1;
			break;
		case 3: 
			fYSet = 1;
			break;
		case 4: 
			fZSet = 1;
			break;	
		}
	
		isPointNew();
	}


	if (flistener != 0)
	{
		flistener->positionChanged(*fName);
	} 
	
	return;
}


void TSpatialPoint::isPointNew()
{
	if (fPointStatus == kNew && fNameSet == 1 && fXSet == 1 && fYSet == 1 && fZSet == 1)
	{
		setStatus(kActive);
	}

}


TSpatialPosition  TSpatialPoint::getPosition( TAReferenceFrame*  frame ) const
{	// Get the point's position
	if (fPosition->getRefFrame() != frame)
	{
		(*fPosition).transform(frame);
	}

	return *fPosition; 
}

TSpatialPosition&  TSpatialPoint::getPosition() const
{
/////////////////////////////////////////////////
// returns a reference to the point's raw position
/////////////////////////////////////////////////

	return *fPosition;
}

/////////////////////////////////////////////
// returns the number of variable coordinates
/////////////////////////////////////////////
int TSpatialPoint::getVariableDimension() const{

	return fPosition->getVariableDimension();
}


//////////////////////////////////////////////
//////////////////////////////////////////////
void TSpatialPoint::setName( const TSpatialPointName& name ){
///////////////////////////////////////////////////////
// sets the point's name 
///////////////////////////////////////////////////////

	*fName = name;
/*
	PtListnrIterator iter = fPtListeners.begin();
	PtListnrIterator iterEnd = fPtListeners.end();

	while (iter != iterEnd){
		(*iter)->pointUpdated(TVSpatialPtListener::EUpdateMessage::kNameChange);
		iter++;
	}*/
	//put in new imp here
	if (flistener != 0)
	{
		flistener->ptNameChanged(*fName);
	} 

}


TSpatialPointName  TSpatialPoint::getName()  const
{	// Get the point's name
	return *fName;
}


void TSpatialPoint::setPtName( const string& ptName ) 
{// Set the point's point name
	fName->setName( ptName);

	if (fPointStatus == kNew && ptName !="")
		fNameSet = 1;
	
	if (flistener != 0)
		flistener->ptNameChanged(*fName);
	 
	isPointNew();
}

/*METHOD FOR CHABA INTERFACE ONLY - CALLED IF TWO POINTS HAVE IDENTICAL TIMESTAMPS*/
void TSpatialPoint::setDuplicatePtName( const string& ptName ) 
{// Set the point's point name
	fName->setName( ptName);
}

string TSpatialPoint::getPtName()  const
{// Get the point's point name
	return fName->getName();
}


void  TSpatialPoint::setHeaderComment( const string& com) 
{

fHeaderComment = com; 
if (flistener != 0)
		{
			flistener->headerChanged(*fName);
		} 

}

void  TSpatialPoint::setDist( const TReal& id) 
{
	fPointDist = id; 
	if (flistener != 0)
		{
			flistener->distChanged(*fName);
		} 
}


void TSpatialPoint::setStatus(const TSpatialPoint::EPointStatus status)
{
	fPointStatus = status;
	if (flistener != 0)
		{
			flistener->statusChanged(*fName);
		} 
}





//not used in chaba interface - enum used instead
bool TSpatialPoint::isActive() const
{// tells the point's active/inactive status
	return fUsedInCalc;
}

//not used in chaba interface - enum used instead
void TSpatialPoint::disactivate()
{// disactivates the point
	//no reaction if !fUsedInCalc

	if (fUsedInCalc){
		fUsedInCalc = false;
		fEqCount = 0;
		sendDisactivatedToAll();
	}
}


void TSpatialPoint::sendDisactivatedToAll()
{// sends a disactivated message to every point listener
/*
	PtListnrIterator iter = fPtListeners.begin();
	PtListnrIterator iterEnd = fPtListeners.end();

	while (iter != iterEnd){
		(*iter)->pointDisactivated();
		iter++;
	}*/
}


void TSpatialPoint::activate()
{// reactivates the point

	//no reaction if fUsedInCalc
	if (!fUsedInCalc)
		sendActivatedToAll();
}


void TSpatialPoint::sendActivatedToAll()
{// sends a Activated message to every point listener
/*
	PtListnrIterator iter = fPtListeners.begin();
	PtListnrIterator iterEnd = fPtListeners.end();

	while (iter != iterEnd){
		(*iter)->pointActivated();
		iter++;
	}*/

}

int TSpatialPoint::equationsCount() const
{// returns the point's equations count
	return fEqCount;
}

void TSpatialPoint::updateEqCount(int eqCountUpdate)
{// updates the point's equation count

	if (eqCountUpdate >0){

		if (fUsedInCalc){
			fEqCount += eqCountUpdate;/*
			PtListnrIterator iter = fPtListeners.begin();
			PtListnrIterator iterEnd = fPtListeners.end();
			while (iter != iterEnd){
				(*iter)->pointUpdated(TVSpatialPtListener::EUpdateMessage::kEqCountChange);
				iter++;
			}*/
		}


		if (!fUsedInCalc){
			if ((fEqCount + eqCountUpdate) >= fNeededEquations){
				fEqCount += eqCountUpdate;
				fUsedInCalc = true;
/*
				PtListnrIterator iter = fPtListeners.begin();
				PtListnrIterator iterEnd = fPtListeners.end();
				while (iter != iterEnd){
					(*iter)->pointActivated();
					(*iter)->pointUpdated(TVSpatialPtListener::EUpdateMessage::kEqCountChange);
					iter++;
				}*/
			}
			else{/*
				PtListnrIterator iter = fPtListeners.begin();
				PtListnrIterator iterEnd = fPtListeners.end();
				while (iter != iterEnd){
					(*iter)->pointDisactivated();
					//the only object it will affect is the sender of the updateEqCount message
					iter++;
				}*/
			}
		}
	}

	if (eqCountUpdate <0){

		//no reaction if !fUsedInCalc, shouldn't occur

		if (fUsedInCalc){

			fEqCount += eqCountUpdate;/*
			PtListnrIterator iter = fPtListeners.begin();
			PtListnrIterator iterEnd = fPtListeners.end();
			while (iter != iterEnd){
				(*iter)->pointUpdated(TVSpatialPtListener::EUpdateMessage::kEqCountChange);
				iter++;
			}*/

			if (fEqCount < fNeededEquations){
				fUsedInCalc = false;/*
				iter = fPtListeners.begin();
				while (iter != iterEnd){
					(*iter)->pointDisactivated();
					iter++;
				}*/
			}
		}
	}
}


void TSpatialPoint::updateNeededEquations()
{// updates the number of needed equations to deterine the point

	int oldNdEq = fNeededEquations;
	fNeededEquations = 0;

	TSpatialStatus::ESpatialStatus positionStatus(fPosition->getObjectStatus());

	if ( positionStatus == TSpatialStatus::kVxyz)
		fNeededEquations += 3;
	
	if (positionStatus == TSpatialStatus::kVxy
		|| positionStatus == TSpatialStatus::kVxz
		|| positionStatus == TSpatialStatus::kVyz)
		fNeededEquations += 2;

	if (positionStatus == TSpatialStatus::kVx
		|| positionStatus == TSpatialStatus::kVy
		|| positionStatus == TSpatialStatus::kVz)
		fNeededEquations++;
/*
	PtListnrIterator iter = fPtListeners.begin();
	PtListnrIterator iterEnd = fPtListeners.end();
	while (iter != iterEnd){
		(*iter)->pointUpdated(TVSpatialPtListener::EUpdateMessage::kNeededEqChange);
		iter++;
	}*/

	//maybe the point has to be activated or disactivated
	int ndEqUpdt = oldNdEq - fNeededEquations;

	if ((!fUsedInCalc) && (ndEqUpdt>0)){
		if ((fEqCount>fNeededEquations) && (fEqCount<oldNdEq))
			activate();
	}

	if ((fUsedInCalc) && (ndEqUpdt<0)){
		if (fEqCount<fNeededEquations)
			disactivate();
	}
}


void TSpatialPoint::setXSet(bool state)
{

	fXSet = state;
	return;
}

void TSpatialPoint::setYSet(bool state)
{

	fYSet = state;
	return;
}

void TSpatialPoint::setZSet(bool state)
{

	fZSet = state;
	return;
}

void TSpatialPoint::setNameSet(bool state)
{

	fNameSet = state;
	return;
}


	

//////////////////////////////////////////////////////////////////////////////////////////////
//END
///////////////////////////////////////////////////////////////////////////////////////////////
