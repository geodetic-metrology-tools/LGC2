#ifndef SU_INSTRUMENTS_CONVERTER
#define SU_INSTRUMENTS_CONVERTER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include "TheodoliteConverter.h"
#include "TALGCConverter.h"
#include "TLGCProject.h"

class InstrumentsConverter : public TALGCConverter
{
public:

	InstrumentsConverter(TAStreamFormatter& s) : TALGCConverter(s) { }

	bool readInstruments(int& lineNumber, TLGCProject* project);

private:
	
};

#endif
