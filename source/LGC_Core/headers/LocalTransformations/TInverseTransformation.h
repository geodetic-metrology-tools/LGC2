#ifndef TINVERSE_TRANSFORMATION
#define TINVERSE_TRANSFORMATION

#include "TTransformation.h"
#include "TDerivativeTransformation.h"
#include "TTransformParameters.h"

/*!
	\ingroup LocalTransformations
	\brief Class representing a STEP DOWN in the tree, this is the inverse of the TDirectTransformation. 
	Partial derivatives are given in terms of the transformation paramters of the direct transformation.
*/
class TInverseTransformation : public TTransformation{

public:
	/*!@name Constructors/Destructor*/
	//@{
		explicit TInverseTransformation();
		TInverseTransformation(const TransformParameters &fParams);
		TInverseTransformation(const TTransformation &transformation);
		~TInverseTransformation();
	//@}

	/*!@name Access methods */
	//@{
		TReal getScaleFactor() const;
		const TAngle& getAngle(int axis) const;
		TReal getTranslation(int axis) const;
		const TransformParameters& getTransformParam() const {return fParameters;}

		TDerivativeTransformation differentiatedTransformationAngle(int angle) const;
		TDerivativeTransformation differentiatedTransformationTranslation(int ti) const;
		TDerivativeTransformation differentiatedTransformationScaleFactor() const;
	//@}

	/*!@name Set methods */
	//@{
			/// Sets the transformation parameters
			void setTransformParam(const TransformParameters& params);

			/// Sets the identity transformation.
			virtual void setIdentityTransformation();

			/// Sets / Resets the rotation part of the transformation matrix. Value given in radians [rad].
			virtual void setRotationTransformation(TReal rx, TReal ry, TReal rz);

			/// Sets / Resets the translation, rotation and scale factor part of the transformation matrix. Distances in meters [m], angles in radians [rad].
			virtual  void setTransformation(TReal tx, TReal ty, TReal tz, TReal rx, TReal ry, TReal rz, TReal scale);
	//@}

	/// Transform a rotation matrix
	bool transform(TRotationMatrix& rm) const;

private:
	TransformParameters fParameters;

	void setIdentityParams();
	void setTranslationPart(const TransformParameters& params);
	void setRotationPart(const TAngle& rx, const TAngle& ry, const TAngle& rz);
	void setScaleFactor(TReal scale);
	
};

#endif