//  TAAffineTransformation.h
//
/** Classe de base pour une transformation affine
 
  
    Patterns:
  
    Copyright 2000-10 CERN SU, M.Jones. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_A_AFFINE_TRANSFORMATION
#define SU_A_AFFINE_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
class  TPositionVector;
class  TFreeVector;
class  TRotationMatrix;
class  TSpatialPosition;
class  TSpatialVector;
class  TSpatialOrientation;
class  TCompositeAffTransform;
//
// typedefs
#include  "TANumericValue.h"
#include  "TVAffineTransformation.h"
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup spatialobjects
	@{*/

//! Classe de base abstraite pour une transformation affine
class  TAAffineTransformation : public TANumericValue, public TVAffineTransformation
{
public:

	/// status enumeration
	//enum EStatus {kNull, kUnknown, kVariable, kFixed};

	/**@name Constructors/destructors */
	//@{
		TAAffineTransformation();
	
		virtual ~TAAffineTransformation();
	
	//@}

	/**@name Member Functions */
	//@{
		using TVAffineTransformation::operator();
		using TVAffineTransformation::transform;

		//! Create a composite transformation by applying this transformation to an affine transformation
		virtual  TCompositeAffTransform & operator() ( TCompositeAffTransform & ) const;

		//! Create a composite transformation by applying this transformation to an affine transformation
		virtual  TCompositeAffTransform operator() ( const TAAffineTransformation & ) const;

		//! Return a pointer to a clone of this transformation
		virtual  TAAffineTransformation*  clone() const = 0;
		
		//! transform a spatial position
		virtual  bool  transform( TSpatialPosition & ) const;

		//! transform a spatial vector 
		virtual  bool  transform( TSpatialVector & ) const;

		//! transform a spatial orientation 
		virtual  bool  transform( TSpatialOrientation & ) const;

		//! transform a position vector 
		virtual  bool  transform( TPositionVector & ) const = 0;

		//! transform a free vector 
		virtual  bool  transform( TFreeVector & ) const = 0;

		//! transform a Rotation Matrix 
		virtual  bool  transform( TRotationMatrix & ) const = 0;

		//! apply this transformation to a spatial position 
		virtual  TSpatialPosition &  operator() ( TSpatialPosition & ) const;

		//! apply this transformation to a spatial vector 
		virtual  TSpatialVector &  operator() ( TSpatialVector & ) const;

		//! apply this transformation to a spatial orientation 
		virtual  TSpatialOrientation &  operator() ( TSpatialOrientation & ) const;

		//! apply this transformation to a position vector 
		virtual  TPositionVector &  operator() ( TPositionVector & ) const = 0;

		//! apply this transformation to a free vector 
		virtual  TFreeVector &  operator() ( TFreeVector & ) const = 0;

		//! apply this transformation to a Rotation Matrix 
		virtual  TRotationMatrix &  operator() ( TRotationMatrix & ) const = 0;

		//! Return a pointer to the inverse of this transformation
		virtual TAAffineTransformation * inverse() const = 0;

		//! Invert the transformation, replaces the current transformation parameters
		//virtual void invert() = 0;
	//@}
};

/*@}*/


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_A_AFFINE_TRANSFORMATION
