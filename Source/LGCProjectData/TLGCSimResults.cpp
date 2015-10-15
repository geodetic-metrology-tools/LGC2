////////////////////////////////////////////////////////////////////
// TLGCSimResults.cpp : implementation file
// List of TLGCSimWorkingPoints.
// Class used for storage of the parts of the simulations' results file.
//
// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include  "TLGCSimResults.h"


//////////////////////
// default constructor
//////////////////////
TLGCSimResults::TLGCSimResults()
{

}


//////////////////////////////////////////////////
// constructor taking an iterator LSCalcPtIterator
//////////////////////////////////////////////////
TLGCSimResults::TLGCSimResults(TLSCalcWorkingPosVec* workingPts)
{
	// initialise the list of working points
	init(workingPts);
}


///////////////////
// copy constructor
///////////////////
TLGCSimResults::TLGCSimResults(const TLGCSimResults& source)
{
	*this = source;
}


/////////////
// destructor
/////////////
TLGCSimResults::~TLGCSimResults()
{
}



///////////////////////////
// copy assignment operator
///////////////////////////
TLGCSimResults& TLGCSimResults::operator=(const TLGCSimResults& right)
{
	if (this != &right)
	{
		fSimPoints = right.fSimPoints;
	}
	return *this;
}


////////////////////////////////////////////////////////
// initialise TLGCSimResults with a set of LSPosVecParam
////////////////////////////////////////////////////////
void	TLGCSimResults::init(TLSCalcWorkingPosVec* workingPts)
{
	LSPosVecIter iter, iterEnd;
	iter = workingPts->begin();
	iterEnd = workingPts->end();
	
	while(iter != iterEnd)
	{
		push_backPoint(TLGCSimPointSummary(iter));
		iter++;
	}

	return;
}


///////////////////////////////////////////////
// update the values of residus in LGCSimPoints
///////////////////////////////////////////////
bool TLGCSimResults::updateResValues(TLSCalcWorkingPosVec* workingPts)
{
	bool error = false;

	if(fSimPoints.size() == 0)
	{
		init(workingPts);
	}

	if (workingPts->size() == fSimPoints.size())
	{
		LSPosVecIter iter, iterEnd;
		iter = workingPts->begin();
		iterEnd = workingPts->end();

		SimPointIterator simIter;
		
		while(iter != iterEnd && error == false)
		{
			simIter = getPoint(iter);
			if (simIter != fSimPoints.end())
			{
				TFreeVector res = iter->getEstimatedValue() - iter->getProvisionalValue();
				simIter->addNewResValue(res);
			}
			else
			{
				error = true;
			}
			iter++;
		}
	}
	else
	{
		error = true;
	}

	return !error;
}
		
			

////////////////////////////////////////////////////////
// Adds a new TLGCSimPointSummary at the end of the list
////////////////////////////////////////////////////////
void	TLGCSimResults::push_backPoint(const TLGCSimPointSummary& simPt)
{
	SimPointIterator iter;

	// Checks if the point is already in the container or not
	bool notFound = true;

	SimPointIterator iterB = beginPoint();
	SimPointIterator iterE = endPoint();

	while (iterB!=iterE && notFound)
	{
		if (simPt == *iterB) {	
			iter = iterB; 
			notFound = false;
		}
		else
			iterB++;
	}
	
	// if the point isn't in the container -> addition at the end of the list
	if (notFound) {	
		fSimPoints.push_back(simPt);
		iter = --fSimPoints.end();
	}
	
	return; 
}

///////////////////////////////////////////////////////////////////////////////////////
// Erases the selected TLGCSimPoint and deletes its corresponding pointer from the list
///////////////////////////////////////////////////////////////////////////////////////
bool	TLGCSimResults::erase(TLGCSimPointSummary& simpt)
{
	bool isErased = false;

	SimPointIterator iterB = beginPoint();
	SimPointIterator iterE = endPoint();

	while (iterB!=iterE) {
		if (simpt == *iterB ) {
			fSimPoints.erase(iterB);
			isErased = true;
			iterB = iterE; }
		else
			iterB++;
	}
		
	return isErased;
}



/////////////////////////////////////////////////////////////////////////////////
// return the simulated points corresponding to a specified iterator, if existing
/////////////////////////////////////////////////////////////////////////////////
SimPointIterator	TLGCSimResults::getPoint(const LSPosVecIter posVec)
{
	SimPointIterator iter = beginPoint();
	SimPointIterator iterEnd = endPoint();

	while(iter != iterEnd)
	{
		if(iter->getPosVec()->getName() == posVec->getName())
		{return iter;}
		iter++;
	}
	return iter;
}


//////////////////////////////////////////////////////////////////////////////////
// return the simulated points corresponding to a specific point name, if existing
//////////////////////////////////////////////////////////////////////////////////
SimPointIterator	TLGCSimResults::getPoint(const string name)
{
	SimPointIterator iter = beginPoint();
	SimPointIterator iterEnd = endPoint();

	while(iter != iterEnd)
	{
		if(iter->getPosVec()->getName() == name)
		{return iter;}
		iter++;
	}
	return iter;
}


SimPointConstIter	TLGCSimResults::getPoint(const string name) const
{
	SimPointConstIter iter = beginPoint();
	SimPointConstIter iterEnd = endPoint();

	while(iter != iterEnd)
	{
		if(iter->getPosVec()->getName() == name)
		{return iter;}
		iter++;
	}
	return iter;
}

////////////////////////////////////////////////////////
// Adds a new TLGCSimCalcRecord at the end of the list
////////////////////////////////////////////////////////
CalcRecordIter 	TLGCSimResults::addSimCalc()
{
	TLGCCalcRecord record;
	CalcRecordIter iter;

	// add record at the end of the list, and get an iterator to it, to return
	fSimCalcRecords.push_back(record);
	iter = --(fSimCalcRecords.end());
	
	return iter; 
}

