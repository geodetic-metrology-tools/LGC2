// TTranslation.h
//
/** A translation transformation*/
//
// Patterns:
//
// 
// Copyright 2000-10 CERN SU, M.Jones. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_TRANSLATION_TRANSFO
#define SU_TRANSLATION_TRANSFO


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  "TAAffineTransformation.h"
#include  "TFreeVector.h"
class  TCompositeAffTransform;
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Class for a translation
class  TTranslation : public TAAffineTransformation  
{
public:
	//constants

	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor , create a translation of vector (0,0,0)
		TTranslation();

		/// Translation taking 3 length as Tx, Ty, Tz
		TTranslation(const TScalar Tx, const TScalar Ty, const TScalar Tz);

		/// Translation taking a free vector
		TTranslation(TFreeVector);

		/// Copy Constructor 
		TTranslation(const  TTranslation&);

		/// Destructor
		virtual  ~TTranslation();
	//@}


	/**@name Member Functions */
	//@{
		using TAAffineTransformation::operator();
		using TAAffineTransformation::transform;

		//! Copy Assignment Operator 
		TTranslation& operator= ( const TTranslation & );

		//! add two translation vectors
		TTranslation  operator+ ( const TTranslation & );

		/*//! Return element i
		TReal       operator[](int ) const;

		//! Return element i
		TReal&      operator[](int );*/

		//! Return the translation vector
		TFreeVector   getVector() const;

		//! Create a composite transformation by applying this transformation to an affine transformation
		//virtual  TCompositeAffTransform operator() ( const TAAffineTransformation & ) const;

		//! Return a pointer to a clone of this transformation
		virtual  TTranslation *  clone() const;

		//! Transform a position vector 
		virtual  bool transform( TPositionVector & ) const;

		//! Transform a free vector
		virtual  bool transform( TFreeVector & ) const;

		//! Transform a rotation matrix
		virtual  bool transform( TRotationMatrix & ) const;

		//! apply this transformation to a position vector 
		virtual  TPositionVector &  operator() ( TPositionVector & ) const;

		//! apply this transformation to a free vector 
		virtual  TFreeVector &  operator() ( TFreeVector & ) const;

		//! apply this transformation to a Rotation Matrix 
		virtual  TRotationMatrix &  operator() ( TRotationMatrix & ) const;

		//! Return a pointer to the inverse of this transformation
		virtual  TTranslation * inverse() const;

		//! Invert the transformation, replaces the current transformation parameters
		virtual  void invert();
	//@}


private:
	TFreeVector fTranslationVector;

	//ClassDef(TTranslation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////

inline TFreeVector TTranslation::getVector() const {return fTranslationVector;}



#endif // SU_TRANSLATION_TRANSFO
