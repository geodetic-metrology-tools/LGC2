//
// TSpatialLine.cpp : implementation file
// class modelising a straight spatial line

#include "TSpatialLine.h"


////////////////////////////////////////////////////////////////////////////////
//constructor / destructor
////////////////////////////////////////////////////////////////////////////////
TSpatialLine::TSpatialLine()
{//default constructor
}


TSpatialLine::TSpatialLine(const TSpatialPointName firstEndName)
{//constructor vertical line
	std::string nullName ="";
	TSpatialPointName secondEndName (nullName);

	fFirstEndName = firstEndName;
	fSecondEndName = secondEndName;

	fName = firstEndName.getName();
}


TSpatialLine::TSpatialLine(const TSpatialPointName firstEndName, const TSpatialPointName secondEndName)
{//constructor spatial line

	fFirstEndName = firstEndName;
	fSecondEndName = secondEndName;

	fName = firstEndName.getName() + fSecondEndName.getName();
}


TSpatialLine::TSpatialLine(const TSpatialLine& source)
{//copy constructor
	(*this)= source;
}


TSpatialLine::~TSpatialLine()
{//detructor
}


///////////////////////////////////////////////////////////////////////////////
//member functions
///////////////////////////////////////////////////////////////////////////////
bool TSpatialLine::operator==(const TSpatialLine& right) const
{

	if (fName == right.getName())
	{
		return true;
	}
	else
	{
		return false;
	}
}


void TSpatialLine::operator=(const TSpatialLine& right)
{
	if (fName != right.getName())
	{
		fFirstEndName = right.fFirstEndName;
		fSecondEndName = right.fSecondEndName;
		fName = right.fName;
	}

	return;
}

		
bool	TSpatialLine::ifNotDoneDefinePlane(const TSpatialPointName firstEndName)
{/*!\return true if the line is define with the new arguments*/
	bool retVal = false;

	if(!(isDefine()))
	{
		std::string nullName ="";
		TSpatialPointName secondEndName (nullName);

		fFirstEndName = firstEndName;
		fSecondEndName = secondEndName;

		fName = firstEndName.getName();

		retVal = true;
	}
	return retVal;
}


bool	TSpatialLine::ifNotDoneDefinePlane(const TSpatialPointName firstEndName, const TSpatialPointName secondEndName)
{/*!\return true if the line is define with the new arguments*/
	bool retVal = false;

	if(!(isDefine()))
	{
		fFirstEndName = firstEndName;
		fSecondEndName = secondEndName;

		fName = firstEndName.getName() + fSecondEndName.getName();

		retVal = true;
	}
	return retVal;
}

TSpatialPointName	TSpatialLine::getFirstEndName() const
{//returns a name of the first end point of the line
	return fFirstEndName;
}


TSpatialPointName	TSpatialLine::getSecondEndName() const
{//returns a name of the second end point of the line
	return fSecondEndName;
}


std::string TSpatialLine::getName() const
{//gets the line's name
	return fName;
}



		
void	TSpatialLine::buildErrorName()
{/*!build the name (used only for TWorkingLines
	if the line which is added to the list is not defined)*/
	if(!(isDefine()))
	{
		fName = "LineNotDefine";
	}
}


///////////////////////////////////////////////////////////////////////
//Private function
///////////////////////////////////////////////////////////////////////
bool	TSpatialLine::isDefine() const
{//return true if the line is define
	bool retVal = false;

	if(fName != "")
	{
		retVal = true;
	}

	return retVal;
}




	
///////////////////////////////////////////////////////////////////////
//stock
/*
/////////////////////////////
// sets the line's end points
/////////////////////////////
bool TSpatialLine::setEndPoints(TWorkingPoints* wp){

	fFirstEnd = wp->getPoint(fFirstEndName);
	fSecondEnd = wp->getPoint(fSecondEndName);

	PointIterator end = wp->getPointsEndIterator();

	if ((fFirstEnd != end) && (fSecondEnd != end))
		return true;
	else
		return false;
}
////////////////////////
// < operator definition
////////////////////////
bool TSpatialLine::operator<(const TSpatialLine& right) const{

	if (fName < right.getName())
		return true;
	else{
		if (fName == right.getName()){
			if ((*fFirstEnd) < *(right.getFirstEnd()))
				return true;
			else{
				if ((*fSecondEnd) < *(right.getSecondEnd()))
					return true;
				else
					return false;
			}
		}
		else
			return false;
	}
}
/////////////////////////////////////
// returns the active/inactive status
/////////////////////////////////////
bool TSpatialLine::isActive() const{

	return fUsedInCalc;
}

/////////////////////
// activates the line
/////////////////////
void TSpatialLine::activate(){

	//no reaction if fUsedInCalc

	if (!fUsedInCalc)
		sendActivatedToAll();
}

////////////////////////
// disactivates the line
////////////////////////
void TSpatialLine::disactivate(){

	//no reaction if !fUsedInCalc
	if (fUsedInCalc){
		fUsedInCalc = false;
		fEqCount = 0;
		sendDisactivatedToAll();
	}
}

/////////////////////////////////////////////////////////
// tells everyone concerned that the line is disactivated
/////////////////////////////////////////////////////////
void TSpatialLine::sendDisactivatedToAll() {

	LineListnrIterator iter = fLineListeners.begin();
	LineListnrIterator iterEnd = fLineListeners.end();

	while (iter != iterEnd){
		(*iter)->measEntityDisactivated();
		iter++;
	}
}

//////////////////////////////////////////////////////////
// tells every concerned object that the line is activated
//////////////////////////////////////////////////////////
void TSpatialLine::sendActivatedToAll() {

	LineListnrIterator iter = fLineListeners.begin();
	LineListnrIterator iterEnd = fLineListeners.end();

	while (iter != iterEnd){
		(*iter)->measEntityActivated();
		iter++;
	}
}
////////////////////////////////////
// update the needed equations count
////////////////////////////////////
void TSpatialLine::updateNeededEquations(){

	int oldNdEq = fNeededEquations;
	fNeededEquations = fFirstEnd->getVariableDimension() +  fSecondEnd->getVariableDimension();

	LineListnrIterator iter = fLineListeners.begin();
	LineListnrIterator iterEnd = fLineListeners.end();
	while (iter != iterEnd){
		(*iter)->measEntityUpdated(TVMeasurableEntityLstnr::EUpdateMessage::kNeededEqChange);
		iter++;
	}

	//maybe the line has to be activated or disactivated
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

/////////////////////////////
// updates the equation count
/////////////////////////////
void TSpatialLine::updateEqCount(int eqCountUpdate){

	if (eqCountUpdate >0){

		if (fUsedInCalc){
			fEqCount += eqCountUpdate;
			LineListnrIterator iter = fLineListeners.begin();
			LineListnrIterator iterEnd = fLineListeners.end();
			while (iter != iterEnd){
				(*iter)->measEntityUpdated(TVMeasurableEntityLstnr::EUpdateMessage::kEqCountChange);
				iter++;
			}
		}


		if (!fUsedInCalc){
			if ((fEqCount + eqCountUpdate) >= fNeededEquations){
				fEqCount += eqCountUpdate;
				fUsedInCalc = true;

				LineListnrIterator iter = fLineListeners.begin();
				LineListnrIterator iterEnd = fLineListeners.end();
				while (iter != iterEnd){
					(*iter)->measEntityActivated();
					(*iter)->measEntityUpdated(TVMeasurableEntityLstnr::EUpdateMessage::kEqCountChange);
					iter++;
				}
			}
			else{
				LineListnrIterator iter = fLineListeners.begin();
				LineListnrIterator iterEnd = fLineListeners.end();
				while (iter != iterEnd){
					(*iter)->measEntityDisactivated();
					//the only object it will affect is the sender of the updateEqCount message
					iter++;
				}
			}
		}
	}


	if (eqCountUpdate <0){
		//no reaction if !fUsedInCalc, shouldn't occur

		if (fUsedInCalc){

			fEqCount += eqCountUpdate;
			LineListnrIterator iter = fLineListeners.begin();
			LineListnrIterator iterEnd = fLineListeners.end();
			while (iter != iterEnd){
				(*iter)->measEntityUpdated(TVMeasurableEntityLstnr::EUpdateMessage::kEqCountChange);
				iter++;
			}

			if (fEqCount < fNeededEquations){
				fUsedInCalc = false;
				iter = fLineListeners.begin();
				while (iter != iterEnd){
					(*iter)->measEntityDisactivated();
					iter++;
				}
			}
		}
	}
}
///////////////////////////////////
// adds a listener to the container
///////////////////////////////////
void TSpatialLine::addListener(TVMeasurableEntityLstnr* mel){

	fLineListeners.push_back(mel);
}

//////////////////////////////
//empty point listener methods
//////////////////////////////
void TSpatialLine::pointUpdated(TVSpatialPtListener::EUpdateMessage um){

}

void TSpatialLine::pointActivated(){

}

void TSpatialLine::pointDisactivated(){

}
*/
