/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SU_ILG_2_ILA_TRANSFORMATION
#define SU_ILG_2_ILA_TRANSFORMATION

// SURVEYLIB
#include <TRefSystemFactory.h>
// LGC
#include <TTransformation.h>

/*!
	\ingroup RefFrameTransformations

	\brief Transformation between local geodetic and a local astronomical systems.

	Origin of the LG system is the same as LA system and is expected to be in CCS.
*/
class TILG2ILATransformation
{
public:
	/**@name Constructors and Destructors */
	//@{
	/*! Constructor
		\param[in] origin Origin of the LG and LA system, expected to be in the CCS.
		\param[in] geoidModel The geoid model used for this transformation.
	*/
	TILG2ILATransformation(TPositionVector &origin, TRefSystemFactory::EGeoid geoidModel);

	/*!
		Constructor. Creates an uninitialized object.
	*/
	TILG2ILATransformation();

	/// Destructor
	~TILG2ILATransformation();
	//@}

	/**@name Transformations */
	//@{
	/// transform a position vector
	bool transform(TPositionVector &pv) const;

	/// transform a free vector
	bool transform(TFreeVector &fv) const;

	/// inverse transformation of a position vector ( ILA -> ILG)
	bool transformInverse(TPositionVector &pv) const;

	/// inverse transformation of a free vector (ILA -> ILG)
	bool transformInverse(TFreeVector &fv) const;

	//@}

	/*!@name Access methods*/
	//@{
	/// Returns THelmertTransformation of this transformation (ILG-> ILA)
	TTransformation getTransformer() const { return fTransform; }

	/// Returns THelmertTransformation of inverse of this transformation (ILA -> ILG)
	TTransformation getInverseTransformer() const { return fTransform.getInversedTransformation(); }

	/// Returns origin of the local geodetic system and local astronomical system
	TPositionVector getOrigin() const { return fOrigin; }

	/// Returns the geoid model used
	TRefSystemFactory::EGeoid getGeoidModel() const { return fGeoidModel; }
	//@}

private:
	TTransformation fTransform;
	TPositionVector fOrigin;
	TPositionVector originInCGRF;
	TRefSystemFactory::EGeoid fGeoidModel;

	bool fInitialised;

	/// Initialise the transformation using the parameters of the two reference frames
	void initialise();
};
#endif // SU_ILG_2_ILA_TRANSFORMATION
