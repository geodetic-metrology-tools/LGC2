//
// TSpatialLineName.cpp : implementation file
// class modelising a straight spatial line

#include "TSpatialLineName.h"


////////////////////////////////////////////////////////////////////////////////
//constructor / destructor
////////////////////////////////////////////////////////////////////////////////
TSpatialLineName::TSpatialLineName() : fName(""), fFirstEndName(), fSecondEndName()
{//default constructor
}


TSpatialLineName::TSpatialLineName(const TSpatialPointName firstEndName)
{//constructor vertical line
	std::string nullName ="";
	TSpatialPointName secondEndName (nullName);

	fFirstEndName = firstEndName;
	fSecondEndName = secondEndName;

	fName = firstEndName.getName();
}


TSpatialLineName::TSpatialLineName(const TSpatialPointName firstEndName, const TSpatialPointName secondEndName)
{//constructor spatial line

	fFirstEndName = firstEndName;
	fSecondEndName = secondEndName;

	fName = firstEndName.getName() + fSecondEndName.getName();
}


TSpatialLineName::TSpatialLineName(const TSpatialLineName& source)
{//copy constructor
	(*this)= source;
}


TSpatialLineName::~TSpatialLineName()
{//detructor
}


///////////////////////////////////////////////////////////////////////////////
//member functions
///////////////////////////////////////////////////////////////////////////////
bool TSpatialLineName::operator==(const TSpatialLineName& right) const
{

	if (fName == right.getName())
	{
		return true;
	}
	else
	{
		return false;
	}
}


void TSpatialLineName::operator=(const TSpatialLineName& right)
{
	if (this != &right)
	{
		fFirstEndName = right.fFirstEndName;
		fSecondEndName = right.fSecondEndName;
		fName = right.getName();

	}

	return;
}


		
bool TSpatialLineName::operator<( const TSpatialLineName& right) const
{//!Less than operator
	bool answer = false;

	if (fName<right.fName)
	{
		answer=true;
	}

	return answer;
}


		
bool	TSpatialLineName::ifNotDoneDefineLine(const TSpatialPointName firstEndName)
{/*!\return true if the line is define with the new arguments*/
	bool retVal = false;

	if(!(isDefine()))
	{
		std::string nullName ="";
		TSpatialPointName secondEndName (nullName);

		fFirstEndName = firstEndName;
		fSecondEndName = secondEndName;

		fName = firstEndName.getName();

		retVal = true;
	}
	return retVal;
}


bool	TSpatialLineName::ifNotDoneDefineLine(const TSpatialPointName firstEndName, const TSpatialPointName secondEndName)
{/*!\return true if the line is define with the new arguments*/
	bool retVal = false;

	if(!(isDefine()))
	{
		fFirstEndName = firstEndName;
		fSecondEndName = secondEndName;

		fName = firstEndName.getName() + fSecondEndName.getName();

		retVal = true;
	}
	return retVal;
}

TSpatialPointName	TSpatialLineName::getFirstEndName() const
{//returns a name of the first end point of the line
	return fFirstEndName;
}


TSpatialPointName	TSpatialLineName::getSecondEndName() const
{//returns a name of the second end point of the line
	return fSecondEndName;
}


std::string TSpatialLineName::getName() const
{//gets the line's name
	return fName;
}



		
void	TSpatialLineName::buildErrorName()
{/*!build the name (used only for TWorkingLines
	if the line which is added to the list is not defined)*/
	if(!(isDefine()))
	{
		fName = "LineNotDefine";
	}
}


///////////////////////////////////////////////////////////////////////
//Private function
///////////////////////////////////////////////////////////////////////
bool	TSpatialLineName::isDefine() const
{//return true if the line is define
	bool retVal = false;

	if(fName != "")
	{
		retVal = true;
	}

	return retVal;
}

