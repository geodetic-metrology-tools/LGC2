#include "Cartesian2Ellipsoidal.h"

    TReal Cartesian2Ellipsoidal::getLambda(TReal X, TReal Y, TReal Z){
		TReal xyDist = sqrtq(pow2(X) + pow2q(Y));
		TReal gamma = atanq( (Z * SemiMajorAxisGRS80 )/(xyDist*SemiMinorAxisGRS80));
		TReal secondEccentricity = eccentrGRS80 / (LITERAL(1.0) - eccentrGRS80);
		return atanq(Y/X);
	}

	TReal Cartesian2Ellipsoidal::getPhi(TReal X, TReal Y, TReal Z){
		TReal xyDist = sqrtq(pow2(X) + pow2q(Y));
		TReal gamma = atanq( (Z * SemiMajorAxisGRS80 )/(xyDist*SemiMinorAxisGRS80));
		TReal secondEccentricity = eccentrGRS80 / (LITERAL(1.0) - eccentrGRS80);
		TReal lambda = atanq(Y/X);
		return atanq((Z  + pow2q(secondEccentricity)*SemiMinorAxisGRS80*powq(sinq(gamma) ,3))/
			(xyDist  - eccentrGRS80*SemiMajorAxisGRS80 *  powq(cosq(gamma) ,3)));
	}