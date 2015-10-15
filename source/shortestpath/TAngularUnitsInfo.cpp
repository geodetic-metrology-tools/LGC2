#include <TAngularUnitsInfo.h>

#include <stdexcept>
#include <memory> // for std::auto_ptr
#include <utility> // for std::make_pair

const TAngularUnitsInfo::AngleUnitsMapType & TAngularUnitsInfo::getAngleUnitsMap()
{
	typedef std::auto_ptr<AngleUnitsMapType> MapPtr;
	static MapPtr mapping;

	if(mapping.get()==NULL)
	{
		MapPtr tmp = MapPtr(new AngleUnitsMapType);
		tmp->insert(std::make_pair(0, std::make_pair("kRadians", "Radians")));
		tmp->insert(std::make_pair(1, std::make_pair("kGons", "Gons")));
		tmp->insert(std::make_pair(2, std::make_pair("k100MicroGons", "100MicroGons")));
		tmp->insert(std::make_pair(3, std::make_pair("kDMS", "DMS")));
		tmp->insert(std::make_pair(4, std::make_pair("kCCs", "CCs")));

		mapping = tmp;
	}
	return *mapping;
}

bool TAngularUnitsInfo::isValidNumber (int number)
{
	AngleUnitsMapType::const_iterator it = getAngleUnitsMap().find(number);
	return it != getAngleUnitsMap().end();

}

TAngle::EUnits TAngularUnitsInfo::fromString (const std::string & unitsName)
{
	AngleUnitsMapType::const_iterator it = getAngleUnitsMap().begin();
	while(it != getAngleUnitsMap().end())
	{
		if (unitsName==it->second.first)
			return static_cast<TAngle::EUnits>(it->first);
		++it;
	}
	throw std::invalid_argument("Unknown Angular Units");

}

std::string TAngularUnitsInfo::toString (TAngle::EUnits number)
{ 
	AngleUnitsMapType::const_iterator it = getAngleUnitsMap().find(number);
	if(it != getAngleUnitsMap().end())
		return (it->second.first);
	throw std::invalid_argument("Unknown Angular Units");

}

std::string TAngularUnitsInfo::toUserFrendlyString (TAngle::EUnits number)
{ 
	AngleUnitsMapType::const_iterator it = getAngleUnitsMap().find(number);
	if(it != getAngleUnitsMap().end())
		return (it->second.second);
	throw std::invalid_argument("Unknown Angular Units");

}