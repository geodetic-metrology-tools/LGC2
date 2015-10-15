// TScaleFactor.h
//
/** A Scaling Transformation */
//
// Patterns:
//
// 
// Copyright 2010 CERN SU, M.Jones. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_SCALE_FACTOR_TRANSFO
#define SU_SCALE_FACTOR_TRANSFO


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
class  TCompositeAffTransform;
//
// typedefs
#include  "TAAffineTransformation.h"
#include  "TScalar.h"
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Class for scaling
class  TScaleFactor : public TAAffineTransformation  
{
public:
		
	/**@name Constructors and Destructors */
	//@{
		//! Default Constructor, factor initialized to 1 
		TScaleFactor();

		//! Constructor taking a scale factor as a TReal
		explicit TScaleFactor(TReal);

		//! Constructor taking a scale factor as a TDouble
		explicit TScaleFactor(TScalar);

		//! Copy Constructor 
		TScaleFactor( const TScaleFactor & );

		//! Destructor
		virtual  ~TScaleFactor();
	//@}


	/**@name Member Functions */
	//@{
		using TAAffineTransformation::operator();
		using TAAffineTransformation::transform;

		//! Copy Assignment Operator 
		TScaleFactor & operator = ( const TScaleFactor & );

		//! Multiplication by another scaling transformation
		TScaleFactor operator * ( const TScaleFactor & );
		
		//! Set the factor of ScaleFactor
		void setScaleFactor( TReal );

		//! Set the factor of ScaleFactor
		void setScaleFactor( TScalar );

		//! Return the factor
		TScalar getScaleFactor() const;
		
		//! Create a composite transformation by applying this transformation to an affine transformation
		//virtual  TCompositeAffTransform operator() ( const TAAffineTransformation & ) const;

		//! Return a pointer to a clone of this transformation
		virtual  TScaleFactor *  clone() const;

		//! Transform a position vector
		virtual  bool transform( TPositionVector & )const;

		//! Transform a  free vector
		virtual  bool transform( TFreeVector & )const;

		//! Transform a rotation matrix
		virtual  bool transform( TRotationMatrix & ) const;

		//! apply this transformation to a position vector
		virtual  TPositionVector &  operator() ( TPositionVector & ) const;

		//! apply this transformation to a free vector
		virtual  TFreeVector &  operator() ( TFreeVector & ) const;

		//! apply this transformation to a Rotation Matrix
		virtual  TRotationMatrix &  operator() ( TRotationMatrix & ) const;

		//! Return a pointer to the inverse of this transformation
		virtual  TScaleFactor * inverse() const;

		//! Invert the transformation, replaces the current transformation parameters
		virtual  void invert();
	//@}


private:
	
		TScalar  fScaleFactor; /*!< scale factor */

	//ClassDef(TScaleFactor, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////

inline void TScaleFactor::setScaleFactor( TReal fact) {fScaleFactor.setValue( fact ); return;}

inline void TScaleFactor::setScaleFactor( TScalar fact) {fScaleFactor = fact; return;}

inline TScalar TScaleFactor::getScaleFactor() const { return fScaleFactor;}




#endif // SU_SCALE_FACTOR_TRANSFO
