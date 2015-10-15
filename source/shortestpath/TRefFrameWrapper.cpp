//  TRefFrameWrapper.cpp
//
/** A Wrapper around a reference frame.
    The wrapper enables a pointer to a reference frame to be stored
	in an STL container, and to add the transformations linking a
	given reference frame to others in a network.

  
    Patterns:
    Wrapper around TVReferenceFrame.
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations

//#include  "TTransformWrapper.h"
#include  "TAReferenceFrame.h"
#include  "TRefFrameWrapper.h"
////////////////////////////////////////////////////////////////


//ClassImp(TRefFrameWrapper)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TRefFrameWrapper::TRefFrameWrapper()
	:  fFrame( 0 )
{	// Default Constructor
}


TRefFrameWrapper::TRefFrameWrapper( TAReferenceFrame* frame )
	:  fFrame( frame )
{	// Constructor taking a pointer to a  reference frame
}


TRefFrameWrapper::TRefFrameWrapper( const  TRefFrameWrapper& original )
{	// copy constructor

	// deep copy of the reference frame
	// if the reference frame is not a null pointer get a clone
	// else set this reference frame pointer to 0
	fFrame = original.getFrame();
}


TRefFrameWrapper::~TRefFrameWrapper()
{
	//delete fFrame;
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TRefFrameWrapper&  TRefFrameWrapper::operator=(const TRefFrameWrapper& right)
{	// Copy Assignment operator
	if (this != &right)
	{
		// deep copy of the reference frame
		//delete fFrame;
		fFrame = right.getFrame();
	}
	return *this;
}


bool  TRefFrameWrapper::operator==(const TRefFrameWrapper& right) const
{	// Equivalence Operator
	// two nodes are equivalent if their reference frames have the same name
	if ( getName() == right.getName() )
		return true;
	else
		return false;
}


bool  TRefFrameWrapper::operator<(const TRefFrameWrapper& right) const
{	// Less Than Operator
	// perform the comparison of nodes using the names of the reference frames
	if ( getName() < right.getName() )
		return true;
	else
		return false;
}


string  TRefFrameWrapper::getName() const
{	// get the name of the reference frame node
	// the node has the same name as the reference frame itself
	return fFrame->getName();
}


TAReferenceFrame*  TRefFrameWrapper::getFrame() const
{	// get a pointer to the reference frame
	return fFrame;
}


void TRefFrameWrapper::setFrame( TAReferenceFrame* frame )
{
	fFrame = frame;
	return;
}


void TRefFrameWrapper::setIdentifier(int i)
{
//	fFrame->setIdentifier(i);
	return;
}


//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

