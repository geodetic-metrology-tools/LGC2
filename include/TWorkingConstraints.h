// 
// TWorkingConstraints.h : Header File
//
//! Class storing all the calculation constraints
//

#ifndef SU_WORKING_CONSTRAINTS
#define SU_WORKING_CONSTRAINTS

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
//
#include <list>
#include <algorithm>
#include <sstream>
using namespace std;

#include	"TRadialOffsetCnstr.h"
#include	"TOrientationCnstr.h"
#include	"UEOIndices.h"
/////////////////////////////////////////////////////



// Class Definition
class	TWorkingConstraints{

public:


	/*!@name Constructors / Destructor*/
	//@{
	//!Default constructor
	TWorkingConstraints();
	//!Destructor
	virtual ~TWorkingConstraints();
	//@}


	/*!@name Member functions for Radial Offset Constraint*/
	//@{
		/*!Adds a contraints to the container
		\param constr a pointer to the contraints to be stored
		\return a true boolean if the contraints was added to the container*/
		bool					addRadOffCnstr(TRadialOffsetCnstr* constr);
		/*!\return the number of stored contraints*/
		int						numberOfRadOffCnstr() const;
		/*!\return an iterator pointing to the first line of the container*/
		RadOffCnstrIterator		getRadOffCnstrBeginIterator();
		/*!\return a cons iterator pointing to the first line of the container*/
		RadOffCnstrConstIter	getRadOffCnstrBeginIterator() const;
		/*!\return an iterator pointing to one past the last line of the container*/
		RadOffCnstrIterator		getRadOffCnstrEndIterator();
		/*!\return a cons iterator pointing to one past the last line of the container*/
		RadOffCnstrConstIter	getRadOffCnstrEndIterator() const;
		/*!\return an iterator pointing to a line specified through its associed point's name*/
		RadOffCnstrIterator		getRadOffCnstr(string name);
		/*!\return a const iterator pointing to a line specified through its associed point's name*/
		RadOffCnstrConstIter	getRadOffCnstr(string name) const;
	//@}


	/*!@name Member functions for orientation constraints*/
	//@{
		/*!Adds a contraints to the container
		\param constr a pointer to the contraints to be stored
		\return a true boolean if the contraints was added to the container*/
		bool					addOrieCnstr(TOrientationCnstr* constr);
		/*!\return the number of stored contraints*/
		int						numberOfOrieCnstr() const;
		/*!\return an iterator pointing to the first line of the container*/
		OrieCnstrIterator		getOrieCnstrBeginIterator();
		/*!\return a cons iterator pointing to the first line of the container*/
		OrieCnstrConstIter		getOrieCnstrBeginIterator() const;
		/*!\return an iterator pointing to one past the last line of the container*/
		OrieCnstrIterator		getOrieCnstrEndIterator();
		/*!\return a cons iterator pointing to one past the last line of the container*/
		OrieCnstrConstIter		getOrieCnstrEndIterator() const;
		/*!\return an iterator pointing to a line specified through its associed point's name*/
		OrieCnstrIterator		getOrieCnstr(string name);
		/*!\return a const iterator pointing to a line specified through its associed point's name*/
		OrieCnstrConstIter		getOrieCnstr(string name) const;
	//@}



private:

	
	int						fLastRadOffCstrNbr;
	RadOffCnstrContainer	fWorkRadOffCnstr;
	int						fLastOrieCstrNbr;
	OrieCnstrContainer		fWorkOrieCnstr;
};

#endif

////////////////////////////////////////////
// Inline definitions
////////////////////////////////////////////
