#include <TLocalSystemOrigin.h>

TLocalSystemOrigin::TLocalSystemOrigin(const TSpatialPosition & origin, const TAngle & gisement, const TAngle & slope, const std::string & name)
: fOrigin(origin)
, fGisement(gisement)
, fSlope(slope)
, fName(name)
{
}

bool TLocalSystemOrigin::operator==(const TLocalSystemOrigin & rhs) const
{
    return fGisement==rhs.fGisement &&
        fSlope==rhs.fSlope&&
        fOrigin.getCoordinates(TCoordSysFactory::k3DCartesian)==rhs.fOrigin.getCoordinates(TCoordSysFactory::k3DCartesian)&&
		fName==rhs.fName;
}
