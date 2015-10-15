#ifndef TDIRECT_TRANSFORMATION
#define TDIRECT_TRANSFORMATION

#include "TDerivativeTransformation.h"
#include "TTransformation.h"
#include "TTransformParameters.h"
#include "Global.h"

class TDirectTransformation : public TTransformation{
public:
	/// Creates an identity transformation with a given name, must have one.
	explicit TDirectTransformation();
	TDirectTransformation(const TransformParameters &fParams);
	TDirectTransformation(const TTransformation &transformation);
	///Destructor
	~TDirectTransformation();


	/// Setter
	//void setAngle(int axis, const LGC::TAngle& angle);
	//void setTranslation(int axis, TReal t);
	void setMatrix(const TransformParameters& params);
	void setTransformParam(const TransformParameters& params){ fEstimatedParams = params;};

	/// Getter
	TReal getScaleFactor() const;
	LGC::TAngle getAngle(int axis) const;
	TReal getTranslation(int axis) const;
	TransformParameters getTransformParam() const {return fEstimatedParams;}

	/// Derivatives
	TDerivativeTransformation differentiatedTransformationAngle(int angle) const;
	TDerivativeTransformation differentiatedTransformationTranslation(int ti) const;
	TDerivativeTransformation differentiatedTransformationScaleFactor() const;

	/// Transformation
	bool transform(TRotationMatrix& rm) const;

private:
	TransformParameters fEstimatedParams;

	void setIdentityParams();
	void setTranslationPart(const TReal tx, const TReal ty, const TReal tz);
	void setRotationPart(const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz);
	void setScaleFactor(TReal scale);
	
};

#endif
