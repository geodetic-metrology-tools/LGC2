/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_CGRF_2_ILG_TRANSFORMATION
#define SU_CGRF_2_ILG_TRANSFORMATION

//LGC
#include  <TTransformation.h>

/*!
	\ingroup RefFrameTransformations
	\brief Transformation between the CGRF and a local geodetic system (LG).

	Origin of the LG system is expected to be in CGRF, geodetic cartesian coordinates.
*/
class  TCGRF2LGTransformation 
{
public:
	/**@name Constructors and Destructors */
	//@{
		/*! 
			Constructor
				\param[in] ILGorigin  Origin of the local geodetic system in CGRF, geodetic cartesian coordinates.
		*/
		TCGRF2LGTransformation(TPositionVector& ILGorigin, bool isSphere);

		/*! 
			Constructor. Creates an uninitialized object.
		*/
		TCGRF2LGTransformation();

		/// Destructor
		~TCGRF2LGTransformation();
	//@}

	/**@name Initialisation */
	//@{

		/// Initialise / reinitialise an existing transformation by passing an origin of the local geodetic frame
		void  initialise(TPositionVector& ILGorigin, bool isSphere = false);

	//@}

	/**@name Transformations */
	//@{
		/// transform a position vector
		bool transform( TPositionVector& pv ) const;

		/// transform a free vector
		bool transform( TFreeVector& fv ) const;

		/// transform a rotation matrix
		bool transform( TRotationMatrix& rmx ) const;

		/// inverse transformation of a position vector, (LG -> CGRF) 
		bool transformInverse( TPositionVector& pv ) const;

		/// inverse transformation of a free vector, (LG -> CGRF)
		bool transformInverse( TFreeVector& fv ) const;

		/// inverse transformation of a rotation matrix, (LG -> CGRF)
		bool transformInverse( TRotationMatrix& rmx ) const;	
	//@}

	/**@name Access methods */
	//@{
		/// Returns THelmertTransformation of this transformation (CGRF-> LG) 
		TTransformation	getTransformer() const { return fTransform; }

		/// Returns THelmertTransformation of inverse of this transformation (LG -> CGRF)
		TTransformation getInverseTransformer() const {return fTransform.getInversedTransformation();}

		///Returns origin of the local geodetic system
		TPositionVector	getLGSOrigin() const{return fILGOrigin;}		
	//@}


private:
	TTransformation fTransform;
	TPositionVector fILGOrigin;

	// Initialise the transformation using the parameters of the two reference frames
	virtual  void  initialiseEllipsoid();
	virtual  void  initialiseSphere();

	//Tells whether the transformation is initialized
	bool fInitialised;
};

#endif // TCGRF2LGTransformation.h
