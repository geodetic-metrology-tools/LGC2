// TALSCalcParameter.cpp

#include "lsalgo/TALSCalcParameter.h"

/*
// Default constructor
TALSCalcParameter::TALSCalcParameter() {

	//not implemented
}
*/


// Copy constructor
TALSCalcParameter::TALSCalcParameter(const TALSCalcParameter& source) {

	fName = source.fName;
}


// Constructor using an identifier
TALSCalcParameter::TALSCalcParameter(const std::string & name) : fName(name) 
{
}

// Destructor
TALSCalcParameter::~TALSCalcParameter() {
}

// Overloaded equality operator
bool  TALSCalcParameter::operator==(const TALSCalcParameter& right) const {

	bool isEqualTo = false;

	if (fName == right.fName)
		isEqualTo = true;

	return isEqualTo;
}

// Returns the parameter's name
std::string TALSCalcParameter::getName() const {

	return fName;
}


