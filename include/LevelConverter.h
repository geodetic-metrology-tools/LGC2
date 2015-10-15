#ifndef SU_LEVEL_CONVERTER
#define SU_LEVEL_CONVERTER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include "TAStreamFormatter.h"
#include "Level.h"
#include "Staff.h"
#include "TDistConstants.h"
#include "TALGCConverter.h"

class LevelConverter : public TALGCConverter
{
public:

	LevelConverter(TAStreamFormatter& s) : TALGCConverter(s) { }

	Level* readLevel(int& lineNumber, int& nextUnknownsNumber);

private:

	void readLevelTargets(Level* level, const string& defaultStaffID, int& nextUnknownsNumber);
	
};

#endif
