// InstrumentsCalcRecord.h
//

// Patterns:
//
// 
// Copyright 2007-2008 CERN TS/SU. All rights reserved. 
// Author M. Jones
//////////////////////////////////////////////////////////////////////



#ifndef SU_INSTRUMENTS_CALC_RECORD
#define SU_INSTRUMENTS_CALC_RECORD


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "LSCalcDataSet.h"

#include <hash_map>
using namespace stdext;

class InstrumentsCalcRecord
{
public:
	InstrumentsCalcRecord(LSCalcDataSet* calcDataSet, TLGCDataSet* dataSet);

	~InstrumentsCalcRecord();

	hash_map<string, TLGCObsSummary*>* getTheodoliteObservationSummary(const string& instrumentID);

	hash_map<string, TLGCObsSummary*>* getEDMObservationSummary(const string& instrumentID);

	hash_map<string, TLGCObsSummary*>* getScaleObservationSummary(const string& instrumentID);

	TLGCObsSummary* getGyroscopeObservationSummary(const string& instrumentID);

	hash_map<string, TLGCObsSummary*>* getLevelObservationSummary(const string& instrumentID);

private:
	
	// thats a hash from the instrument's ID to a hash from observation types to summaries
	hash_map<string, hash_map<string, TLGCObsSummary*>* > theodolitesMap;
	hash_map<string, hash_map<string, TLGCObsSummary*>* > edmsMap;
	hash_map<string, hash_map<string, TLGCObsSummary*>* > scalesMap;
	hash_map<string, TLGCObsSummary*> gyroscopesMap;
	hash_map<string, hash_map<string, TLGCObsSummary*>* > levelsMap;
};



#endif // SU_INSTRUMENTS_CALC_RECORD


