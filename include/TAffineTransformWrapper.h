//  TAffineTransformWrapper.h
//
/** A Wrapper around a spatial transformation.
    The wrapper enables a pointer to a transformation to be stored
	in an STL container, and to add a pointer to each of the two 
	reference frames that are linked by the transformation.

  
    Patterns:
    Wrapper around TVAffineTransformation
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_AFFINE_TRANSFORMATION_WRAPPER
#define SU_AFFINE_TRANSFORMATION_WRAPPER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include  "TPtrWrapper.h"
//#include  "TConstPtrWrapper.h"
#include  "TAAffineTransformation.h"


//! Wrapper class
class  TAffineTransformWrapper : public TPtrWrapper<TAAffineTransformation>
{
public:
		/// default constructeur
		TAffineTransformWrapper();

		/// Constructor taking a pointer to a transformation
		TAffineTransformWrapper( TAAffineTransformation* transformer );

		/// Copy Constructor 
		TAffineTransformWrapper( const TAffineTransformWrapper & );

		/// Destructor
		virtual  ~TAffineTransformWrapper();

		TAAffineTransformation*  getTransformation()  {return this->getPtr();}
		const TAAffineTransformation*  getTransformation() const {return this->getPtr();}

private:
		//!Copy assignment operator
		TAffineTransformWrapper& operator=( TAffineTransformWrapper & right );

};

/*#include <list>
using namespace std;
	typedef list<TAffineTransformWrapper> CompositeTransformationSet;
	typedef CompositeTransformationSet::iterator CompositeIterator;
	typedef CompositeTransformationSet::const_iterator ConstCompositeIter;
	typedef list<TAffineTransformWrapper> MySet;
	typedef MySet::iterator MyIterator;
	typedef MySet::const_iterator ConstMyIter;*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////

/*
//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//Default constructor
//template <class T>
TAffineTransformWrapper::TAffineTransformWrapper() 
:TPtrWrapper<TVAffineTransformation>()
{}

//Constructor taking a pointer to the template object
//template <class T>
TAffineTransformWrapper::TAffineTransformWrapper( TVAffineTransformation * transformer )
:TPtrWrapper<TVAffineTransformation>(transformer)
{ 
}


//Copy constructor
//template <class T>
TAffineTransformWrapper::TAffineTransformWrapper( TAffineTransformWrapper & original )
:TPtrWrapper<TVAffineTransformation>( original )
{
	this->setPtr( original.getPtr() );
	this->setCountPtr( original.getCountPtr() );
	this->incrementCount();
}


//Destructor
//template <class T>
TAffineTransformWrapper::~TAffineTransformWrapper()
{  
}
*/
#endif // SU_TRANSFORMATION_WRAPPER
