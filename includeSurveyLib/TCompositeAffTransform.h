// TCompositeAffTransform.h
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



#ifndef SU_COMPOSITE_AFFINE_TRANSFO
#define SU_COMPOSITE_AFFINE_TRANSFO


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include <list>
//using namespace std;

#include  "TAffineTransformWrapper.h"
#include  "TAAffineTransformation.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Class for transformations composed using wrappers
class  TCompositeAffTransform : public TAAffineTransformation//, virtual public TVAffineTransformation  
{
public:
	//typedefs
    typedef std::list<TAffineTransformWrapper> CompositeTransformationSet;
	typedef CompositeTransformationSet::iterator CompositeIterator;
	typedef CompositeTransformationSet::const_iterator ConstCompositeIter;

	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TCompositeAffTransform();

		/// Copy Constructor 
		TCompositeAffTransform( const TCompositeAffTransform & );

		//! Constructor taking a TAAffineTransformation
		explicit TCompositeAffTransform( const TAAffineTransformation & );

		/// Destructor
		virtual  ~TCompositeAffTransform();
	//@}


	/**@name Member Functions */
	//@{
		using TAAffineTransformation::operator();
		using TAAffineTransformation::transform;

		/// Copy Assignment Operator 
		TCompositeAffTransform& operator=( const TCompositeAffTransform& );

		//! add to the composite transformation by applying this transformation to an affine transformation
		// the added transformation will be applied before any other transformations already in this composite
		virtual  TCompositeAffTransform & operator()( const TAAffineTransformation & );
	
		//! Return a pointer to a clone of this transformation
		virtual  TCompositeAffTransform *  clone() const;

		//! Transform a TPosition Vector
		virtual bool transform( TPositionVector& ) const;

		//! Transform a TFreeVector
		virtual bool transform( TFreeVector & ) const;

		//! Transform a TRotationMatrix
		virtual bool transform( TRotationMatrix& ) const;

		//! apply this transformation to a position vector 
		virtual  TPositionVector &  operator() ( TPositionVector & ) const;

		//! apply this transformation to a free vector 
		virtual  TFreeVector &  operator() ( TFreeVector & ) const;

		//! apply this transformation to a Rotation Matrix 
		virtual  TRotationMatrix &  operator() ( TRotationMatrix & ) const;

		//! Return a pointer to the inverse of this transformation
		TCompositeAffTransform * inverse() const;

		//! Invert the transformation, replaces the current transformation parameters
		void invert();

		//! iterator begin
		CompositeIterator		getCompositeBeginIterator() {return fComposite.begin();}
		//! const iterator begin
		ConstCompositeIter		getCompositeBeginIterator() const {return fComposite.begin();}
		
		//! iterator end
		CompositeIterator		getCompositeEndIterator() {	return fComposite.end();}
		//! const iterator end
		ConstCompositeIter		getCompositeEndIterator() const {	return fComposite.end();}
		
		//! return the composite set
		//CompositeTransformationSet getComposite() const;

		// Prepend a TAAffineTransformation, 
		// the added transformation will be applied after any other transformations already in this composite
		void prepend( const TAAffineTransformation & transf );
	//@}
protected:
		
private:
	 	// Member Attributes
		
		CompositeTransformationSet		fComposite; /*!< composite transformation */

	//ClassDef(TCompositeAffTransform, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////

//inline TCompositeAffTransform::CompositeTransformationSet TCompositeAffTransform::getComposite() const {return fComposite;}

/*
// prepend a TAAffineTransformation
TCompositeAffTransform &operator+( TAAffineTransformation& left, TCompositeAffTransform &right )
{// prepend another affine transformation to the composition  right = left + right
	
	right.prepend(left);
	return right;	

}*/


#endif // SU_COMPOSITE_AFFINE_TRANSFO
