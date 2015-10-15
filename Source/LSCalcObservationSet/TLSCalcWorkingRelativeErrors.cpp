////////////////////////////////////////////////////////////////////
// TLSCalcWorkingRelativeErrors.cpp :Implementation file
// Container for the relative errors to be determined as part of a LGC calculation
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include "TLSCalcWorkingRelativeErrors.h"

//////////////////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////
TLSCalcWorkingRelativeErrors::TLSCalcWorkingRelativeErrors():fWarning(""),fListError(false),
fLastElNbr(0) {// Default constructor
}


TLSCalcWorkingRelativeErrors::~TLSCalcWorkingRelativeErrors()
{//Destructor 
}

////////////////////////////////////////////////////////////////
// PUBLIC METHODS		
////////////////////////////////////////////////////////////////
LSRelErrorIter TLSCalcWorkingRelativeErrors::begin()
{//Returns an iterator to the first element in the list 
	return fRelErrorsList.begin();
}


LSRelErrorConstIter TLSCalcWorkingRelativeErrors::begin() const
{// Returns a const iterator to the first element in the list 
	return fRelErrorsList.begin();
}


LSRelErrorIter	TLSCalcWorkingRelativeErrors::end()
{// Returns an iterator pointing after the last element in the list
	return fRelErrorsList.end();
}


LSRelErrorConstIter TLSCalcWorkingRelativeErrors::end() const
{// Returns a const iterator pointing after the last element in the list 
	return fRelErrorsList.end();
}

	
LSRelErrorIter	TLSCalcWorkingRelativeErrors::push_back(const TLSCalcRelativeError& erel)
{/* Inserts a new TLSCalcRelativeError in the list,
	if it is not done yet (tests first if the object is already in the list or not)*/

	LSRelErrorIter erelIt;

	if(!fListError)
	{
		// Checks if the relative error is already in the container or not
		bool notFound = true;

		LSRelErrorIter iterB = begin();
		LSRelErrorIter iterE = end();

		while (iterB!=iterE && notFound)
		{
			if (const_cast<TLSCalcRelativeError&>(erel) == *iterB)
			{	
				erelIt = iterB; 
				notFound = false;
			}
			else
			{
				iterB++;
			}
		}
		
		// if the observation isn't in the container -> addition at the end of the list
		if (notFound)
		{	
			fRelErrorsList.push_back(erel);
			fLastElNbr++;
			// Debugging for the case where the push_back method fails
			if(fLastElNbr == fRelErrorsList.size())
			{
				//no error
				erelIt = --(fRelErrorsList.end());
			}
			else
			{
				//error
				//treat error
				fListError = true;
				erelIt = end();
			}
		}
	}
	else
	{
		erelIt = end();
	}
		
	return erelIt; 
}


bool TLSCalcWorkingRelativeErrors::erase(TLSCalcRelativeError& erel)
{/* Erases the selected TLSCalcRelativeError and
	deletes its corresponding pointer from the list if it exists*/


	bool isErased = false;

	LSRelErrorIter iterB = begin();
	LSRelErrorIter iterE = end();

	while (iterB!=iterE)
	{
		if (erel == *iterB )
		{
			fRelErrorsList.erase(iterB);
			isErased = true;
			fLastElNbr--;
			iterB = iterE;
		}
		else
		{
			iterB++;
		}
	}
	return isErased;
}

// sets warning msg
void TLSCalcWorkingRelativeErrors::setWarning(const string msg) {
	
	fWarning = msg;
	return;
}
	
/////////////////////////////////////////////////////////////////////////////////////////////
//end
/////////////////////////////////////////////////////////////////////////////////////////////
