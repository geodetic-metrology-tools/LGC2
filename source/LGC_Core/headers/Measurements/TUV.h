/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _TUV_H_
#define _TUV_H_

#include <TAMeas.h>

/*!
	\ingroup Measurements
	\brief Pixel coordinates (u, v) of a target measured by a pixel camera, stored in the x and y components of the vector.
*/
class TUV : public TAVectorMeas<TInstrumentData::TCAMD::TTarget>
{
public:
	/// Constructor
	TUV(const LGCAdjustablePoint &pos, TInstrumentData::TCAMD::TTarget tgt) : TAVectorMeas<TInstrumentData::TCAMD::TTarget>(pos, tgt)
	{
		fMeasIdx.eqDim = 2;
		fMeasIdx.obsDim = 2;
	}

	virtual void setObsVector(const Eigen::VectorXd &obsVect) override { setVectorMeasurement(TFreeVector(obsVect(0), obsVect(1), 0.0, TCoordSysFactory::k3DCartesian)); }

	virtual Eigen::VectorXd getObsVector() const override { return Eigen::Vector2d(getVectorValue().getX().getMetresValue(), getVectorValue().getY().getMetresValue()); }

	/// TRUE if the target was behind the camera at the last evaluation (see getUVContrib), marked in the result file
	bool behindCamera = false;
};
#endif
