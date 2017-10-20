/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_CCS_2_CGRF_TRANSFORMATION
#define SU_CCS_2_CGRF_TRANSFORMATION


//LGC
#include  <TTransformation.h>

/*!
	\ingroup RefFrameTransformations
	\brief Transformation between the CCS and the Cern Geodetic reference frame (CGRF).
*/
class  TCCS2CGRFTransformation
{
public:
	/**@name Constructors and Destructors */
	//@{
		/// Constructors

		/*Uses either ellipsoid CGRF or Sphere CGRF*/
	TCCS2CGRFTransformation(bool fIsSphere = false);

		/*Resets the transformation, sets either ellipsoid or spere CGRF*/
	void reInitialize(bool fIsSphere);

		/// Destructor
		~TCCS2CGRFTransformation();
	//@}


	/**@name Transformations */
	//@{
		/// transform a position vector from CCS to CGRF
		bool transform( TPositionVector& pv ) const;

		/// transform a free vector from CCS to CGRF
		bool transform( TFreeVector& fv ) const;

		TPositionVector getTransformedPt( const TPositionVector& pv ) const;

		/// inverse transformation of a position vector, from CGRF to CCS
		bool transformIverse( TPositionVector& pv ) const;

		TPositionVector  getTransformedPtInverse( const TPositionVector& pv ) const;

		/// inverse transformation of a free vector, from CGRF to CCS
		bool transformInverse( TFreeVector& fv ) const;

	//@}

	/**@name Access methods */
	//@{
		/// Returns THelmertTransformation of this transformation (CCS -> CGRF) 
		TTransformation getTransformer() const { return fTransform; }

		/// Returns THelmertTransformation of inverse of this transformation (CGRF -> CCS)
		TTransformation getInverseTransformer() const {return fTransform.getInversedTransformation();}

		inline bool isCGRFEllipsoid() { return !fIsSphere; }
		inline bool isCGRFSphere() { return fIsSphere; }
	//@}


private:

	TTransformation fTransform;

	/*For ellispoid transformation different transformation is applied than for Sphere*/
	bool fIsSphere;

	/// Initialise the transformation
	//void  initialise();
	void initialiseEllipsoid();
	void initialiseSphere();
};


#endif // TCCS2CGRFTransformation.h
