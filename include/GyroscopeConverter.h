#ifndef SU_GYROSCOPE_CONVERTER
#define SU_GYROSCOPE_CONVERTER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include "TAStreamFormatter.h"
#include "Gyroscope.h"
#include "TAngleConstants.h"
#include "TALGCConverter.h"

class GyroscopeConverter : public TALGCConverter
{
public:

	GyroscopeConverter(TAStreamFormatter& s) : TALGCConverter(s) { }

	Gyroscope* readGyroscope();
	
};

#endif
