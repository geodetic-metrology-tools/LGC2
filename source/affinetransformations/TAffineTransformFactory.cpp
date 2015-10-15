#include <TAffineTransformFactory.h>
#include "Global.h"

THelmertTransformation
TAffineTransformFactory::createRotation(const std::string& name, const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz) 
{
	THelmertTransformation t(name);
	t.setRotationPart(rx,ry,rz, kRzyx);
	return t;
}

THelmertTransformation
TAffineTransformFactory::createTranslation(const std::string& name, const TReal tx, const TReal ty, const TReal tz)
{
	THelmertTransformation t(name); 
	t.setTranslationPart(tx, ty, tz);
	return t;

}

THelmertTransformation
TAffineTransformFactory::create6Parameter(const std::string& name, const TReal tx, const TReal ty, const TReal tz, 
		                 const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz)
{
	THelmertTransformation t(name); 
	t.setRotationPart(rx,ry,rz, kRzyx);
	t.setTranslationPart(tx, ty, tz);
	return t;
}

THelmertTransformation
TAffineTransformFactory::create7Parameter(const std::string& name, const TReal tx, const TReal ty, const TReal tz, 
		                 const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz, TReal scale)
{
	THelmertTransformation t(name); 
	t.setRotationPart(rx, ry, rz, kRzyx);
	TReal scaleInv = 1.0/scale;
	t.setTranslationPart(scaleInv*tx, scaleInv*ty, scaleInv*tz);
	zerofy(scale);
	t.setScaleFactor(scale);
	return t;
}

THelmertTransformation 
TAffineTransformFactory::createScaling(const std::string& name, const TReal& scale){
	THelmertTransformation t(name); 
	t.setScaleFactor(scale);
	return t;
}




