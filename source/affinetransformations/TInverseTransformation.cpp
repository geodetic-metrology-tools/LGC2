#include "TInverseTransformation.h"

TInverseTransformation::TInverseTransformation():TTransformation(){setIdentityParams();}


TInverseTransformation::TInverseTransformation(const TransformParameters &fParams) 
{
	this->setTransformParam(fParams);
	this->setMatrix(fParams);
}


TInverseTransformation::TInverseTransformation(const TTransformation &transformation): TTransformation(transformation){
}


TInverseTransformation::~TInverseTransformation(){}

void TInverseTransformation::setIdentityParams(){
	fEstimatedParams.setIdentity();
}

void TInverseTransformation::setTranslationPart(const TransformParameters& params){
	auto& m(*(fTransM));

	TReal omegacos = params.omega.cos();
	zerofy(omegacos);

	TReal omegasin = params.omega.sin();
	zerofy(omegasin);

	TReal phicos = params.phi.cos();
	zerofy(phicos);

	TReal phisin = params.phi.sin();
	zerofy(phisin);

	TReal kappacos = params.kappa.cos();
	zerofy(kappacos);

	TReal kappasin = params.kappa.sin();
	zerofy(kappasin);

	m(0,3) = -(kappacos*phicos)*params.t1 - (kappacos*phisin*omegasin-kappasin*omegacos)*params.t2 - (kappacos*phisin*omegacos+kappasin*omegasin)*params.t3;
	m(1,3) = -(kappasin*phicos)*params.t1 - (kappasin*phisin*omegasin+kappacos*omegacos)*params.t2 - (kappasin*phisin*omegacos-kappacos*omegasin)*params.t3;
	m(2,3) = -(-phisin)*params.t1 - (phicos*omegasin)*params.t2 - (phicos*omegacos)*params.t3;

}

void TInverseTransformation::setRotationPart(const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz){
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
	m(0,1)  =  kappacos*phisin*omegasin-kappasin*omegacos;
	m(0,2)  =  kappacos*phisin*omegacos+kappasin*omegasin;
	m(1,0)  =  kappasin*phicos;
	m(1,1)  =  kappasin*phisin*omegasin+kappacos*omegacos;
	m(1,2)  =  kappasin*phisin*omegacos-kappacos*omegasin;
	m(2,0)  =  -phisin;
	m(2,1)  =  phicos*omegasin;
	m(2,2)  =  phicos*omegacos;
}

//void TInverseTransformation::setAngle(int axis, const LGC::TAngle& angle){
//	assert3D(axis);
//	
//	if (axis == 0){
//		setRotationPart(angle, fEstimatedParams.phi ,fEstimatedParams.kappa);
//	}
//	else if(axis == 1){
//		setRotationPart(fEstimatedParams.omega, angle , fEstimatedParams.kappa);
//	}
//	else{
//		setRotationPart(fEstimatedParams.omega, fEstimatedParams.phi , angle);
//	}
//}

void TInverseTransformation::setScaleFactor(TReal scale){
	auto& m(*(fTransM));
	zerofy(scale);
	m(3,3) = scale;
}

void TInverseTransformation::setMatrix(const TransformParameters& params){
	fEstimatedParams = params;
	this->setRotationPart(params.omega, params.phi, params.kappa);
	this->setTranslationPart(params);
	this->setScaleFactor(params.scale);
}

TReal TInverseTransformation::getScaleFactor() const{
	return fEstimatedParams.scale;
}

LGC::TAngle TInverseTransformation::getAngle(int axis) const{
	LGC::TAngle ang(LGC::TAngle::kRadians, NO_VALf);
	assert3D(axis);

	if(axis == 0)
		return fEstimatedParams.omega;
	else if(axis == 1)
		return fEstimatedParams.phi;
	else
		return fEstimatedParams.kappa;
}

TReal TInverseTransformation::getTranslation(int axis) const{
	assert3D(axis);
	if(axis == 0)
		return fEstimatedParams.t1;
	else if(axis == 1)
		return fEstimatedParams.t2;
	else
		return fEstimatedParams.t3;
}

TDerivativeTransformation TInverseTransformation::differentiatedTransformationAngle(int angle) const{

	assert3D(angle);
	Eigen::Matrix4d m;
	m.setZero();

	TReal t1 = this->getTranslation(0);
	TReal t2 = this->getTranslation(1);
	TReal t3 = this->getTranslation(2);

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
			m(0,1) = kappaCos*phiSin*omegaCos+kappaSin*omegaSin;
			m(0,2) = -kappaCos*phiSin*omegaSin+kappaSin*omegaCos;
			m(0,3) = -m(0,0)*t1 - m(0,1)*t2 - m(0,2)*t3;
			m(1,1) = kappaSin*phiSin*omegaCos-kappaCos*omegaSin;
			m(1,2) = -kappaSin*phiSin*omegaSin-kappaCos*omegaCos;
			m(1,3) = -m(1,0)*t1 - m(1,1)*t2 - m(1,2)*t3;
			m(2,1) = phiCos*omegaCos;
			m(2,2) = -phiCos*omegaSin;
			m(2,3) = -m(2,0)*t1 - m(2,1)*t2 - m(2,2)*t3;
			m(3,3) = getScaleFactor();	//Scale is on this place by definition
			break;
	case 1:
			m(0,0) = -kappaCos*phiSin;
			m(0,1) = -kappaCos*phiSin*omegaSin;
			m(0,2) = kappaCos*phiCos*omegaCos;
			m(0,3) = -m(0,0)*t1 - m(0,1)*t2 - m(0,2)*t3;
			m(1,0) = -kappaSin*phiSin;
			m(1,1) = kappaSin*phiCos*omegaSin;
			m(1,2) = kappaSin*phiCos*omegaCos;
			m(1,3) = -m(1,0)*t1 - m(1,1)*t2 - m(1,2)*t3;
			m(2,0) = -phiCos;
			m(2,1) = -phiSin*omegaSin;
			m(2,2) = -phiSin*omegaCos;
			m(2,3) = -m(2,0)*t1 - m(2,1)*t2 - m(2,2)*t3;
			m(3,3) = getScaleFactor();	//Scale is on this place by definition
			break;
	case 2:
			m(0,0) = -kappaSin*phiCos;
			m(0,1) = -kappaSin*phiSin*omegaSin-kappaCos*omegaCos;
			m(0,2) = -kappaSin*phiSin*omegaCos+kappaCos*omegaSin;
			m(0,3) = -m(0,0)*t1 - m(0,1)*t2 - m(0,2)*t3;
			m(1,0) = kappaCos*phiCos;
			m(1,1) = kappaCos*phiSin*omegaSin-kappaSin*omegaCos;
			m(1,2) = kappaCos*phiSin*omegaCos+kappaSin*omegaSin;
			m(1,3) = -m(1,0)*t1 - m(1,1)*t2 - m(1,2)*t3;
			m(3,3) = getScaleFactor();	//Scale is on this place by definition
			break;
	}

	// set the new matrix to the derivative object
	return TDerivativeTransformation(m);
}



TDerivativeTransformation TInverseTransformation::differentiatedTransformationTranslation(int ti) const{
	assert3D(ti);
	Eigen::Matrix4d m;
	m.setZero();
	const auto& transfMatrix(this->getMatrix());

	switch (ti) {
	case 0:
		m(0,3) = -transfMatrix(0,0);
		m(1,3) = -transfMatrix(1,0);
		m(2,3) = -transfMatrix(2,0);
		m(3,3) = getScaleFactor();	//Scale is on this place by definition
		break;
	case 1:
		m(0,3) = -transfMatrix(0,1);
		m(1,3) = -transfMatrix(1,1);
		m(2,3) = -transfMatrix(2,1);
		m(3,3) = getScaleFactor();	//Scale is on this place by definition
		break;
	case 2:
		m(0,3) = -transfMatrix(0,2);
		m(1,3) = -transfMatrix(1,2);
		m(2,3) = -transfMatrix(2,2);
		m(3,3) = getScaleFactor();	//Scale is on this place by definition
		break;
	}
	return TDerivativeTransformation(m);
}

TDerivativeTransformation TInverseTransformation::differentiatedTransformationScaleFactor() const{
	Eigen::Matrix4d m(*fTransM);
	
	//Matrice remains unchanged, only on m(3,3) position is -scale^2
	m(3,3) = - pow2q(getScaleFactor());
	return TDerivativeTransformation(m);
}


bool TInverseTransformation::transform(TRotationMatrix& rm) const{
	TCoordSysFactory::ECoordSys k3DCart = TCoordSysFactory::ECoordSys::k3DCartesian;
	TANumericValue::EStatus kNull = TANumericValue::EStatus::kNull;
	bool result = false;

	//If status is not null and matrix is in a 3d cart. coordinates.
	if(rm.getStatus()!=kNull && rm.getCoordSys()==k3DCart){

		//(TRotationMatrix is not affected by translation and scaling), just multiplication of rotation matrices.
		TRotationMatrix transformation(TRotationMatrix::ERotationType::kRzyx, getAngle(0).rad(), getAngle(1).rad(), getAngle(2).rad());
		//Transform 
		rm = transformation.transposed() * rm;	
		result = true;
	}
	else{
		rm.setStatus(kNull);
	}
	return result;
}
