#include "TInverseTransformation.h"

TInverseTransformation::TInverseTransformation():TTransformation(){setIdentityParams();}


TInverseTransformation::TInverseTransformation(const TransformParameters &fParams) 
{
	this->setTransformParam(fParams);
}


TInverseTransformation::TInverseTransformation(const TTransformation &transformation): TTransformation(transformation){
}


TInverseTransformation::~TInverseTransformation(){}

void TInverseTransformation::setIdentityParams(){
	fParameters.setIdentity();
}

void TInverseTransformation::setTranslationPart(const TransformParameters& params){
	auto& m(*(fTransM));

	TReal omegacos = params.omega.cosine();
	zerofy(omegacos);

	TReal omegasin = params.omega.sine();
	zerofy(omegasin);

	TReal phicos = params.phi.cosine();
	zerofy(phicos);

	TReal phisin = params.phi.sine();
	zerofy(phisin);

	TReal kappacos = params.kappa.cosine();
	zerofy(kappacos);

	TReal kappasin = params.kappa.sine();
	zerofy(kappasin);

	m(0,3) = -(kappacos*phicos)*params.tX - (kappacos*phisin*omegasin-kappasin*omegacos)*params.tY - (kappacos*phisin*omegacos+kappasin*omegasin)*params.tZ;
	m(1,3) = -(kappasin*phicos)*params.tX - (kappasin*phisin*omegasin+kappacos*omegacos)*params.tY - (kappasin*phisin*omegacos-kappacos*omegasin)*params.tZ;
	m(2,3) = -(-phisin)*params.tX - (phicos*omegasin)*params.tY - (phicos*omegacos)*params.tZ;

}

void TInverseTransformation::setRotationPart(const TAngle& rx, const TAngle& ry, const TAngle& rz){
	auto& m(*(fTransM));
	TReal omegacos = rx.cosine();
	zerofy(omegacos);

	TReal omegasin = rx.sine();
	zerofy(omegasin);

	TReal phicos = ry.cosine();
	zerofy(phicos);

	TReal phisin = ry.sine();
	zerofy(phisin);

	TReal kappacos = rz.cosine();
	zerofy(kappacos);

	TReal kappasin = rz.sine();
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

void TInverseTransformation::setScaleFactor(TReal scale){
	auto& m(*(fTransM));
	zerofy(scale);
	m(3,3) = scale;
}

void TInverseTransformation::setTransformParam(const TransformParameters& params){
	fParameters = params;
	this->setRotationPart(params.omega, params.phi, params.kappa);
	this->setTranslationPart(params);
	this->setScaleFactor(params.scale);
}

void TInverseTransformation::setIdentityTransformation(){
	fParameters.omega.setRadiansValue(0);
	fParameters.phi.setRadiansValue(0);
	fParameters.kappa.setRadiansValue(0);

	fParameters.tX = TLength(0.0);
	fParameters.tY = TLength(0.0);
	fParameters.tZ = TLength(0.0);

	fParameters.scale = 1.0;

	this->setRotationPart(fParameters.omega, fParameters.phi, fParameters.kappa);
	this->setTranslationPart(fParameters);
	this->setScaleFactor(fParameters.scale);
}

void TInverseTransformation::setRotationTransformation(TReal rx, TReal ry, TReal rz){
	fParameters.omega.setRadiansValue(rx);
	fParameters.phi.setRadiansValue(ry);
	fParameters.kappa.setRadiansValue(rz);

	this->setRotationPart(fParameters.omega, fParameters.phi, fParameters.kappa);
}

void TInverseTransformation::setTransformation(TReal tx, TReal ty, TReal tz, TReal rx, TReal ry, TReal rz, TReal scale){
	fParameters.omega.setRadiansValue(rx);
	fParameters.phi.setRadiansValue(ry);
	fParameters.kappa.setRadiansValue(rz);

	fParameters.tX = TLength(tx);
	fParameters.tY = TLength(ty);
	fParameters.tZ = TLength(tz);

	fParameters.scale = scale;

	this->setRotationPart(fParameters.omega, fParameters.phi, fParameters.kappa);
	this->setTranslationPart(fParameters);
	this->setScaleFactor(fParameters.scale);
}

TReal TInverseTransformation::getScaleFactor() const{
	return fParameters.scale;
}

const TAngle& TInverseTransformation::getAngle(int axis) const{
	TAngle ang(NO_VALf, TAngle::kRadians);
	assert3D(axis);

	if(axis == 0)
		return fParameters.omega;
	else if(axis == 1)
		return fParameters.phi;
	else
		return fParameters.kappa;
}

TReal TInverseTransformation::getTranslation(int axis) const{
	assert3D(axis);
	if(axis == 0)
		return fParameters.tX;
	else if(axis == 1)
		return fParameters.tY;
	else
		return fParameters.tZ;
}

TDerivativeTransformation TInverseTransformation::differentiatedTransformationAngle(int angle) const{

	assert3D(angle);
	Eigen::Matrix4d m;
	m.setZero();
	
	TReal t1 = this->getTranslation(0);
	TReal t2 = this->getTranslation(1);
	TReal t3 = this->getTranslation(2);

	TReal omegaCos = fParameters.omega.cosine();
	zerofy(omegaCos);

	TReal omegaSin = fParameters.omega.sine();
	zerofy(omegaSin);

	TReal phiCos = fParameters.phi.cosine();
	zerofy(phiCos);

	TReal phiSin = fParameters.phi.sine();
	zerofy(phiSin);

	TReal kappaCos = fParameters.kappa.cosine();
	zerofy(kappaCos);

	TReal kappaSin = fParameters.kappa.sine();
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
			m(0, 3) = -m(0, 0) * t1 - m(0, 1) * t2 - m(0, 2) * t3;
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
	bool result = false;

	//If status is not null and matrix is in a 3d cart. coordinates.
	if(rm.isInitialise() && rm.getCoordSys()==k3DCart){

		//(TRotationMatrix is not affected by translation and scaling), just multiplication of rotation matrices.
		TRotationMatrix transformation(TRotationMatrix::ERotationType::kRzyx, getAngle(0).getRadiansValue(), getAngle(1).getRadiansValue(), getAngle(2).getRadiansValue());
		//Transform 
		rm = transformation.transposed() * rm;	
		result = true;
	}
	return result;
}
