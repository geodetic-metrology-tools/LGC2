////////////////////////////////////////////////////////////////////
// TPointNamePair.cpp : implementation file
//  Pair of points names. 
//	Used to store the pair of points declared with option EREL
//
// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include "TPointNamePair.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TPointNamePair::TPointNamePair()
{// default constructor
}


TPointNamePair::TPointNamePair(const TSpatialPointName pt1, const TSpatialPointName pt2):
fPt1Name(pt1), fPt2Name(pt2)
{// Constructor taking 2 TSpatialPointName
}


TPointNamePair::TPointNamePair(const TPointNamePair& source)
{// Copy constructor
	fPt1Name = source.fPt1Name;
	fPt2Name = source.fPt2Name;
}


TPointNamePair::~TPointNamePair()
{// Destructor 
}


////////////////////////////////////////////////////////////////////////
//Member functions
////////////////////////////////////////////////////////////////////////


///////////////////////////
// copy assignment operator
///////////////////////////
TPointNamePair&  TPointNamePair::operator=(const TPointNamePair& right)
{
	// compare values to see if objects are the same
	if (this != &right)
	{
	// statistics on the observation residuals
		fPt1Name == right.fPt1Name;
		fPt2Name == right.fPt2Name;
	}

	return *this;
}


////////////////////////
// Equivalence operator
////////////////////////
bool TPointNamePair::operator==(const TPointNamePair& right) const
{
	if ((fPt1Name == right.fPt1Name) && (fPt2Name == right.fPt2Name))
		return true;
	else
		return false;
}

////////////////////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////////////////////