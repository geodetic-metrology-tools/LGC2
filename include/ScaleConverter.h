#ifndef SU_SCALE_CONVERTER
#define SU_SCALE_CONVERTER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include "TAStreamFormatter.h"
#include "Scale.h"
#include "TDistConstants.h"
#include "TALGCConverter.h"

class ScaleConverter : public TALGCConverter
{
public:

	ScaleConverter(TAStreamFormatter& s) : TALGCConverter(s) { }

	Scale* readScale(int& nextUnknownsNumber);
	
};

#endif
