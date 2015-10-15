//  TVAffineTransformation.h
//
/** Classe de base purement abstraite pour une transformation affine
 
  
    Patterns:
  
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_V_AFFINE_TRANSFORMATION
#define SU_V_AFFINE_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//
class  TPositionVector;
class  TFreeVector;
class  TRotationMatrix;
class  TSpatialPosition;
class  TSpatialVector;
class  TSpatialOrientation;

#include  <TVTransformation.h>
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Classe de base purement abstraite pour une transformation affine
class  TVAffineTransformation : public TVTransformation//, public TObject  
{
public:
	/**@name Member Functions */
	//@{
		//! Return a pointer to a clone of this transformation
		virtual  TVAffineTransformation*  clone() const = 0;

		//! transform a spatial position
		virtual  bool  transform( TSpatialPosition& ) const = 0;

		//! transform a spatial vector
		virtual  bool  transform( TSpatialVector& ) const = 0;

		//! transform a spatial orientation
		virtual  bool  transform( TSpatialOrientation& ) const = 0;

		//! transform a position vector
		virtual  bool  transform( TPositionVector& ) const = 0;

		//! transform a free vector
		virtual  bool  transform( TFreeVector& ) const = 0;

		//! transform a Rotation Matrix
		virtual  bool  transform( TRotationMatrix& ) const = 0;

		//! apply this transformation to a spatial position 
		virtual  TSpatialPosition &  operator() ( TSpatialPosition & ) const = 0;

		//! apply this transformation to a spatial vector 
		virtual  TSpatialVector &  operator() ( TSpatialVector & ) const = 0;

		//! apply this transformation to a spatial orientation 
		virtual  TSpatialOrientation &  operator() ( TSpatialOrientation & ) const = 0;

		//! apply this transformation to a position vector 
		virtual  TPositionVector &  operator() ( TPositionVector & ) const = 0;

		//! apply this transformation to a free vector 
		virtual  TFreeVector &  operator() ( TFreeVector & ) const = 0;

		//! apply this transformation to a Rotation Matrix 
		virtual  TRotationMatrix &  operator() ( TRotationMatrix & ) const = 0;

		//! Return a pointer to the inverse of this transformation
		virtual TVAffineTransformation * inverse() const = 0;

		//! Invert the transformation, replaces the current transformation parameters
		virtual void invert() = 0;
	//@}


protected:


private:

	//ClassDef(TVAffineTransformation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_V_AFFINE_TRANSFORMATION
