////////////////////////////////////////////////////////////////////
// TLGCOutputOptions.cpp : implementation file
// The parameters of a LGC project. Identifies the specific options to a LGC output file:
//  	naming conventions
//		punch files options
//
// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include  "TLGCOutputOptions.h"

///////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
///////////////////////////////////////////////////////////////////////////
TLGCOutputOptions::TLGCOutputOptions()
{//Default Constructor
	fUsingGEODE = TSpatialPointName::kName;
	fPuncOptions =	TLGCOutputOptions::kNo;
	fMesOptions =	TLGCOutputOptions::kNo;
	fCooOptions =	TLGCOutputOptions::kNo;
	fHistOption = false;
	fDeformationAnalysisOption = false;
}


TLGCOutputOptions::TLGCOutputOptions(const TLGCOutputOptions& source)
{//copy constructor
	*this = source;
}


TLGCOutputOptions::~TLGCOutputOptions()
{//destructor
}


//////////////////////////////////////////////////////////////////////////
//MEMBER FUNCTION
//////////////////////////////////////////////////////////////////////////
TLGCOutputOptions& TLGCOutputOptions::operator=(const TLGCOutputOptions& right)
{//copy assignment operator
	if (this != &right)
	{
		fUsingGEODE = right.fUsingGEODE;
		fPuncOptions = right.fPuncOptions;
		fMesOptions = right.fMesOptions;
		fCooOptions = right.fCooOptions;
		fHistOption = right.fHistOption;
		fDeformationAnalysisOption = right.fDeformationAnalysisOption;
	}
	return *this;
}


bool	TLGCOutputOptions::operator==(const TLGCOutputOptions& right) const
{//equivalence operator
	return ( (fUsingGEODE == right.fUsingGEODE) &&
			 (fPuncOptions== right.fPuncOptions) &&
			 (fMesOptions== right.fMesOptions) &&
			 (fCooOptions== right.fCooOptions) &&
			 (fHistOption == right.fHistOption));
}


////////////////////////////////////////////////////////////
// GEODE FUNCTION
////////////////////////////////////////////////////////////
void	TLGCOutputOptions::setUsingGEODEOption()
{//turn on the option for the use of GEODE naming convention
	fUsingGEODE = TSpatialPointName::kDBName;
	return;
}


bool	TLGCOutputOptions::usingGeodeFiles() const
{//indicate if the "_" must be replace by space in output file
	if (fUsingGEODE == TSpatialPointName::kName)
	{
		return false;
	}
	else
	{
		return true;
	}
}


///////////////////////////////////////////////////////
//END
///////////////////////////////////////////////////////
