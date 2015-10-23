#ifndef TTRANSFORMATION_PARAMETERS
#define TTRANSFORMATION_PARAMETERS

#include "TAngle.h"

/*!
	\ingroup LocalTransformations
	\brief Structure which stores parameters of an helmert transformation.
*/
struct TransformParameters{
	TAngle omega; //!< Rotation about the X axis
	TAngle phi;  //!< Rotation about the Y axis
	TAngle kappa; //!< Rotation about the Z axis 

	TReal tX; //!< Translation about the X axis in meters [m]
	TReal tY; //!< Translation about the Y axis in meters [m]
	TReal tZ; //!< Translation about the Z axis in meters [m]
	
	TReal scale; //!< The unitless scale factor

	TransformParameters() : omega(TAngle(0.0)), phi(TAngle(0.0)), kappa(TAngle(0.0)), tX(TReal(0.0)), tY(TReal(0.0)), tZ(TReal(0.0)), scale(TReal(1.0)) {};

	/// Overlading  comparition operator for TransformParameters
	bool operator==(const TransformParameters& p) {
		return (omega==p.omega && phi==p.phi && kappa==p.kappa && tX==p.tX && tY==p.tY && tZ==p.tZ && scale==p.scale);
	}

	/// Overlading  + operator for TransformParameters
	TransformParameters operator+(const TransformParameters &transfParam1)
	{//add two Transformation parameters objects
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
	{//add two Transformation parameters objects
		TransformParameters resultat;
		resultat.tX = this->tX + transfParam1.tX;
		resultat.tY = this->tY + transfParam1.tY;
		resultat.tZ = this->tZ + transfParam1.tZ;

		resultat.omega = this->omega - transfParam1.omega;
		resultat.phi = this->phi - transfParam1.phi;
		resultat.kappa = this->kappa - transfParam1.kappa;

		resultat.scale = this->scale - transfParam1.scale;
		return resultat;
	}

	/// Overlading  * operator for TransformParameters
	TransformParameters operator*(const double& a)
	{
		TransformParameters resultat;
		resultat.tX = this->tX *a;
		resultat.tY = this->tY *a;
		resultat.tZ = this->tZ *a;

		resultat.omega = this->omega *a;
		resultat.phi = this->phi *a;
		resultat.kappa = this->kappa *a;

		resultat.scale = this->scale *a;
		return resultat;
	}

	/// Overlading  += operator for TransformParameters
	TransformParameters& operator+=(const TransformParameters &transfParam1)
	{//add two TLength objects and rewrite this
		return *this=*this+transfParam1;
	}

	/// Sets the parameters to their identity
	void setIdentity() {
		omega = phi = kappa = TAngle(0.0);
		tX = tY = tZ = TReal(0.0);
		scale = TReal(1.0);
	}
};

#endif