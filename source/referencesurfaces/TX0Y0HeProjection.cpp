// TX0Y0HeProjection.cpp
//
/** A projection on XY plane with ellipsoidal height 

  
    Patterns:
    A concrete STATE of a Spatial Position
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//forward declarations
#include  "TReferenceEllipsoid.h"
#include  "TXYHeProjection.h"
#include  "TX0Y0HeProjection.h"
////////////////////////////////////////////////////////////////


//ClassImp(TX0Y0HeProjection)



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TX0Y0HeProjection::TX0Y0HeProjection( )
	:TA2DPlus1MapProjection( "NULL" ), fXYHe(0)
{	//default constructor
	
}


TX0Y0HeProjection::TX0Y0HeProjection( const string& name, TXYHeProjection* xyhe) 
	: TA2DPlus1MapProjection( name )
{	// constructor taking the name, the reference ellipsoid and the MLA system
	fXYHe = xyhe;
}


// **Deliberately not implemented**
//TX0YH0eProjection::TXYHeProjection( const  TX0Y0HeProjection& original )
//{	// copy constructor
//}


TX0Y0HeProjection::~TX0Y0HeProjection()
{
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
// **Deliberately not implemented**
//TX0Y0HeProjection&  TX0Y0HeProjection::operator=(const TX0Y0HeProjection& right)
//{	// Copy Assignment operator
//
//	if (this != &right)
//	{
//	}
//	return *this;
//}



//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////

