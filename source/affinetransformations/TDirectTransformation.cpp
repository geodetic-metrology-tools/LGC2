#include "TDirectTransformation.h"
#include "TDerivativeTransformation.h"
#include <Eigen/Dense>


TDirectTransformation::TDirectTransformation():TTransformation(){setIdentityParams();}

TDirectTransformation::TDirectTransformation(const TransformParameters &fParams)
{
	this->setTransformParam(fParams);
	this->setMatrix(fParams);
}

TDirectTransformation::TDirectTransformation(const TTransformation &transformation): TTransformation(transformation){
}

TDirectTransformation::~TDirectTransformation(){}

void TDirectTransformation::setIdentityParams(){
	fEstimatedParams.setIdentity();
}

void TDirectTransformation::setTranslationPart(const TReal tx, const TReal ty, const TReal tz){
	auto& m(*(fTransM));
	TReal scl = getScaleFactor();

	m(0,3) = tx/scl;
	m(1,3) = ty/scl;
	m(2,3) = tz/scl;
}

void TDirectTransformation::setRotationPart(const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz){
	auto& m(*(fTransM));
	TReal omegacos = rx.cos();
	zerofy(omegacos);

	TReal omegasin = rx.sin();
	zerofy(omegasin);

	TReal phicos = ry.cos();
	zerofy(phicos);

	TReal phisin = ry.sin();
	zerofy(phisin);

	TReal kappacos = rz.cos();
	zerofy(kappacos);

	TReal kappasin = rz.sin();
	zerofy(kappasin);

	m(0,0)  =  kappacos*phicos;
	m(0,1)  =  kappasin*phicos;
	m(0,2)  =  -phisin;
	m(1,0)  =  kappacos*phisin*omegasin-kappasin*omegacos;
	m(1,1)  =  kappasin*phisin*omegasin+kappacos*omegacos;
	m(1,2)  =  phicos*omegasin;
	m(2,0)  =  kappacos*phisin*omegacos+kappasin*omegasin;
	m(2,1)  =  kappasin*phisin*omegacos-kappacos*omegasin;
	m(2,2)  =  phicos*omegacos;

}

//void TDirectTransformation::setAngle(int axis, const LGC::TAngle& angle){
//	assert3D(axis);
//	
//	if (axis == 0){
//		setRotationPart(angle, fEstimatedParams.phi , fEstimatedParams.kappa);
//	}
//	else if(axis == 1){
//		setRotationPart(fEstimatedParams.omega, angle , fEstimatedParams.kappa);
//	}
//	else{
//		setRotationPart(fEstimatedParams.omega, fEstimatedParams.phi , angle);
//	}
//}

void TDirectTransformation::setScaleFactor(TReal scale){
	auto& m(*(fTransM));
	zerofy(scale);
	if(scale == 0.0)
		throw std::runtime_error("Scale factor is zero or a very small number!");
	
	m(3,3) = 1.0/scale;
}

//void TDirectTransformation::setTranslation(int axis, TReal t){
//	TReal scl = getScaleFactor();
//	assert3D(axis);
//	matrix(axis,3) = t/scl;
//}

void TDirectTransformation::setMatrix(const TransformParameters& params){
	fEstimatedParams = params;
	this->setRotationPart(params.omega, params.phi, params.kappa);
	this->setTranslationPart(params.t1, params.t2, params.t3);
	this->setScaleFactor(params.scale);
}



TReal TDirectTransformation::getScaleFactor() const{
	return fEstimatedParams.scale;
}

LGC::TAngle TDirectTransformation::getAngle(int axis) const{
	LGC::TAngle ang(LGC::TAngle::kRadians, NO_VALf);
	assert3D(axis);

		if(axis == 0)
			return fEstimatedParams.omega;
		else if(axis == 1)
			return fEstimatedParams.phi;
		else
			return fEstimatedParams.kappa;
}

TReal TDirectTransformation::getTranslation(int axis) const{
	assert3D(axis);
	if(axis == 0)
		return fEstimatedParams.t1;
	else if(axis == 1)
		return fEstimatedParams.t2;
	else
		return fEstimatedParams.t3;
}



TDerivativeTransformation TDirectTransformation::differentiatedTransformationAngle(int angle) const{
	assert3D(angle);
	Eigen::Matrix4d m;
	m.setZero();

	TReal omegaCos = fEstimatedParams.omega.cos();
	zerofy(omegaCos);

	TReal omegaSin = fEstimatedParams.omega.sin();
	zerofy(omegaSin);

	TReal phiCos = fEstimatedParams.phi.cos();
	zerofy(phiCos);

	TReal phiSin = fEstimatedParams.phi.sin();
	zerofy(phiSin);

	TReal kappaCos = fEstimatedParams.kappa.cos();
	zerofy(kappaCos);

	TReal kappaSin = fEstimatedParams.kappa.sin();
	zerofy(kappaSin);

	switch (angle) {
	case 0:
		    m(1,0) = kappaCos*phiSin*omegaCos + kappaSin*omegaSin;
			m(1,1) = kappaSin*phiSin*omegaCos-kappaCos*omegaSin;
			m(1,2) = phiCos*omegaCos;
			m(2,0) = -kappaCos*phiSin*omegaSin+kappaSin*omegaCos;
			m(2,1) = -kappaSin*phiSin*omegaSin-kappaCos*omegaCos;
			m(2,2) = -phiCos*omegaSin;
			m(3,3) =  1/getScaleFactor(); //Defined to be (1/scale) = m(3,3)
			break;
	case 1:
			m(0,0) = -kappaCos*phiSin;
			m(0,1) = -kappaSin*phiSin;
			m(0,2) = -phiCos;
			m(1,0) = kappaCos*phiCos*omegaSin;
			m(1,1) = kappaSin*phiCos*omegaSin;
			m(1,2) = -phiSin*omegaSin;
			m(2,0) = kappaCos*phiCos*omegaCos;
			m(2,1) = kappaSin*phiCos*omegaCos;
			m(2,2) = -phiSin*omegaCos;
			m(3,3) =  1/getScaleFactor(); //Defined to be (1/scale) = m(3,3)
			break;
	case 2:
   			m(0,0) = -kappaSin*phiCos;
			m(0,1) = kappaCos*phiCos;
			m(1,0) = -kappaSin*phiSin*omegaSin-kappaCos*omegaCos;
			m(1,1) = kappaCos*phiSin*omegaSin-kappaSin*omegaCos;
			m(2,0) = -kappaSin*phiSin*omegaCos+kappaCos*omegaSin;
			m(2,1) = kappaCos*phiSin*omegaCos+kappaSin*omegaSin;
			m(3,3) =  1/getScaleFactor();//Defined to be (1/scale) = m(3,3)
			break;
	}

	// set the new matrix to the derivative object
	return TDerivativeTransformation(m);
}

TDerivativeTransformation TDirectTransformation::differentiatedTransformationTranslation(int ti) const{
	
	assert3D(ti);
	Eigen::Matrix4d m;
	m.setZero();

	m(3,3) = 1.0; // Defined to be 1
	if(ti == 0)
		m(0,3) = 1.0;
	else if(ti == 1)
		m(1,3) = 1.0;
	else if(ti == 2)
		m(2,3) = 1.0;

	return TDerivativeTransformation(m);
}

TDerivativeTransformation TDirectTransformation::differentiatedTransformationScaleFactor() const{

	Eigen::Matrix4d m(*fTransM);

	//Rotation part remains unchanged, no translation and 1 on a scale position
	m(0,3) = 0;
	m(1,3) = 0;
	m(2,3) = 0;
	m(3,3) = 1;

	return TDerivativeTransformation(m);
}

bool TDirectTransformation::transform(TRotationMatrix& rm) const{
	TCoordSysFactory::ECoordSys k3DCart = TCoordSysFactory::ECoordSys::k3DCartesian;
	TANumericValue::EStatus kNull = TANumericValue::EStatus::kNull;
	bool result = false;

	//If status is not null and matrix is in a 3d cart. coordinates.
	if(rm.getStatus()!=kNull && rm.getCoordSys()==k3DCart){

		//(TRotationMatrix is not affected by translation and scaling), just multiplication of rotation matrices.
		TRotationMatrix transformation(TRotationMatrix::ERotationType::kRzyx, getAngle(0).rad(), getAngle(1).rad(), getAngle(2).rad());
		//Transform
		rm = transformation * rm;	
		result = true;
	}
	else{
		rm.setStatus(kNull);
	}
	return result;
}


