#ifndef TTRANSFORMATION
#define TTRANSFORMATION

#include "TFreeVector.h"
#include "TPositionVector.h"
#include "TRotationMatrix.h"

#include <Eigen/Dense>

struct TransformParameters;

class TTransformation{

public:
	
	friend class TCCS2CGRFTransformation;
	friend class TCGRF2ILGTransformation;
	friend class TILG2ILATransformation;
	friend class TLOR2LOR;
	/**@name  Constructors and destructors */
	//@{
		/// Creates an identity transformation.
		explicit TTransformation();

		///Copy constructor
		TTransformation(const TTransformation &transformation); 
		
		//! Destructor
		~TTransformation();
	//@}


		/**@name Setting Member Functions */
	//@{
		void setIdentityTransformation();

		///Sets the rotation part of the transformation matrix, needs to be overriden in derived classes (MAKE IT VIRTUAL!!!!!!)
		void setRotationTransformation(TReal rx, TReal ry, TReal rz);
	//@}

		/**@name Getting Member Functions */
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

		// Transforms the POSITION VECTOR by this transformation
		bool transform(TPositionVector& pv) const;

		/// Transforms the FREE VECTOR by this transformation
		bool transform(TFreeVector& fv) const;

		/// Multiplication operator for chaining transformations
		TTransformation operator*(const TTransformation& trans) const;
		TTransformation& operator*=(const TTransformation& trans);

		/// These operators transform vectors
		TPositionVector operator*(const TPositionVector& pos) const;
		TFreeVector operator*(const TFreeVector& pos) const;

		//! Copy Assignment Operator 
		TTransformation& operator=(const TTransformation& rhs);

		/*!
			Returns transformation which transformation matrix is inverted.
		*/
		TTransformation getInversedTransformation()const;


		static TTransformation getIdentity() ;
	//@}

protected:
	// Transformation matrix in homogeneous coordinates
	std::unique_ptr<Eigen::Matrix4d> fTransM;

	///Sets (i,j) position in matrix
	void setMatrixIJPosition(int row, int column, TReal value);
	
	///Sets zero matrix
	void TTransformation::setZeroMatrix();

};


#endif