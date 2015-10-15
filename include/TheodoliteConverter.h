#ifndef SU_THEODOLITE_CONVERTER
#define SU_THEODOLITE_CONVERTER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include "TAStreamFormatter.h"
#include "TTheodolite.h"
#include "TheodoliteTarget.h"
#include "TAngleConstants.h"
#include "TDistConstants.h"
#include "TALGCConverter.h"

class TheodoliteConverter : public TALGCConverter
{
public:

	TheodoliteConverter(TAStreamFormatter& s) : TALGCConverter(s) { }

	TTheodolite* readTheodolite(int& lineNumber, int& nextUnknownsNumber);

private:

	void readTheodoliteTargets(TTheodolite* theo, const string& defaultTargetID, int& nextUnknownsNumber);
	
};

#endif
