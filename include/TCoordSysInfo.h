#ifndef CSGEOCORE_T_COORD_SYS_INFO_H_53156EB3D1184aebA2360100FCEE1F80
#define CSGEOCORE_T_COORD_SYS_INFO_H_53156EB3D1184aebA2360100FCEE1F80

#include <TCoordSysFactory.h>
#include <map>
#include <string>

class TCoordSysInfo
{
	TCoordSysInfo();
	TCoordSysInfo(const TCoordSysInfo & other); // non construction-copyable
    const TCoordSysInfo & operator=(const TCoordSysInfo &); // non copyable

	//typedef std::map<int, std::string> CoordMapType;

	typedef std::map<int, std::pair<std::string, std::string>> CoordMapType;

	static const CoordMapType & getCoordSysMap();


public:
	static bool isValidNumber (int system);
	static TCoordSysFactory::ECoordSys fromString (const std::string & systemName);
	static std::string toString (TCoordSysFactory::ECoordSys number);
	static std::string toUserFrendlyString (TCoordSysFactory::ECoordSys number);

};

#endif