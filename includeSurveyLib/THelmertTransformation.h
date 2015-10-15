/////////////////////////////////////////////////////////////////////////////
//THelmertTransformation.h
//
/** Classe pour une transformation helmert X1 = Fact*R*X+T*/
//
// Copyright 2000-2010 CERN SU, M.Jones. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef SU_HELMERT_TRANSFORMATION
#define SU_HELMERT_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//
#include  "TScaleFactor.h"
#include  "TRotation.h"
#include  "TTranslation.h"
#include  "TAAffineTransformation.h"
// typedefs
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Classe pour une transformation helmert
class  THelmertTransformation : public TAAffineTransformation  
{
public:
	/**@name  Constructors and destructors */
	//@{
		//! Default Constructor
		THelmertTransformation();

		//! Constructor
		THelmertTransformation(const TScaleFactor &, const TRotation &, const TTranslation &);

		//! Constructor
		THelmertTransformation(const TRotation &, const TTranslation &);

		//! Copy Constructor
		THelmertTransformation(const THelmertTransformation & );
		
		//! Destructor
		virtual ~THelmertTransformation();
	//@}

	using TAAffineTransformation::operator();
	using TAAffineTransformation::transform;

	//! Copy Assignment Operator 
	THelmertTransformation & operator= ( const THelmertTransformation & );

	/**@name Setting Member Functions */
	//@{
		//! Set all 3 transformations
		void setTransformations(const TScaleFactor&, const TRotation&, const TTranslation&);
		
		//! Set scaling factor
		void setScaleFactor( const TScaleFactor&);

		//! Set rotation
		void setRotation(const TRotation&);

		//! Set translation
		void setTranslation(const TTranslation&);
	//@}

	/**@name Getting Member Functions */
	//@{
		//! Get scaling factor
		TScaleFactor getScaleFactor() const;

		//! Get rotation
		TRotation getRotation() const;
		
		//! Get translation
		TTranslation getTranslation() const;
	//@}
		
		
	/**@name  TVAffineTransformation Interface Methods */
	//@{	
		//! Create a composite transformation by applying this transformation to an affine transformation
		//virtual  TCompositeAffTransform operator() ( const TAAffineTransformation & ) const;

		//! Return a pointer to a clone of this transformation
		virtual THelmertTransformation*  clone() const;

		//! Transform a position vector
		virtual bool  transform( TPositionVector& ) const;

		//! Transform a free vector
		virtual bool  transform( TFreeVector& ) const ;

		//! Transform a Rotation Matrix
		virtual bool  transform( TRotationMatrix& ) const;

		//! apply this transformation to a position vector
		virtual  TPositionVector &  operator() ( TPositionVector & ) const;

		//! apply this transformation to a free vector
		virtual  TFreeVector &  operator() ( TFreeVector & ) const;

		//! apply this transformation to a Rotation Matrix
		virtual  TRotationMatrix &  operator() ( TRotationMatrix & ) const;

		//! Return a pointer to the inverse of this transformation: X = R~-1*(1/factor)(x-T)
		virtual  THelmertTransformation * inverse() const;

		//! Invert the transformation, replaces the current transformation parameters
		virtual  void invert();
//@}


protected:
	// protected methods
	//! Get a pointer to the scale factor
	const TScaleFactor * scaleFactor() const { return &fScaleFactor; }

	//! Get a pointer to the rotation
	const TRotation * rotation() const { return &fRotation; }
	
	//! Get a pointer to the translation
	const TTranslation * translation() const { return & fTranslation; }


private:
	// member attributes
	TScaleFactor fScaleFactor; /*!< scaling */
	TRotation fRotation; /*!< rotation */
	TTranslation fTranslation; /*!< translation */
		
	//ClassDef(THelmertTransformation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////

inline  void THelmertTransformation::setScaleFactor( const TScaleFactor& scaling) {fScaleFactor = scaling; return;}
inline	void THelmertTransformation::setRotation(const TRotation& rotate) {fRotation = rotate; return;}
inline	void THelmertTransformation::setTranslation(const TTranslation& trans) {fTranslation = trans;return;}

inline  TScaleFactor THelmertTransformation::getScaleFactor() const {return fScaleFactor;}
inline	TRotation    THelmertTransformation::getRotation() const {return fRotation;}
inline	TTranslation THelmertTransformation::getTranslation() const {return fTranslation;}
		

#endif // SU_HELMERT_TRANSFORMATION
