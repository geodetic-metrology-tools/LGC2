//!
//! TPtrWrapper
//!
//! A templated counter pointer. 
//! A wrapper around the pointed to object that manages a count of it's copies.
//! It is responsible for deleting the object being pointed to, and only does
//! this when the last copy of the wrapper is being deleted. 
//!
//! Only wrap pointers that have memory allocated using new.
//!
//! Don't put the same pointer in a second object also responsible fordeleting 
//! it's children. The TPtrWrapper is responsible for deleting the pointed to object.
//! 
//! Copyright 2002-2010, CERN, SU, M/Jones. All rights reserved.
//!////////////////////////////////////////////////////////////////////




#ifndef SU_POINTERWRAPPER
#define SU_POINTERWRAPPER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
//
//include files
//using namespace std;
//typedefs
//
//
////////////////////////////////////////////////////////////////


template <class T>
class TPtrWrapper 
{
public:
	/*!@name Constructors / Destructor*/
	//@{
	//!Default constructor
    //TPtrWrapper();
	//!Constructor taking a pointer to the template object
    TPtrWrapper( T* pointer = 0 );
	//!Copy constructor
    TPtrWrapper( const TPtrWrapper<T>& source ); 
	//!Destructor
    virtual ~TPtrWrapper();
	//@}
	
	//!Copy assignment operator
    TPtrWrapper<T>& operator=( const TPtrWrapper<T>& right );
	
	//!Get the pointer
	T* getPtr() const { return fPointer; };

protected:
	//!Manages the changes necessary when a wrapper is deleted
	void  deletion();
	//!Get the count
	int* getCountPtr() const { return fCount; } ;

private:
    T*	  fPointer;
	int*  fCount;
};


template <class T>
bool operator==( const TPtrWrapper<T>& wrapper1, const TPtrWrapper<T>& wrapper2 ) 
{
    return *(wrapper1.getPtr())  ==  *(wrapper2.getPtr());
}

template <class T>
bool operator<( const TPtrWrapper<T>& wrapper1, const TPtrWrapper<T>& wrapper2 ) 
{
    return *(wrapper1.getPtr())  <  *(wrapper2.getPtr());
}



//Default constructor
/*template <class T>
TPtrWrapper<T>::TPtrWrapper() : fPointer(0), fCount(0) 
{ 
}*/


//Constructor taking a pointer to the template object
template <class T>
TPtrWrapper<T>::TPtrWrapper( T* pointer ) : fPointer( pointer ) 
{ 
	if ( fPointer == 0 )
		fCount = 0;
	else
		fCount = new int(1);
}


//Copy constructor
template <class T>
TPtrWrapper<T>::TPtrWrapper( const TPtrWrapper<T>& source ) : fPointer(0), fCount(0)
{
	*this = source;
}


//Destructor
template <class T>
TPtrWrapper<T>::~TPtrWrapper() 
{ 
	deletion(); 
}


//Assignment operator
template <class T>
TPtrWrapper<T>& TPtrWrapper<T>::operator= ( const TPtrWrapper<T>& right ) 
{
    if( this != &right  &&  fPointer != right.getPtr() )
	{
		deletion();
		fPointer = right.getPtr();
		fCount = right.getCountPtr();
		if ( fPointer != 0 ) 
			(*fCount)++;
	}

    return *this;
}


//Manages the changes necessary when a wrapper is deleted
template <class T>
void  TPtrWrapper<T>::deletion()
{
	// If the wrapper is pointing to something, and this was the last
	// wrapper around the pointer, delete the pointers
	if( fCount  &&  --(*fCount) == 0 )
	{
		delete fPointer;
		delete fCount;

		fPointer = 0;
		fCount = 0;
	}

	return;
}



#endif  //SU_POINTERWRAPPER
