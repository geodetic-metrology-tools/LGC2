#include "AffineTransformFactory.h"

namespace {
	// sets the value excatly to zero if it is very small
	inline void zerofy(TReal& v) {
		if (fabsq(v) < 2*EPSILON)
			v = 0.0;
	}
}

THelmertTransformation
AffineTransformFactory::createRotation(const std::string& name, const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz) 
{
	THelmertTransformation t(name);
	t.setRotationPart(rx,ry,rz);
	return t;
}

THelmertTransformation
AffineTransformFactory::createTranslation(const std::string& name, const TReal tx, const TReal ty, const TReal tz)
{
	THelmertTransformation t(name); 
	t.setTranslationPart(tx, ty, tz);
	return t;

}

THelmertTransformation
AffineTransformFactory::create6Parameter(const std::string& name, const TReal tx, const TReal ty, const TReal tz, 
		                 const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz)
{
	THelmertTransformation t(name); 
	t.setRotationPart(rx,ry,rz);
	t.setTranslationPart(tx, ty, tz);
	return t;
}

THelmertTransformation
AffineTransformFactory::create7Parameter(const std::string& name, const TReal tx, const TReal ty, const TReal tz, 
		                 const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz, TReal scale)
{
	THelmertTransformation t(name); 
	t.setRotationPart(rx,ry,rz);
	t.setTranslationPart(tx, ty, tz);
	zerofy(scale);
	t.setScaleFactor(scale);
	return t;
}

THelmertTransformation 
AffineTransformFactory::createScaling(const std::string& name, const TReal& scale){
	THelmertTransformation t(name); 
	t.setScaleFactor(scale);
	return t;
}
