//
//TASpatialPtListener.h : header file
//////////////////////////////////////////////////////////

#ifndef SU_TASPPTLISTENER
#define SU_TASPPTLISTENER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//include files
#include <list>
//using namespace std;

#include "TVSpatialPtListener.h"

/*!\ingroup spatialobjects
	@{*/
//! Abstract base class defining the default implementation of the method of the TVSpatialPtListener interface
class TASpatialPtListener : public TVSpatialPtListener{

public:


	virtual void	pointDisactivated();
	virtual void	pointActivated();

	virtual void	pointUpdated(EUpdateMessage);

protected:

	TASpatialPtListener();
	virtual			~TASpatialPtListener();

};

/*@}*/

/*!@name Typedefs*/
//@{
//! Type of the container used to store points
typedef list<TVSpatialPtListener*> PtListnrContainer;
//! Type of an iterator pointing to an element of the container
typedef PtListnrContainer::iterator PtListnrIterator;
//! Type of a const iterator pointing to an element of the container
typedef PtListnrContainer::const_iterator PtListnrConstIter;
//@}


#endif
