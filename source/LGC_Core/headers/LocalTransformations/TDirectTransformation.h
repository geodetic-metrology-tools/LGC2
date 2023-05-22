/*
© Copyright CERN 2000-2019. All rights reserved. This software is released under a CERN proprietary software license.
Any permission to use it shall be granted in writing. Request shall be addressed to CERN through mail-KT@cern.ch
*/

#ifndef TDIRECT_TRANSFORMATION
#define TDIRECT_TRANSFORMATION

// LGC
#include <TDerivativeTransformation.h>
#include <TTransformParameters.h>
#include <TTransformation.h>

// SurveyLib
#include <TSparseMatrix.h>

/*!
	\ingroup LocalTransformations
	\brief Class representing a STEP UP in the tree of local frames, this is the transformation which is used to define a node of the tree with respect to its parent.
*/
class TDirectTransformation : public TTransformation
{
public:
	/*!@name Constructors/Destructor*/
	//@{
	explicit TDirectTransformation();
	TDirectTransformation(const TransformParameters &fParams);
	TDirectTransformation(const TTransformation &transformation);
	~TDirectTransformation();
	//@}

	/*!@name Access methods */
	//@{
	TReal getScaleFactor() const;
	const TAngle &getAngle(int axis) const;
	TReal getTranslation(int axis) const;
	const TransformParameters &getTransformParam() const { return fParameters; }

	TDerivativeTransformation differentiatedTransformationAngle(int angle) const;
	TDerivativeTransformation differentiatedTransformationTranslation(int ti) const;
	TDerivativeTransformation differentiatedTransformationScaleFactor() const;
	TDenseMatrix getPartialDerivativeWrtPosition() const;
	TDenseMatrix getPartialDerivativeWrtHelmertParameters(const TPositionVector &pos) const;
	//@}

	/*!@name Set methods */
	//@{
	/// Sets the transformation parameters
	void setTransformParam(const TransformParameters &params);

	/// Sets the identity transformation.
	virtual void setIdentityTransformation();

	/// Sets / Resets the rotation part of the transformation matrix. Value given in radians [rad].
	virtual void setRotationTransformation(TReal rx, TReal ry, TReal rz);

	/// Sets / Resets the translation, rotation and scale factor part of the transformation matrix. Distances in meters [m], angles in radians [rad].
	virtual void setTransformation(TReal tx, TReal ty, TReal tz, TReal rx, TReal ry, TReal rz, TReal scale);
	//@}

	/// Transform a rotation matrix
	bool transform(TRotationMatrix &rm) const;

private:
	TransformParameters fParameters;

	void setIdentityParams();
	void setTranslationPart(const TReal tx, const TReal ty, const TReal tz);
	void setRotationPart(const TAngle &rx, const TAngle &ry, const TAngle &rz);
	void setScaleFactor(TReal scale);
};

#endif
