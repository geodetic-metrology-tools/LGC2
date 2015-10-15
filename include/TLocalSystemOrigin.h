#ifndef SPATIAL_OBJECTS_e62c6be5_6903_4e36_b58e_5dd8c858f5d2
#define SPATIAL_OBJECTS_e62c6be5_6903_4e36_b58e_5dd8c858f5d2

#include <TAngle.h>
#include <TSpatialPosition.h>
#include <TSpatialPoint.h>

class TLocalSystemOrigin
{
public:
	//@*@
	TLocalSystemOrigin(const TSpatialPosition & origin, const TAngle & gisement, const TAngle & slope, const std::string & name="ORIGIN");
    bool operator==(const TLocalSystemOrigin & rhs) const;
    
	//@*@
	TSpatialPosition origin() const {return fOrigin;}
    TAngle gisement() const {return fGisement;}
    TAngle slope() const {return fSlope;}
	const std::string & name() const {return fName;}
private:
	// Cannot copy const members, so assignment is private
	TLocalSystemOrigin& operator=(const TLocalSystemOrigin&);
    const TSpatialPosition fOrigin; 
    const TAngle fGisement; 
    const TAngle fSlope;
	const std::string fName;
};

#endif