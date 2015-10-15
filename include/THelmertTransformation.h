/////////////////////////////////////////////////////////////////////////////
//THelmertTransformation.h
//
/** Class for Helmert transformation X1 = Fact*R*X+T, performed in homogeneous coordinates.*/
//
// Copyright 2000-2014 CERN SU, M.Jones. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef SU_HELMERT_TRANSFORMATION
#define SU_HELMERT_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLGCAngle.h"
#include "TFreeVector.h"
#include "TPositionVector.h"
#include "TRotationMatrix.h"


enum ERotationType {kRzyx, kRyxz}; 

//! structure for 3 rotation angles
struct RotAngles{
	LGC::TAngle omega;
	LGC::TAngle phi;
	LGC::TAngle kappa;
	ERotationType kR; //Rotation type
};

/*!
	Represents a 7-parameter transformation for cartesian coordinate systems.
	It efficiently treats pure rotations and pure translations as well.

	\note The order of rotations is ZYX like it was in previous versions of LGC
*/
class  THelmertTransformation 
{
public:
		friend class TAffineTransformFactory;
		friend class TCCS2CGRFTransformation;
		friend class TCGRF2ILGTransformation;
		friend class TILG2ILATransformation;

	 //Line in the input file where the tranformation was introduced
		int line;

	/**@name  Constructors and destructors */
	//@{
		/// Creates an identity transformation with a given name, must have one.
		explicit THelmertTransformation(const std::string& name);

		///Copy constructor
		THelmertTransformation(const THelmertTransformation &transformation); 
		
		//! Destructor
		~THelmertTransformation();
	//@}


	/**@name Setting Member Functions */
	//@{
		/// Adjustables are required to have a name
		void setName(const std::string& name);
		
		/// Set/update the scale factor of the transformation
		void setScaleFactor(TReal scale);

		/*! Update one of the three rotation angles.
		
			\param axis  Goes from 0(X-axis) to 2(Z-axis).
			\param angle The updated angle which is copied internally
		*/
		void setAngle(int axis, const LGC::TAngle& angle);

		/// Set the translation along an axis from 0(X-axis) to 2(Z-axis).
		void setTranslation(int axis, TReal t);

		void setIdentityTransformation();

	//@}

	/**@name Getting Member Functions */
	//@{
		/// Adjustables are required to have a name
		const std::string& getName() const;

		/// Get the scale factor of the transformation
		TReal getScaleFactor() const;

		/*! Get one of the three rotation angles.
		
			\param axis Goes from 0(X-axis) to 2(Z-axis).
		*/
		LGC::TAngle getAngle(int axis) const;


		RotAngles getAngles() const;


		/// Get the translation along an axis from 0(X-axis) to 2(Z-axis).
		TReal getTranslation(int axis) const;

		///Returns element on (row,column) position
		TReal getMmatrixIJPosition(int row, int column) const;
	//@}
		
		
	/**@name  Transformation methods */
	//@{	

		//! Return a pointer to a clone of this transformation
		THelmertTransformation*  clone() const;

		/// Transforms the POSITION VECTOR by this transformation
	    bool transform(TPositionVector& pv) const;

		/// Transforms the FREE VECTOR by this transformation
		bool transform(TFreeVector& fv) const;

		//! Transform a Rotation Matrix
		bool transform( TRotationMatrix& rm) const;

	   //! apply this transformation to a position vector 
		//TPositionVector &  operator() ( TPositionVector & ) const;

		//! apply this transformation to a free vector 
		//TFreeVector &  operator() ( TFreeVector & ) const;

		//! apply this transformation to a Rotation Matrix 
		//TRotationMatrix &  operator() ( TRotationMatrix & ) const;

		/// inverts the transformation and returns  itself
		THelmertTransformation& invert();

		/// returns a new object that is the inverse of this transformation
		inline THelmertTransformation inverse() const;

		/// Multiplication operator for chaining transformations
		THelmertTransformation operator*(const THelmertTransformation& trans);
			
		//! Copy Assignment Operator 
		THelmertTransformation& operator=(const THelmertTransformation& rhs);
//@}

	/**@name  Transformation methods */
	//@{	
		/*!
			Partial derivatives with respect to x0 (x coordinate of the point in the original LOR (local object reference)) .

			\param[in] pos Coordinate of the transformed point of which we want the derivatives, point is in homogeneous coordinates (0[X], 1[Y], 2[Z], 3[W]). 

			The partial derivative obtained is therefore: pos = 0 =>  (d X)/(d x0); pos = 1 => (d Y)/(d x0); pos = 2 => (d Z)/(d x0)

			\returns Partial derivative with respect to x0 of the 'pos' coordinate of the point.
		*/
		inline TReal partDerivX0(int pos) const{
			assert4D(pos);
			return getScaleFactor() * getMmatrixIJPosition(pos,0);
		}
		/*!
			Partial derivatives with respect to y0 (y coordinate of the point in the original LOR (local object reference)) .

			\param[in] pos Coordinate of the transformed point of which we want the derivatives, point is in homogeneous coordinates (0[X], 1[Y], 2[Z], 3[W]). 

			The partial derivative obtained is therefore: pos = 0 =>  (d X)/(d y0); pos = 1 => (d Y)/(d y0); pos = 2 => (d Z)/(d y0)

			\returns Partial derivative with respect to x0 of the 'pos' coordinate of the point.
		*/
		inline TReal partDerivY0(int pos) const{
			assert4D(pos);
			return getScaleFactor() * getMmatrixIJPosition(pos,1);
		}
		/*!
			Partial derivatives with respect to z0 (z coordinate of the point in the original LOR (local object reference)) .

			\param[in] pos Coordinate of the transformed point of which we want the derivatives, point is in homogeneous coordinates (0[X], 1[Y], 2[Z], 3[W]). 

			The partial derivative obtained is therefore: pos = 0 =>  (d X)/(d z0); pos = 1 => (d Y)/(d z0); pos = 2 => (d Z)/(d z0)

			\returns Partial derivative with respect to z0 of the 'pos' coordinate of the point.
		*/
		inline TReal partDerivZ0(int pos) const{
			assert4D(pos); 
			return getScaleFactor() * getMmatrixIJPosition(pos,2);
		}
		/*!
			Partial derivatives with respect to angles.

			\param[in] angle Specification of the the angle (0 - omega (x-axis), 1 - phi (y-axis), 2 - kappa (z-axis)). With respect to this angle is returned partial derivative.

			\returns TLocalTransformation object which represents partial derivative of this transformation.
		*/
		THelmertTransformation differentiatedTransformationAngle(int angle) const;

		THelmertTransformation differentiatedTransformationAngleInverse(int ti) const;

		/*!
			Partial derivatives with respect to translations.

			\param[in] ti Specification of the translation part (0[t1], 1[t2], 2[t3]). With respect to this part is returned partial derivative.

			\returns TLocalTransformation object which represents partial derivative of this transformation.
		*/
		THelmertTransformation differentiatedTransformationTranslation(int ti) const;

		THelmertTransformation differentiatedTransformationTranslationInverse(int ti) const;

		/*!
			Partial derivatives with respect to scale factor.

			\returns TLocalTransformation object which represents partial derivative of this transformation.
		*/
		THelmertTransformation differentiatedTransformationScaleFactor() const;

		THelmertTransformation differentiatedTransformationScaleFactorInverse() const;

		//@}
private:

	/// PIMPL idom for private members
	struct D;
	std::unique_ptr<D> d;

	///Sets rotation part of the m matrix
	void setRotationPart(const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz, ERotationType rt);
	///Sets translation part of the m matrix
	void setTranslationPart(const TReal tx, const TReal ty, const TReal tz);
	///Sets (i,j) position in matrix
	void setMatrixIJPosition(int row, int column, TReal value);
	///Sets zero matrix
	void THelmertTransformation::setZeroMatrix();	
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////
inline THelmertTransformation THelmertTransformation::inverse() const {
			THelmertTransformation t(*this);
			t.invert();
			return t;
	}
		

#endif // SU_HELMERT_TRANSFORMATION