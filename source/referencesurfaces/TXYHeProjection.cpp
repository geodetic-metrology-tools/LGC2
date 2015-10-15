// TXYHeProjection.cpp
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
#include  "TAModifiedLocalAstronomicalRF.h"
#include  "TXYHeProjection.h"

////////////////////////////////////////////////////////////////


//ClassImp(TXYHeProjection)



//////////////////////////////////////////////////////////////////////
//Construction/Destruction
//////////////////////////////////////////////////////////////////////
TXYHeProjection::TXYHeProjection( )
	:TA2DPlus1MapProjection( "NULL" ), fEllipsoid(0), fMLASystem(0)
{	//default constructor
	
}


TXYHeProjection::TXYHeProjection( const string& name, TReferenceEllipsoid* ellipsoid,
								 TAModifiedLocalAstronomicalRF* ccs) 
	: TA2DPlus1MapProjection( name )
{	// constructor taking the name, the reference ellipsoid and the MLA system
	fEllipsoid = ellipsoid;
	fMLASystem = ccs;
}

// **Deliberately not implemented**
//TXYHeProjection::TXYHeProjection( const  TXYHeProjection& original )
//{	// copy constructor
//}


TXYHeProjection::~TXYHeProjection()
{
}

//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////
