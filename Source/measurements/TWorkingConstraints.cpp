// 
// TWorkingConstraints.cpp : implementation file
//
//! Class storing all the calculation constraints
//


#include	"TWorkingConstraints.h"


//////////////////////////////////////////////////////////////////////
// CONSTRUCTOR/DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TWorkingConstraints::TWorkingConstraints():fLastRadOffCstrNbr(0), fLastOrieCstrNbr(0)
{//default constructor
}

TWorkingConstraints::~TWorkingConstraints()
{//destructor
}


/////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS
/////////////////////////////////////////////////////////////
bool	TWorkingConstraints::addRadOffCnstr(TRadialOffsetCnstr *cstr)
{//Adding a Spatial Constraint to the ConstraintsSet member variable
	
	string empty("");
	bool insert_ok = false;
	
	// check that the Constraint is not already in the list
/*	if( count(fWorkRadOffCnstr.begin(), fWorkRadOffCnstr.end(), *cstr) == 0 )
	{*/

	//Modif du 04/06/2003 pour permettre comme dans LGC d inserer deux contraintes identiques
		if ( (cstr->getName()) == empty)
		{
			ostringstream oss;
			fLastRadOffCstrNbr ++;
			oss << "SpPt" << fLastRadOffCstrNbr;
			cstr->ifNotDoneSetName(oss.str());
		}
		fWorkRadOffCnstr.push_back(*cstr);

		insert_ok = true;
/*	}*/

	return insert_ok;
}



int		TWorkingConstraints::numberOfRadOffCnstr() const
{//Returns the number of constraints stored in the ConstraintsSet
	return fWorkRadOffCnstr.size();
}


RadOffCnstrIterator		TWorkingConstraints::getRadOffCnstrBeginIterator()
{//Returns an iterator on the first element of the fWorkRadOffCnstr
	return fWorkRadOffCnstr.begin();
}


RadOffCnstrConstIter	TWorkingConstraints::getRadOffCnstrBeginIterator() const
{//Returns a const iterator on the first element of the fWorkRadOffCnstr
	return fWorkRadOffCnstr.begin();
}


RadOffCnstrIterator		TWorkingConstraints::getRadOffCnstrEndIterator()
{//returns an iterator on one element past the end of fWorkRadOffCnstr
	return fWorkRadOffCnstr.end();
}


RadOffCnstrConstIter	TWorkingConstraints::getRadOffCnstrEndIterator() const
{//returns a const iterator on one element past the end of fWorkRadOffCnstr
	return fWorkRadOffCnstr.end();
}


RadOffCnstrIterator		TWorkingConstraints::getRadOffCnstr(string name)
{//returns an iterator on a Constraint, given its TRadialOffsetCnstrName

	RadOffCnstrIterator iter = fWorkRadOffCnstr.begin();
	RadOffCnstrIterator iterEnd = fWorkRadOffCnstr.end();

	bool notFound = true;

	while (iter != iterEnd  &&  notFound)
	{
		if ((iter->getName()) == name)
		{
			notFound = false;
		}
		else
		{
			iter ++;
		}
	}

	return iter;
}


RadOffCnstrConstIter	TWorkingConstraints::getRadOffCnstr(string name ) const
{//returns a const iterator on a Constraint, given its TRadialOffsetCnstrName
	RadOffCnstrConstIter iter = fWorkRadOffCnstr.begin();
	RadOffCnstrConstIter iterEnd = fWorkRadOffCnstr.end();

	bool notFound = true;

	while (iter != iterEnd  &&  notFound)
	{
		if ((iter->getName()) == name)
		{
			notFound = false;
		}
		else
		{
			iter ++;
		}
	}

	return iter;
}



/////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS FOR ORIENTATION CONSTRAINTS
/////////////////////////////////////////////////////////////
bool	TWorkingConstraints::addOrieCnstr(TOrientationCnstr *cstr)
{//Adding a Spatial Constraint to the ConstraintsSet member variable
	
	string empty("");
	bool insert_ok = false;
	
	// check that the Constraint is not already in the list
	if( count(fWorkOrieCnstr.begin(), fWorkOrieCnstr.end(), *cstr) == 0 )
	{
		if ( (cstr->getName()) == empty)
		{
			ostringstream oss;
			fLastOrieCstrNbr ++;
			oss << "SpPt" << fLastOrieCstrNbr;
			cstr->ifNotDoneSetName(oss.str());
		}
		fWorkOrieCnstr.push_back(*cstr);

		insert_ok = true;
	}

	return insert_ok;
}



int		TWorkingConstraints::numberOfOrieCnstr() const
{//Returns the number of constraints stored in the ConstraintsSet
	return fWorkOrieCnstr.size();
}


OrieCnstrIterator		TWorkingConstraints::getOrieCnstrBeginIterator()
{//Returns an iterator on the first element of the fWorkOrieCnstr
	return fWorkOrieCnstr.begin();
}


OrieCnstrConstIter		TWorkingConstraints::getOrieCnstrBeginIterator() const
{//Returns a const iterator on the first element of the fWorkOrieCnstr
	return fWorkOrieCnstr.begin();
}


OrieCnstrIterator		TWorkingConstraints::getOrieCnstrEndIterator()
{//returns an iterator on one element past the end of fWorkOrieCnstr
	return fWorkOrieCnstr.end();
}


OrieCnstrConstIter		TWorkingConstraints::getOrieCnstrEndIterator() const
{//returns a const iterator on one element past the end of fWorkOrieCnstr
	return fWorkOrieCnstr.end();
}


OrieCnstrIterator		TWorkingConstraints::getOrieCnstr(string name)
{//returns an iterator on a Constraint, given its TOrientationCnstrName

	OrieCnstrIterator iter = fWorkOrieCnstr.begin();
	OrieCnstrIterator iterEnd = fWorkOrieCnstr.end();

	bool notFound = true;

	while (iter != iterEnd  &&  notFound)
	{
		if ((iter->getName()) == name)
		{
			notFound = false;
		}
		else
		{
			iter ++;
		}
	}

	return iter;
}


OrieCnstrConstIter		TWorkingConstraints::getOrieCnstr(string name ) const
{//returns a const iterator on a Constraint, given its TOrientationCnstrName
	OrieCnstrConstIter iter = fWorkOrieCnstr.begin();
	OrieCnstrConstIter iterEnd = fWorkOrieCnstr.end();

	bool notFound = true;

	while (iter != iterEnd  &&  notFound)
	{
		if ((iter->getName()) == name)
		{
			notFound = false;
		}
		else
		{
			iter ++;
		}
	}

	return iter;
}


/////////////////////////////////////////////////////////////////////////////
//END
/////////////////////////////////////////////////////////////////////////////

