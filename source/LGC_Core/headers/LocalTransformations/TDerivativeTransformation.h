/*
 * SPDX-FileCopyrightText: 2025 CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TDERIVATIVE_TRANSFORMATION
#define TDERIVATIVE_TRANSFORMATION


//STL
#include <memory>
//SURVEYLIB
#include <TFreeVector.h>
//OTHER
#include <Eigen/Dense>

class TPositionVector;

/** 
	\ingroup LocalTransformations
	\brief This class represents a partial derivatives of a transformation (TDirectTransformation or TInverseTransformation).

	The multiplication operator enables to multiply the differentiated transformation matrix with a TPositionVector. The result of this operation is a TFreeVector,
	because after this multiplication is applied, the Point (TPositionVector) is no longer a point, but vector of partial derivatives, represented by TFreeVector.
*/
class TDerivativeTransformation
{
public:
	/// Constructor
	TDerivativeTransformation();

	/// Copy constructor
	TDerivativeTransformation(const TDerivativeTransformation &derivTransform); 

	//Constructor
	TDerivativeTransformation(const Eigen::Matrix4d& mat);

	/// Destructor
	~TDerivativeTransformation() {};

	TDerivativeTransformation& operator=(const TDerivativeTransformation& rhs);

	/// Return a constant reference on the matrix
	const Eigen::Matrix4d& getMatrix() const;

	/// Sets the matrix
	void setMatrix(const Eigen::Matrix4d& mat);

	/*! 
		\brief Returns one element of the matrix

		\param[in] row Row position of the element
		\param[in] column Column position of the element
	*/
	TReal getMmatrixIJPosition(int row, int column) const;

	/// multiplication operator which transform a Point (TPositionVector) into a vector (TFreeVector)
	TFreeVector operator*(const TPositionVector& pos) const;

	/// transformation function which transform a Point (TPositionVector) into a vector (TFreeVector), does the same as multiplication operator
	TFreeVector transform(const  TPositionVector& rm) const;

private:
	
	// Transformation matrix in homogeneous coordinates
	std::unique_ptr<Eigen::Matrix4d> fTransM;
};

#endif
