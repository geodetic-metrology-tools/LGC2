#ifndef CART2ELLIPS_H
#define CART2ELLIPS_H

#include "GeodeticConstants.h"
#include "Quad.h"

class Cartesian2Ellipsoidal{
public:
	static TReal getPhi(TReal X, TReal Y, TReal Z);
	static TReal getLambda(TReal X, TReal Y, TReal Z);
	static TReal getHeight(TReal X, TReal Y, TReal Z);
/*	static EllipsoidalCoord getEllipsoidalCoordinates(TReal X, TReal Y, TReal Z);*/
};


struct EllipsoidalCoord{
	TReal phi;
	TReal lambda;
	TReal height;
};


#endif