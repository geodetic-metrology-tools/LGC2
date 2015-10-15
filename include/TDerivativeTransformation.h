#ifndef TDERIVATIVE_TRANSFORMATION
#define TDERIVATIVE_TRANSFORMATION

#include "TFreeVector.h"
#include "TPositionVector.h"

#include <Eigen/Dense>

/*!
	This class represents a partial derivative transformation. 
	The functionalities are limited according to the fact that this object is used to transform a point or a vector into a freevector.
*/
class TDerivativeTransformation
{
public:

	TDerivativeTransformation();
	TDerivativeTransformation(const Eigen::Matrix4d& mat);
	~TDerivativeTransformation() {};

	/// Matrix operations
	Eigen::Matrix4d getMatrix() const;
	void setMatrix(const Eigen::Matrix4d& mat);
	TReal getMmatrixIJPosition(int row, int column) const;

	/// operator which transform a point into a FreeVector
	TFreeVector operator*(const TPositionVector& pos) const;

	/// Transformation
	TFreeVector transform(const  TPositionVector& rm) const;

private:
	
	// Transformation matrix in homogeneous coordinates
	Eigen::Matrix4d fTransM;
};

#endif
