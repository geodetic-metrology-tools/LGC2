//  TTransformWrapper.cpp
//
/** A Wrapper around a spatial transformation.
    The wrapper enables a pointer to a transformation to be stored
	in an STL container.

  
    Patterns:
    Wrapper around TVTransformation
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "TVTransformation.h"
#include  "TTransformWrapper.h"

#include "TARefFrameTransformation.h"
////////////////////////////////////////////////////////////////


//ClassImp(TTransformWrapper)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TTransformWrapper::TTransformWrapper() : fTransformer(0)
{
}


TTransformWrapper::TTransformWrapper( TARefFrameTransformation*  transformer )
	: fTransformer( transformer )
{	// constructor taking a pointer to a transformation 
}


TTransformWrapper::TTransformWrapper( const  TTransformWrapper& original )
{	// copy constructor

	// deep copy of the transformation
	// if the transformation is not a null pointer get a clone
	// else set this transformation pointer to 0
	fTransformer = original.getTransformation();
}


TTransformWrapper::~TTransformWrapper()
{   // destructor

	//delete fTransformer;
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


TTransformWrapper&  TTransformWrapper::operator=(const TTransformWrapper& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		// deep copy of the transformation
		//delete fTransformer;
		fTransformer = ( right.getTransformation() );
	}
	return *this;
}


TARefFrameTransformation*  TTransformWrapper::getTransformation() const
{	// get a pointer to the transformation

	return fTransformer;
}


TARefFrameTransformation*  TTransformWrapper::getCopyOfTransformation() const
{	// get a pointer to the transformation

	return fTransformer->clone();
}



void TTransformWrapper::setTransformation(TARefFrameTransformation* transform)
{
	fTransformer = transform;
}

//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////

