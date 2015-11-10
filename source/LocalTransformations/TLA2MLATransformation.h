#ifndef SU_LA_2_MLA_TRANSFORMATION
#define SU_LA_2_MLA_TRANSFORMATION

#include  "TTransformation.h"
#include  "TRefSystemFactory.h"

/*!
\ingroup RefFrameTransformations

\brief Transformation between local astronomical and a modified local astronomical systems.

*/
class  TLA2MLATransformation
{
public:
	/**@name Constructors and Destructors */
	//@{
	/*! Constructor
	\param[in] origin Origin of the LG and LA system, expected to be in the CCS.
	\param[in] geoidModel The geoid model used for this transformation.
	*/
	TLA2MLATransformation(TPositionVector& origin, TAngle gis, TAngle slope);

	/*!
	Constructor. Creates an uninitialized object.
	*/
	TLA2MLATransformation();

	/// Destructor
	~TLA2MLATransformation();
	//@}


	/**@name Transformations */
	//@{
	/// transform a position vector
	bool						transform(TPositionVector& pv) const;

	/// transform a free vector
	bool						transform(TFreeVector& fv) const;


	/// inverse transformation of a position vector
	bool						transformInverse(TPositionVector& pv) const;

	/// inverse transformation of a free vector
	bool						transformInverse(TFreeVector& fv) const;

	//@}

	/*!@name Access methods*/
	//@{
	/// Returns THelmertTransformation of this transformation (LA-> MLA) 
	TTransformation		getTransformer() const { return fTransform; }

	/// Returns THelmertTransformation of inverse of this transformation (MLA -> LA)
	TTransformation      getInverseTransformer() const { return fTransform.getInversedTransformation(); }

	///Returns origin of the local geodetic system and local astronomical system
	TPositionVector	            getOrigin() const { return fOrigin; }

	//@}

private:
	TTransformation		fTransform;
	TPositionVector				fOrigin;
	TAngle fBearing, fSlope;

	bool fInitialised;

	//Initialise the transofrmation
	void  initialise();

};
#endif // SU_LA_2_MLA_TRANSFORMATION