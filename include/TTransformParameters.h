#ifndef TTRANSFORMATION_PARAMETERS
#define TTRANSFORMATION_PARAMETERS

#include "TLGCAngle.h"
#include "TFreeVector.h"

/// transformation parameters for the Helmert transformation
struct TransformParameters{
	
	// 3 rotation angles
	LGC::TAngle omega; 
	LGC::TAngle phi; 
	LGC::TAngle kappa; 
	// 3 translations
	TReal t1;
	TReal t2;
	TReal t3;
	// scale factor
	TReal scale;

	TransformParameters() : omega(LGC::TAngle()), phi(LGC::TAngle()), kappa(LGC::TAngle()), t1(TReal(0.0)), t2(TReal(0.0)), t3(TReal(0.0)), scale(TReal(1.0)) {};

	bool operator==(const TransformParameters& p) {
		return (omega==p.omega && phi==p.phi && kappa==p.kappa && t1==p.t1 && t2==p.t2 && t3==p.t3 && scale==p.scale);
	}

	void setIdentity() {
		omega = phi = kappa = LGC::TAngle();
		t1 = t2 = t3 = TReal(0.0);
		scale = TReal(1.0);
	}
};

#endif;