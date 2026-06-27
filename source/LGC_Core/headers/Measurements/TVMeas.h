/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _TV_MEAS_H_
#define _TV_MEAS_H_

#include <stdexcept>
#include <string>

#include <Eigen/Dense>

#include <TStatusObject.h>

/*!
	\ingroup Measurements
	\brief Non-template polymorphic interface for measurements.

	Provides a common base, independent of the target type, so that measurements of any
	type can be held and accessed generically: by observation id and observation vector.
	Implemented by \ref TAMeas and its subclasses. The activation status (isActive /
	setActive) is inherited from \ref TStatusObject.
*/
class TVMeas : public TStatusObject
{
public:
	virtual ~TVMeas() = default;

	/// Observation identifier of this measurement.
	virtual const std::string &getObsId() const = 0;

	/// Observation value as a vector (dimension depends on the measurement type).
	virtual Eigen::VectorXd getObsVector() const = 0;

	/// Set the observation value from a vector of the matching dimension.
	virtual void setObsVector(const Eigen::VectorXd &obsVect) = 0;

	/// Estimated residual as a vector, in the same component order and dimension as the observation vector.
	virtual Eigen::VectorXd getResidualVector() const = 0;

	/// Observation standard deviations, in the same component order/dimension as the observation vector.
	/// Default throws; measurement types that participate in the monitoring API override it.
	virtual Eigen::VectorXd getObsSigmaVector() const
	{
		throw std::logic_error("getObsSigmaVector is not implemented for this measurement type.");
	}

	/// Set the observation standard deviations from a vector of the matching dimension.
	virtual void setObsSigmaVector(const Eigen::VectorXd &)
	{
		throw std::logic_error("setObsSigmaVector is not implemented for this measurement type.");
	}
};

#endif // _TV_MEAS_H_
