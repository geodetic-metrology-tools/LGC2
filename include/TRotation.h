// TRotation.h
//
/** Class for rotations. Affine Transformation. Uses as default representation RzRyRx matrix
It authorizes the inverse rotation and the multiplication 
by an other affine transformation when possible.
 Sub classes for rotations around each axis*/
//
// Patterns:
//
// 
// Copyright 2000-2010 CERN SU, M.Jones. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_ROTATION_TRANSFO
#define SU_ROTATION_TRANSFO


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
class  TCompositeAffTransform;
//
// typedefs
#include  "TAAffineTransformation.h"
#include  "TRotationMatrix.h"
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Class for rotations. Affine Transformation
class  TRotation : public TAAffineTransformation  
{
public:
	//constants

	/**@name Constructors and Destructors */
	//@{
		//! Default Constructor, initialize the matrix to identity
		TRotation();

		/** Constructor taking the rotation matrix*/  
		TRotation(TRotationMatrix matrix);
		
		/** Constructor taking the radians value of the angles in the specified order */  
		TRotation(TRotationMatrix::ERotationType kR, TReal omega, TReal phi, TReal kappa);

		//! Copy Constructor 
		TRotation(const  TRotation&);

		/// Destructor
		virtual  ~TRotation();
	//@}


	/**@name Member Functions */
	//@{
		using TAAffineTransformation::operator();
		using TAAffineTransformation::transform;

		//! Copy Assignment Operator 
		TRotation & operator = ( const TRotation& );

		//! Multiplication by another rotation
		TRotation operator * ( const TRotation & );
		
		//! Return element ri, cj of the rotation matrix
		TReal operator()(int row, int col) const;

		//! set the 3 rotations taking angles values in the order specified by the enumerator
		void setAllRotations(TRotationMatrix::ERotationType kR, TReal omega, TReal phi, TReal kappa);

		//! return the rotation matrix as TRotationMatrix
		TRotationMatrix getRotationMatrix() const;

		//! calculate the angles from the matrix with the specified rotation order
		struct Angles getAngles(TRotationMatrix::ERotationType kR);

		//! Create a composite transformation by applying this transformation to an affine transformation
		//virtual  TCompositeAffTransform operator() ( const TAAffineTransformation & ) const;// {return TAAffineTransformation::operator ()(trans);}

		//! Return a pointer to a clone of this transformation
		virtual  TRotation *  clone() const;

		//! Transform a position vector
		virtual  bool transform( TPositionVector & ) const;

		//! Transform a free vector
		virtual  bool transform( TFreeVector & ) const;

		//! Transform a rotation matrix
		virtual  bool  transform( TRotationMatrix & ) const;

		//! apply this transformation to a position vector
		virtual  TPositionVector &  operator() ( TPositionVector & ) const;

		//! apply this transformation to a free vector
		virtual  TFreeVector &  operator() ( TFreeVector & ) const;

		//! apply this transformation to a Rotation Matrix
		virtual  TRotationMatrix &  operator() ( TRotationMatrix & ) const;

		//! Return a pointer to the inverse of this transformation
		virtual  TRotation *  inverse() const;

		//! Invert the transformation, replaces the current transformation parameters
		virtual  void invert();

	//@}
		

private:

	//! fill the matrix of rotation with the specified type
	void fillRotationMatrix(TRotationMatrix::ERotationType kR, TReal om, TReal p, TReal k);

	//! Member Attributs
	TRotationMatrix		fRotationMatrix; /*!< rotation matrix */
	
	//ClassDef(TRotation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////

inline TRotationMatrix TRotation::getRotationMatrix() const {return fRotationMatrix;}


//////////////////////////////////////////////////////////////////////
// Sous-classes pour rotations autour d'un seul axe
//////////////////////////////////////////////////////////////////////


// rotations autour de l'axe X

class TXAxisRotation : public TRotation
{
public:
	
	//! Constructor
	TXAxisRotation(const TReal& omega):TRotation(TRotationMatrix::kRzyx, omega, LITERAL(0.0), LITERAL(0.0)){};

	/// Destructor
	virtual ~TXAxisRotation(){};
};


// rotations autour de l'axe Y

class TYAxisRotation : public TRotation
{
public:
	
	//! Constructor
	TYAxisRotation(const TReal& phi):TRotation(TRotationMatrix::kRzyx, LITERAL(0.0), phi, LITERAL(0.0)){};

	/// Destructor
	virtual ~TYAxisRotation(){};
};


// rotations autour de l'axe Z

class TZAxisRotation : public TRotation
{
public:
	
	//! Constructor
	TZAxisRotation(const TReal& kappa):TRotation(TRotationMatrix::kRzyx, LITERAL(0.0), LITERAL(0.0), kappa){};

	/// Destructor
	virtual ~TZAxisRotation(){};
};

#endif // SU_ROTATION_TRANSFO

