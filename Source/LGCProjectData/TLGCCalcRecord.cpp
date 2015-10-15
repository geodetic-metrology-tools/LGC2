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
#include "InstrumentsCalcRecord.h"


// default constructor
TLGCCalcRecord::TLGCCalcRecord()
{
	fS0APosteriori = TReal(LITERAL(0.0)); 
	fChiHiLimit = TReal(LITERAL(0.0)); 
	fChiLoLimit = TReal(LITERAL(0.0)); 
}

// destructor
TLGCCalcRecord::~TLGCCalcRecord()
{
	list<InstrumentsCalcRecord*>::iterator i = instrumentCalcRecords.begin();

	while (i != instrumentCalcRecords.end())
	{
		delete *i;

		i++;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
// return the LS position vector corresponding to a specified point name, if existing
//////////////////////////////////////////////////////////////////////////////////////
LSPosVecIter	TLGCCalcRecord::getPV(const string name)
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
LSPosVecConstIter	TLGCCalcRecord::getPV(const string name) const
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
	unsigned numSumm = fObsSummaries.size();

	// add the new observation summary
	fObsSummaries.push_back(std::make_pair(key,obsSumm));

	// check the observation summary was added ok, and return the result
	return numSumm + 1 == fObsSummaries.size(); 
}

void	TLGCCalcRecord::addInstrumentCalcRecord(InstrumentsCalcRecord* record)
{
	instrumentCalcRecords.push_back(record);
}
