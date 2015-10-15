// TXYHgProjection.cpp
//
/** A projection on XY plane with height above the Geoid

  
    Patterns:
    A concrete STATE of a Spatial Position
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//forward declarations

#include  "TAGeoidModel.h"
#include  "TXYHeProjection.h" //modified
#include  "TXYHgProjection.h"
////////////////////////////////////////////////////////////////


//ClassImp(TXYHgProjection)



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TXYHgProjection::TXYHgProjection( )
	:TA2DPlus1MapProjection( "NULL" ), fGeoid(0), fXYHe(0) //fMLASystem(0) modified
{	//default constructor
	
}


TXYHgProjection::TXYHgProjection( const string& name, TAGeoidModel* geoid,
								 TXYHeProjection* xyhe) //TAModifiedLocalAstronomicalRF* ccs)  modified
	: TA2DPlus1MapProjection( name )
{	// constructor taking the name, the geoid and the MLA system
	fGeoid = geoid;
	fXYHe = xyhe;
}

// **Deliberately not implemented**
//TXYHgProjection::TXYHgProjection( const  TXYHgProjection& original )
//{	// copy constructor
//}


TXYHgProjection::~TXYHgProjection()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
// **Deliberately not implemented**
//TXYHeProjection&  TXYHeProjection::operator=(const TXYHeProjection& right)
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
