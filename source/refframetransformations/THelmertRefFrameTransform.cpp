//  THelmertRefFrameTransform.cpp
//
/** A Helmert transformation between two reference frames.
    
	The two reference frames should be Euclidean reference frames,
	and the parameters of the transformation have to be defined 
	explicitly.

  
    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "THelmertTransformation.h"
//#include  "TCompositeAffTransform.h"
#include  "THelmertRefFrameTransform.h"
#include  "TAReferenceFrame.h"
////////////////////////////////////////////////////////////////


//ClassImp(THelmertRefFrameTransform)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
THelmertRefFrameTransform::THelmertRefFrameTransform()
: fFrom(0), fTo(0), fTransform(0)
{	// default constructor
}


THelmertRefFrameTransform::THelmertRefFrameTransform( TAReferenceFrame* from, 
													  TAReferenceFrame* to, 
													  THelmertTransformation* transform )
: fFrom(from), fTo(to), fTransform(0)
{	// constructor taking pointers to the source and destination reference frames
	fTransform = transform;
}

/*
THelmertRefFrameTransform::THelmertRefFrameTransform( TAReferenceFrame* from, 
													  TAReferenceFrame* to, 
													  const TScaleFactor& enlarg, const TRotation& rot, const TTranslation& transl)
: fFrom(from), fTo(to), fTransform(0)
{
	setTransform(enlarg, rot, transl);
}
*/


THelmertRefFrameTransform::THelmertRefFrameTransform( const  THelmertRefFrameTransform& original )
: fFrom(0), fTo(0), fTransform(0)
{// copy constructor
	*this = original;
}


THelmertRefFrameTransform::~THelmertRefFrameTransform()
{//destructor
	if( fTransform != 0 )
	{
		delete fTransform;
	}
}




//////////////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////
THelmertRefFrameTransform&  THelmertRefFrameTransform::operator=(const THelmertRefFrameTransform& right)
{	// Copy Assignment operator
	if (this != &right)
	{
		setSourceFrame( right.getSourceFrame() );
		setDestinationFrame( right.getDestinationFrame() );
		setTransform( right.getTransform()->clone() );
	}
	return *this;
}


THelmertRefFrameTransform*  THelmertRefFrameTransform::clone() const
{// Return a pointer to a clone of this reference frame
	THelmertRefFrameTransform *  result = new THelmertRefFrameTransform();
	result->setSourceFrame( this->getSourceFrame() );
	result->setDestinationFrame( this->getDestinationFrame() );
	result->setTransform( this->getTransform()->clone() );
	return new THelmertRefFrameTransform( *this );
}


THelmertRefFrameTransform*  THelmertRefFrameTransform::inverse() const
{// Return a pointer to the inverse of this transformtion

	THelmertRefFrameTransform * inver = this->clone();
	inver->invert();
	return inver;
}


void  THelmertRefFrameTransform::invert()
{
	TAReferenceFrame *  tmp = getSourceFrame();
	this->setSourceFrame( getDestinationFrame() );
	this->setDestinationFrame( tmp );
	this->getTransform()->invert();
	return;
}


bool  THelmertRefFrameTransform::transform( TPositionVector& pv ) const
{// transform a position vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(pv);

	return result;
}


bool  THelmertRefFrameTransform::transform( TFreeVector& fv ) const
{// transform a free vector
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(fv);

	return result;
}


bool  THelmertRefFrameTransform::transform( TRotationMatrix& rmx ) const
{// transform a Rotation Matrix
	bool result = false;
	  
	if( isInitialised() )
		  result = fTransform->transform(rmx);

	return result;
}


void THelmertRefFrameTransform::setTransform(THelmertTransformation* helmert)
{// set the transformation
	if (fTransform != 0)
	{
		delete fTransform;
	}
	fTransform = helmert;
}

/*
void THelmertRefFrameTransform::setTransform( const TScaleFactor& scale, const TRotation& rot, const TTranslation& transl )
{ 
	if (fTransform != 0)
	{
		delete fTransform;
	}
	fTransform = new THelmertTransformation(scale, rot, transl);
	return;
}*/



//////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////

