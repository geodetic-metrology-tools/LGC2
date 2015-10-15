//  TCCS2CGRFTransformation.h
#ifndef SU_CCS_2_CGRF_TRANSFORMATION
#define SU_CCS_2_CGRF_TRANSFORMATION

#include  "TTransformation.h"

/*!
	Transformation between the CCS and the
    Cern Geodetic reference frame (CGRF).
*/
class  TCCS2CGRFTransformation
{
public:
	/**@name Constructors and Destructors */
	//@{
		/// Constructors

		/*Uses either ellipsoid CGRF or Sphere CGRF*/
		TCCS2CGRFTransformation(bool isEllipsoid = true);

		/*Resets the transformation, sets either ellipsoid or spere CGRF*/
		void reInitialize (bool isEllipsoid);

		/// Destructor
		~TCCS2CGRFTransformation();
	//@}


	/**@name Transformations */
	//@{
		/// transform a position vector from CCS to CGRF
		bool						transform( TPositionVector& pv ) const;

		/// transform a free vector from CCS to CGRF
		bool						transform( TFreeVector& fv ) const;

		TPositionVector getTransformedPt( const TPositionVector& pv ) const;


		/// transform a rotation matrix  from CCS to CGRF
		bool						transform( TRotationMatrix& rmx ) const;

		/// inverse transformation of a position vector, from CGRF to CCS
		bool						transformIverse( TPositionVector& pv ) const;

		TPositionVector  getTransformedPtInverse( const TPositionVector& pv ) const;

		/// inverse transformation of a free vector, from CGRF to CCS
		bool						transformInverse( TFreeVector& fv ) const;

		/// inverse transformation of a rotation matrix, from CGRF to CCS
		bool						transformInverse( TRotationMatrix& rmx ) const;
	//@}

	/**@name Access methods */
	//@{
		/// Returns THelmertTransformation of this transformation (CCS -> CGRF) 
		TTransformation				getTransformer() const { return fTransform; }

		/// Returns THelmertTransformation of inverse of this transformation (CGRF -> CCS)
		TTransformation              getInverseTransformer() const {return fTransform.getInversedTransformation();}

		inline bool                       isCGRFEllipsoid() {return fIsEllipsoid;}
		inline bool                       isCGRFSphere() {return !fIsEllipsoid;}
	//@}


private:

	TTransformation		fTransform;

	/*For ellispoid transformation different transformation is applied than foe Sphere*/
	bool		        fIsEllipsoid;

	/// Initialise the transformation
	void  initialise();
};


#endif // TCCS2CGRFTransformation.h
