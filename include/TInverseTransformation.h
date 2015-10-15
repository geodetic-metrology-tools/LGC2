#ifndef TINVERSE_TRANSFORMATION
#define TINVERSE_TRANSFORMATION

#include "TTransformation.h"
#include "TDerivativeTransformation.h"
#include "TTransformParameters.h"
#include "Global.h"


class TInverseTransformation : public TTransformation{

public:
	/// Creates an identity transformation with a given name, must have one.
	explicit TInverseTransformation();
	TInverseTransformation(const TransformParameters &fParams);
	///Copy constructor
	TInverseTransformation(const TTransformation &transformation);
	///Destructor
	~TInverseTransformation();


	/// Setter
	//void setAngle(int axis, const LGC::TAngle& angle);
	void setMatrix(const TransformParameters& params);
	void setTransformParam(const TransformParameters& params){ fEstimatedParams = params;}

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
	void setTranslationPart(const TransformParameters& params);
	void setRotationPart(const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz);
	void setScaleFactor(TReal scale);
	
};

#endif