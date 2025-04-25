/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/
#ifndef TTRANSFORMATION
#define TTRANSFORMATION

//SURVEYLIB
#include <TFreeVector.h>
#include <TPositionVector.h>
#include <TRotationMatrix.h>
#include "TTransformParameters.h"
//LGC
#include <Global.h>
//OTHER
#include <Eigen/Dense>

/*!
	\ingroup LocalTransformations
	\brief General representation of Helmert transformation, which enables to transform TPositionVector or TFreeVector. The rotation order used is Rxyz = Rx*Ry*Rz.
*/
class TTransformation{

public:
	

	/**@name  Constructors and destructors */
	//@{
		/// Creates an identity transformation.
		explicit TTransformation();

		///Copy constructor
		TTransformation(const TTransformation &transformation); 
		
		/// Destructor
		~TTransformation();
	//@}


	/**@name Set methods */
	//@{
		 /// Sets the identity transformation.
		 virtual void setIdentityTransformation();

		 /// Sets / Resets the rotation part of the transformation matrix. Value given in radians [rad].
		 virtual void setRotationTransformation(TReal rx, TReal ry, TReal rz);

		 /// Sets / Resets the translation, rotation and scale factor part of the transformation matrix. Distances in meters [m], angles in radians [rad].
		 virtual  void setTransformation(TReal tx, TReal ty, TReal tz, TReal rx, TReal ry, TReal rz, TReal scale);

		 /// Sets (i,j) position in the transformation matrix.
		void setMatrixIJPosition(int row, int column, TReal value);
	
		/// Sets zero matrix
		void setZeroMatrix();
	//@}

	/**@name Get methods */
	//@{
		/// Returns element on (row,column) position
		TReal getMmatrixIJPosition(int row, int column) const;

		/// Returns element on (row,column) position
		const Eigen::Matrix4d& getMatrix() const ;

		/// Returns a scale factor of this transformation
		TReal getScaleFactor() const {return 1/getMmatrixIJPosition(3,3);}
	//@}

	/**@name  Transformation methods */
	//@{	

		/// Transforms the POSITION VECTOR by this transformation
		bool transform(TPositionVector& pv) const;

		/// Transforms the FREE VECTOR by this transformation
		bool transform(TFreeVector& fv) const;

		/// Multiplication operator for Tranformations, it is the multiplication of the transformation matrices, enabling to chain the transformations.
		TTransformation operator*(const TTransformation& trans) const;
		/// Multiplication operator for Tranformations, it is the multiplication of the transformation matrices, enabling to chain the transformations.
		TTransformation& operator*=(const TTransformation& trans);


		/// Multiplication operator on TPositionVector
		TPositionVector operator*(const TPositionVector& pos) const;
		/// Multiplication operator on TFreeVector
		TFreeVector operator*(const TFreeVector& pos) const;

		/// Copy Assignment Operator 
		TTransformation& operator=(const TTransformation& rhs);

		///	Returns transformation which is the inverse of this transformation, i.e. its transformation matrix is inverted.
		TTransformation getInversedTransformation()const;

		/// Returns An identity transformation
		static TTransformation getIdentity() ;

		// get the Helmert transformation parameters
		TransformParameters getTrafoParameters() const;
	//@}

protected:
	/// Transformation matrix in homogeneous coordinates, representing the transformation.
	std::unique_ptr<Eigen::Matrix4d> fTransM;

	

};


#endif