#include "TWorkingPointNamePairs.h"



//////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////
TWorkingPointNamePairs::TWorkingPointNamePairs():
fListError(false), fWarning(""),fLastElNbr(0)
{// Default constructor
}


TWorkingPointNamePairs::~TWorkingPointNamePairs()
{// Destructor
}



////////////////////////////////////////////////////////////////
// PUBLIC METHODS		
////////////////////////////////////////////////////////////////

/* Equivalence operator */
bool TWorkingPointNamePairs::operator==(const TWorkingPointNamePairs& right) const
{
	return  fLastElNbr == right.fLastElNbr
				&& fListError == right.fListError
				&& fWarning == right.fWarning
				&& fWorkingPtNamePairs == right.fWorkingPtNamePairs;
}


PtNamePairIter	TWorkingPointNamePairs::push_back(const TPointNamePair& pnp)
{/* Inserts a new point name pair in the list,
	if it is not done yet (tests first if the object is already in the list or not)*/

	PtNamePairIter npIt;

	if(!fListError)
	{
		// Checks if the relative error is already in the container or not
		bool notFound = true;

		PtNamePairIter iterB = begin();
		PtNamePairIter iterE = end();

		while (iterB!=iterE && notFound)
		{
			if (pnp == *iterB)
			{	
				npIt = iterB; 
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
			fWorkingPtNamePairs.push_back(pnp);
			fLastElNbr++;
			// Debugging for the case where the push_back method fails
			if(fLastElNbr == fWorkingPtNamePairs.size())
			{
				//no error
				npIt = --(fWorkingPtNamePairs.end());
			}
			else
			{
				//error
				//treat error
				fListError = true;
				npIt = end();
			}
		}
	}
	else
	{
		npIt = end();
	}
		
	return npIt; 
}

bool TWorkingPointNamePairs::erase(TPointNamePair& pnp)
{/* Erases the selected new point name pair and
	deletes its corresponding pointer from the list if it exists*/


	bool isErased = false;

	PtNamePairIter iterB = begin();
	PtNamePairIter iterE = end();

	while (iterB!=iterE)
	{
		if (pnp == *iterB )
		{
			fWorkingPtNamePairs.erase(iterB);
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


//////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////

