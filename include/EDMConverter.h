#ifndef SU_EDM_CONVERTER
#define SU_EDM_CONVERTER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include "TAStreamFormatter.h"
#include "EDM.h"
#include "EDMTarget.h"
#include "TDistConstants.h"
#include "TALGCConverter.h"

class EDMConverter : public TALGCConverter
{
public:

	EDMConverter(TAStreamFormatter& s) : TALGCConverter(s) { }

	EDM* readEDM(int& lineNumber, int& nextUnknownsNumber);

private:

	void readEDMTargets(EDM* edm, const string& defaultTargetID, int& nextUnknownsNumber);
	
};

#endif
