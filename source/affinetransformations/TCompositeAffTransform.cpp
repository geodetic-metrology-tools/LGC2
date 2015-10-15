// TCompositeAffTransform.cpp
//
/** Class for transformations composed of multiple single transformations
Wrappers around those transformations are kept in a list

The last transformation added is applied first in the transformation of an object*/
//
// Patterns:
// this class is close to the pattern Composite
// 
// Copyright 2000-10 CERN SU, M.Jones. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "TCompositeAffTransform.h"
#include  "TPositionVector.h"
#include  "TFreeVector.h"
#include  "TRotationMatrix.h"

////////////////////////////////////////////////////////////////


//ClassImp(TCompositeAffTransform)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TCompositeAffTransform::TCompositeAffTransform()
{	// default constructor
	setStatus( TVNumericValue::kNull );
}


TCompositeAffTransform::TCompositeAffTransform( const TCompositeAffTransform & original )
{	// copy constructor
	*this = original;
}


// Constructor taking a TAAffineTransformation
TCompositeAffTransform::TCompositeAffTransform( const TAAffineTransformation & transfn )
: TAAffineTransformation()
{
	TAffineTransformWrapper wrapper( transfn.clone() );
	fComposite.push_back(wrapper);
	setStatus(transfn.getStatus());
}


TCompositeAffTransform::~TCompositeAffTransform()
{//destructor
	if(fComposite.size() != 0)
	{
		fComposite.erase(fComposite.begin(), fComposite.end());
	}
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////



TCompositeAffTransform&  TCompositeAffTransform::operator=( const TCompositeAffTransform& right )
{	// Copy Assignment operator

	if (this != &right)
	{
		if(fComposite.size() != 0)
		{
			//fComposite.clear();
			fComposite.erase(fComposite.begin(), fComposite.end());
		}
		//fComposite = right.getComposite();
		TCompositeAffTransform::ConstCompositeIter  end = right.getCompositeEndIterator();
		TCompositeAffTransform::ConstCompositeIter  beg = right.getCompositeBeginIterator();

		while(beg != end)
		{
			fComposite.push_back(*beg);
			beg++;
		}

		setStatus(right.getStatus());
	}
	return *this;
}


//////////////////////////////////////////////////////////////////////
// Operator ()
// add to the composite transformation by applying this transformation 
// to an affine transformation
//////////////////////////////////////////////////////////////////////
TCompositeAffTransform & TCompositeAffTransform::operator() ( const TAAffineTransformation & right )
{// Add another affine transformation to the composition  this = this( right )
	// right will be applied before any other transformations already in this composite
	TAffineTransformWrapper wrapper( right.clone() );
	this->fComposite.push_front( wrapper );
	this->setStatus( this->testStatus(right) );
	return *this;	
	
}


TCompositeAffTransform *  TCompositeAffTransform::clone() const
{// Return a pointer to a clone of this transformation

	TCompositeAffTransform *  clonedTransform = new TCompositeAffTransform();
	
	//fComposite = right.getComposite();
	TCompositeAffTransform::ConstCompositeIter  end = this->getCompositeEndIterator();
	TCompositeAffTransform::ConstCompositeIter  beg = this->getCompositeBeginIterator();

	while(beg != end)
	{
		// prepends a clone of the current transformation
		clonedTransform->prepend( *(beg->getTransformation()) );
		beg++;
	}

	clonedTransform->setStatus( this->getStatus() );
	
	return clonedTransform;
	
}


bool  TCompositeAffTransform::transform(TPositionVector& pv) const
{/// Return a transformed position vector
	bool trans = true;


	if (isNull() == false)
	{
		ConstCompositeIter iter = fComposite.begin();
		ConstCompositeIter iterEnd = fComposite.end();

		if(iter!=iterEnd) { trans = true; }
	
		while (iter != iterEnd && trans!=false)
		{
			trans = iter->getTransformation()->transform(pv);
			iter ++;
		}
	}

	return trans;	
}


bool  TCompositeAffTransform::transform(TFreeVector& fv) const 
{/// Return a transformed free vector
	bool trans = true;

	if (isNull() == false)
	{
		ConstCompositeIter iter = fComposite.begin();
		ConstCompositeIter iterEnd = fComposite.end();


		while (iter != iterEnd && trans != false)
		{
			trans = iter->getTransformation()->transform(fv);
			iter ++;
		}
	}

	return trans;
}


bool  TCompositeAffTransform::transform(TRotationMatrix& rm) const
{/// Return a transformed Rotation Matrix
	bool trans = true;
	
	if (isNull() == false)
	{
		ConstCompositeIter iter = fComposite.begin();
		ConstCompositeIter iterEnd = fComposite.end();

		if(iter!=iterEnd) { trans = true; }

		while (iter != iterEnd && trans != false)
		{
			trans = iter->getTransformation()->transform(rm);
			iter ++;
		}
	}

	return trans;
}


TPositionVector &  TCompositeAffTransform::operator() ( TPositionVector & right ) const
{// apply this transformation to a position vector 
	if ( this->isNull() || right.isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	else
	{
		ConstCompositeIter iter = fComposite.begin();
		ConstCompositeIter iterEnd = fComposite.end();

		while (iter != iterEnd )
		{
			iter->getTransformation()->operator ()(right);
			iter ++;
		}
	}
	return right;
}


TFreeVector &  TCompositeAffTransform::operator() ( TFreeVector & right ) const
{// apply this transformation to a free vector 
	if ( this->isNull() || right.isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	else
	{
		ConstCompositeIter iter = fComposite.begin();
		ConstCompositeIter iterEnd = fComposite.end();

		while (iter != iterEnd )
		{
			iter->getTransformation()->operator ()(right);
			iter ++;
		}
	}
	return right;
}


TRotationMatrix &  TCompositeAffTransform::operator() ( TRotationMatrix & right ) const
{// apply this transformation to a Rotation Matrix 
	if ( this->isNull() || right.isNull() )
	{
		right.setStatus( TVNumericValue::kNull );
	}
	else
	{
		ConstCompositeIter iter = fComposite.begin();
		ConstCompositeIter iterEnd = fComposite.end();

		while (iter != iterEnd )
		{
			iter->getTransformation()->operator ()(right);
			iter ++;
		}
	}
	return right;
}



TCompositeAffTransform * TCompositeAffTransform::inverse() const
{/// Return the inverse of this transformation
	TCompositeAffTransform * copy = this->clone();
	copy->invert();
	return copy;
}


void TCompositeAffTransform::invert()
{/// Invert the transformation, replaces the current transformation parameters
	
	fComposite.reverse();
	CompositeIterator iter = getCompositeBeginIterator();
	CompositeIterator iterEnd = getCompositeEndIterator();
	while (iter != iterEnd)
	{	
		iter->getTransformation()->invert();
		iter ++;
	}
	return;
}


/*// Append a TAAffineTransformation
void TCompositeAffTransform::append( const TAAffineTransformation &transf )
{
	TAffineTransformWrapper wrapper( &transf );
	this->fComposite.push_front( wrapper );
	if (transf.getStatus() == kNull && this->getStatus() != kNull)
	{
		setStatus(kNull);
	}
	return;
}


// Append a TAAffineTransformation
void TCompositeAffTransform::append( const TAAffineTransformation &transf )
{
	TAffineTransformWrapper wrapper( &transf );
	this->fComposite.push_front( wrapper );
	if (transf.getStatus() == kNull && this->getStatus() != kNull)
	{
		setStatus(kNull);
	}
	return;
}
*/

// Prepend a TAAffineTransformation
void TCompositeAffTransform::prepend( const TAAffineTransformation & transf )
{
	TAffineTransformWrapper wrapper( transf.clone() );
	this->fComposite.push_back( wrapper );
	if (transf.getStatus() == kNull && this->getStatus() != kNull)
	{
		setStatus(kNull);
	}
	return;
}
	

/*void TCompositeAffTransform::add(const TAAffineTransformation& right)
{ 
	TAffineTransformWrapper wrapper (&right);
	fComposite.push_front(wrapper);
	setStatus(right.getStatus());
	return;
}*/



///////////////////////////////////////////////////////////////////////////////////////
//END
///////////////////////////////////////////////////////////////////////////////////////




