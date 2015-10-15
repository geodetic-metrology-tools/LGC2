// TLGCCalcRecord.cpp
//
// Class containing a record of data from a simulation calculation.
//	Sigma0 parameters, calculated point data, and observation type summaries.
//
// Patterns:
//
// 
// Copyright 2007-2008 CERN TS/SU. All rights reserved. 
// Author M. Jones
//////////////////////////////////////////////////////////////////////

#include "TLGCCalcRecord.h"
#include	<list>
using namespace std;


// default constructor
TLGCCalcRecord::TLGCCalcRecord()
{
	fS0APosteriori = TReal(LITERAL(0.0)); 
	fChiHiLimit = TReal(LITERAL(0.0)); 
	fChiLoLimit = TReal(LITERAL(0.0)); 
}

// copy constructor
TLGCCalcRecord::TLGCCalcRecord(const TLGCCalcRecord& source)
{
	*this = source;
}

// destructor
TLGCCalcRecord::~TLGCCalcRecord()
{
}

// copy assignment operator
TLGCCalcRecord& TLGCCalcRecord::operator=(const TLGCCalcRecord& right)
{
	if (this != &right)
	{
		fS0APosteriori = right.fS0APosteriori; 
		fChiHiLimit = right.fChiHiLimit; 
		fChiLoLimit = right.fChiLoLimit; 
		fSimPoints = right.fSimPoints;
		fObsSummaries = right.fObsSummaries; 
	}

	return *this;
}

// equivalence operator
bool	TLGCCalcRecord::operator==(const TLGCCalcRecord& right) const
{
	bool equal = false;

	if(fS0APosteriori == right.fS0APosteriori 
		&& fChiHiLimit == right.fChiHiLimit 
		&& fChiLoLimit == right.fChiLoLimit
		&& fSimPoints.size() == right.fSimPoints.size()
		&& fSimPoints == right.fSimPoints
		&& fObsSummaries.size() == right.fObsSummaries.size()
		&& fObsSummaries == right.fObsSummaries
		) 
	{
		equal = true;
	}
	return equal;
}


//////////////////////////////////////////////////////////////////////////////////////
// return the LS position vector corresponding to a specified point name, if existing
//////////////////////////////////////////////////////////////////////////////////////
LSPosVecIter	TLGCCalcRecord::getPoint(const string name)
{
	LSPosVecIter iter = beginPoint();
	LSPosVecIter iterEnd = endPoint();

	while(iter != iterEnd)
	{
		if(iter->getName() == name)
		{return iter;}
		iter++;
	}
	return iter;
}


//////////////////////////////////////////////////////////////////////////////////////
// return the LS position vector corresponding to a specified point name, if existing
//////////////////////////////////////////////////////////////////////////////////////
LSPosVecConstIter	TLGCCalcRecord::getPoint(const string name) const
{
	LSPosVecConstIter iter = beginPoint();
	LSPosVecConstIter iterEnd = endPoint();

	while(iter != iterEnd)
	{
		if(iter->getName() == name)
		{return iter;}
		iter++;
	}
	return iter;
}


//////////////////////////////////////////////////////////////////////////////////////
// Adds a new observation summary to the list
//////////////////////////////////////////////////////////////////////////////////////
bool 	TLGCCalcRecord::addObsSumm(const TLGCObsSummary& obsSumm, TALGCConverter::ELGCObservations key)
{
	// add the summary to the map using the keyword and return the result
	// (true = successful insertion)
	// note how many observation summaries there are
	auto numSumm = fObsSummaries.size();

	// add the new observation summary
	fObsSummaries.push_back(std::make_pair(key,obsSumm));

	// check the observation summary was added ok, and return the result
	return numSumm + 1 == fObsSummaries.size(); 
}
