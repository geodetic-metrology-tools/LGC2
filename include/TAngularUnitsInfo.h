#ifndef CSGEOCORE_T_ANGULAR_UNITS_INFO_H_28966B862F2A46f4900219F55E582392
#define CSGEOCORE_T_ANGULAR_UNITS_INFO_H_28966B862F2A46f4900219F55E582392

#include "TAngle.h"
#include <map>
#include <string>

class TAngularUnitsInfo
{
	TAngularUnitsInfo();
	TAngularUnitsInfo(const TAngularUnitsInfo & other); // non construction-copyable
    const TAngularUnitsInfo & operator=(const TAngularUnitsInfo &); // non copyable

	//typedef std::map<int, std::string> AngleUnitsMapType;
	typedef std::map<int, std::pair<std::string, std::string>> AngleUnitsMapType;

	static const AngleUnitsMapType & getAngleUnitsMap();


public:
	static bool isValidNumber (int number);
	static TAngle::EUnits fromString (const std::string & unitsName);
	static std::string toString (TAngle::EUnits number);
	static std::string toUserFrendlyString (TAngle::EUnits number);

};

#endif