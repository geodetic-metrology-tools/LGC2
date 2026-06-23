/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _TUVEC_H_
#define _TUVEC_H_

// SURVEYLIB
#include <UEOIndices.h>
// LGC
#include <TAMeas.h>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER
/*!
	\ingroup Measurements
	\brief Unit vector measurement made by a camera (TCAM).
*/
class TUVEC : public TAVectorMeas<TInstrumentData::TCAMD::TTarget>
{
public:
	/// Sign of the observed uz component (+1 or -1), used to select the correct unit-sphere hemisphere
	TReal signUz = 1.0;

	/// Constructor
	TUVEC(const LGCAdjustablePoint &pos, TInstrumentData::TCAMD::TTarget tgt) : TAVectorMeas<TInstrumentData::TCAMD::TTarget>(pos, tgt)
	{
		fMeasIdx.eqDim = 2;
		fMeasIdx.obsDim = 2;
	}

	virtual void setObsVector(const Eigen::VectorXd &obsVect) override
	{
		// internally UVEC has 2 residuals: x,y
		// z is reconstructed here for consistency but is not used in the adjustment
		double xyNormSq = pow2(obsVect(0)) + pow2(obsVect(1));
		double zObs = sqrt(std::clamp(1.0 - xyNormSq, 0.0, 1.0));
		TFreeVector direction(obsVect(0), obsVect(1), zObs, TCoordSysFactory::k3DCartesian);
		setVectorMeasurement(direction);
	}

	virtual Eigen::VectorXd getObsVector() const override
	{
		Eigen::VectorXd result(2);
		result << getVectorValue().getX().getMetresValue(), getVectorValue().getY().getMetresValue();
		return result;
	}

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override { TAVectorMeas<TInstrumentData::TCAMD::TTarget>::serialize(obj); }
#endif
};
#endif
