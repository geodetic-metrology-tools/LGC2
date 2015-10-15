// TANumericValue.cpp
/*
The TANumericValue class implements the interface for a
numerical value.
 
It is a abstract base class (PABC) that should be inherited 
by all numerical values. 

The methods declared in the TVNumericValue for accessing, 
changing, and assessing the status of a numerical value 
are defined.

Patterns:
 
 
Copyright 2002 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////


//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
//
#include	"TANumericValue.h"
#include <utility>
////////////////////////////////////////////////////////////////



//ClassImp(TANumericValue)

TANumericValue::TANumericValue()
{
	fStatus = kNull;
}

void TANumericValue::swap(TANumericValue & other) throw()
{
	std::swap(fStatus, other.fStatus);
}

//////////////
// Test Status
//////////////



TVNumericValue::EStatus TANumericValue::testStatus( const TVNumericValue & obj ) const
{   // Determines the status of a numerical value that should
    // be assigned when an operation is performed on this 
	// numeric value and the given value.
	
	if ( this->isNull() || obj.isNull() )
	{
		return TVNumericValue::kNull;
	}
	else
	{
		return TVNumericValue::kKnown;
	}	
}
