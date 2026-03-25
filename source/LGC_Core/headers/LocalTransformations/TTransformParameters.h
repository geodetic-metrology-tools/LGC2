/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TTRANSFORMATION_PARAMETERS
#define TTRANSFORMATION_PARAMETERS

// SURVEYLIB
#include <TAngle.h>
#include <TLength.h>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER

/*!
	\ingroup LocalTransformations
	\brief Structure which stores parameters of an helmert transformation.
*/
#if USE_SERIALIZER
struct TransformParameters : public Serializable
#else
struct TransformParameters
#endif // USE_SERIALIZER
{
	TAngle omega; //!< Rotation about the X axis
	TAngle phi; //!< Rotation about the Y axis
	TAngle kappa; //!< Rotation about the Z axis

	TLength tX; //!< Translation about the X axis in meters [m]
	TLength tY; //!< Translation about the Y axis in meters [m]
	TLength tZ; //!< Translation about the Z axis in meters [m]

	TReal scale; //!< The unitless scale factor

	TransformParameters() : omega(TAngle(0.0)), phi(TAngle(0.0)), kappa(TAngle(0.0)), tX(TLength(0.0)), tY(TLength(0.0)), tZ(TLength(0.0)), scale(TReal(1.0)){};
	// parametrized constructor
	TransformParameters(TAngle rx, TAngle ry, TAngle rz, TLength tx, TLength ty, TLength tz, TReal s) : omega(rx), phi(ry), kappa(rz), tX(tx), tY(ty), tZ(tz), scale(s){};

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif

	/// Overlading  comparition operator for TransformParameters
	bool operator==(const TransformParameters &p)
	{
		return (omega == p.omega && phi == p.phi && kappa == p.kappa && tX == p.tX && tY == p.tY && tZ == p.tZ && scale == p.scale);
	}

	/// Overlading  + operator for TransformParameters
	TransformParameters operator+(const TransformParameters &transfParam1)
	{ // add two Transformation parameters objects
		TransformParameters resultat;
		resultat.tX = this->tX + transfParam1.tX;
		resultat.tY = this->tY + transfParam1.tY;
		resultat.tZ = this->tZ + transfParam1.tZ;

		resultat.omega = this->omega + transfParam1.omega;
		resultat.phi = this->phi + transfParam1.phi;
		resultat.kappa = this->kappa + transfParam1.kappa;

		resultat.scale = this->scale + transfParam1.scale;
		return resultat;
	}

	/// Overlading  - operator for TransformParameters
	TransformParameters operator-(const TransformParameters &transfParam1)
	{ // add two Transformation parameters objects
		TransformParameters resultat;
		resultat.tX = this->tX - transfParam1.tX;
		resultat.tY = this->tY - transfParam1.tY;
		resultat.tZ = this->tZ - transfParam1.tZ;

		resultat.omega = this->omega - transfParam1.omega;
		resultat.phi = this->phi - transfParam1.phi;
		resultat.kappa = this->kappa - transfParam1.kappa;

		resultat.scale = this->scale - transfParam1.scale;
		return resultat;
	}

	/// Overlading  * operator for TransformParameters
	TransformParameters operator*(const double &a)
	{
		TransformParameters resultat;
		resultat.tX = this->tX * a;
		resultat.tY = this->tY * a;
		resultat.tZ = this->tZ * a;

		resultat.omega = this->omega * a;
		resultat.phi = this->phi * a;
		resultat.kappa = this->kappa * a;

		resultat.scale = this->scale * a;
		return resultat;
	}

	/// Overlading  += operator for TransformParameters
	TransformParameters &operator+=(const TransformParameters &transfParam1)
	{ // add two TLength objects and rewrite this
		return *this = *this + transfParam1;
	}

	/// Sets the parameters to their identity
	void setIdentity()
	{
		omega = phi = kappa = TAngle(0.0);
		tX = tY = tZ = TLength(0.0);
		scale = TReal(1.0);
	}
};
#if USE_SERIALIZER
// Inherited via Serializable
inline void TransformParameters::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("kappa", kappa);
	obj.addProperty("omega", omega);
	obj.addProperty("phi", phi);

	obj.addProperty("scale", scale);

	obj.addProperty("tX", tX);
	obj.addProperty("tY", tY);
	obj.addProperty("tZ", tZ);
};
#endif

#endif
