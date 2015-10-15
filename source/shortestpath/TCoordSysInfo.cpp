#include <TCoordSysInfo.h>

#include <stdexcept>
#include <memory> // for std::auto_ptr
#include <utility> // for std::make_pair

const TCoordSysInfo::CoordMapType & TCoordSysInfo::getCoordSysMap()
{
	typedef std::auto_ptr<CoordMapType> MapPtr;
	static MapPtr mapping;

	if(mapping.get()==NULL)
	{
		MapPtr tmp = MapPtr(new CoordMapType);
		tmp->insert(std::make_pair(1, std::make_pair("k3DCartesian", "3DCartesian")));
		tmp->insert(std::make_pair(2, std::make_pair("kGeodetic", "Geodetic")));
		tmp->insert(std::make_pair(4, std::make_pair("k2DPlusH", "2DPlusH")));
		tmp->insert(std::make_pair(8, std::make_pair("k2DCartesian", "2DCartesian")));
		tmp->insert(std::make_pair(16, std::make_pair("kGeodeticSphere", "GeodeticSphere")));

		mapping = tmp;
	}
	return *mapping;
}

bool TCoordSysInfo::isValidNumber (int system)
{
	CoordMapType::const_iterator it = getCoordSysMap().find(system);
	return it != getCoordSysMap().end();

}

TCoordSysFactory::ECoordSys TCoordSysInfo::fromString (const std::string & systemName)
{
	CoordMapType::const_iterator it = getCoordSysMap().begin();
	while(it != getCoordSysMap().end())
	{
		if (systemName==it->second.first)
			return static_cast<TCoordSysFactory::ECoordSys>(it->first);
		++it;
	}
	
	throw std::invalid_argument("Unknown Coordinate System");

}

std::string TCoordSysInfo::toString (TCoordSysFactory::ECoordSys number)
{
	CoordMapType::const_iterator it = getCoordSysMap().find(number);
	if(it != getCoordSysMap().end())
		return (it->second.first);

	throw std::invalid_argument("Unknown Coordinate System");

}

std::string TCoordSysInfo::toUserFrendlyString (TCoordSysFactory::ECoordSys number)
{
	CoordMapType::const_iterator it = getCoordSysMap().find(number);
	if(it != getCoordSysMap().end())
		return (it->second.second);

	throw std::invalid_argument("Unknown Coordinate System");

}

