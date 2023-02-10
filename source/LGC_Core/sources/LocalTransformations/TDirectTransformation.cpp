#include "TDirectTransformation.h"
#include "TDerivativeTransformation.h"


TDirectTransformation::TDirectTransformation():TTransformation(){setIdentityParams();}

TDirectTransformation::TDirectTransformation(const TransformParameters &fParams)
{
	this->setTransformParam(fParams);
}

TDirectTransformation::TDirectTransformation(const TTransformation &transformation): TTransformation(transformation){
}

TDirectTransformation::~TDirectTransformation(){}

void TDirectTransformation::setIdentityParams(){
	fParameters.setIdentity();
}

void TDirectTransformation::setTranslationPart(const TReal tx, const TReal ty, const TReal tz){
	auto& m(*(fTransM));
	TReal scl = getScaleFactor();

	m(0,3) = tx/scl;
	m(1,3) = ty/scl;
	m(2,3) = tz/scl;
}

void TDirectTransformation::setRotationPart(const TAngle& rx, const TAngle& ry, const TAngle& rz){
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
	m(0,1)  =  kappasin*phicos;
	m(0,2)  =  -phisin;
	m(1,0)  =  kappacos*phisin*omegasin-kappasin*omegacos;
	m(1,1)  =  kappasin*phisin*omegasin+kappacos*omegacos;
	m(1,2)  =  phicos*omegasin;
	m(2,0)  =  kappacos*phisin*omegacos+kappasin*omegasin;
	m(2,1)  =  kappasin*phisin*omegacos-kappacos*omegasin;
	m(2,2)  =  phicos*omegacos;

}

void TDirectTransformation::setScaleFactor(TReal scale){
	auto& m(*(fTransM));
	zerofy(scale);
	if(scale == 0.0)
		throw std::runtime_error("Scale factor is zero or a very small number!");
	
	m(3,3) = 1.0/scale;
}

void TDirectTransformation::setTransformParam(const TransformParameters& params){
	fParameters = params;
	this->setRotationPart(params.omega, params.phi, params.kappa);
	this->setTranslationPart(params.tX, params.tY, params.tZ);
	this->setScaleFactor(params.scale);
}


void TDirectTransformation::setIdentityTransformation(){
	fParameters.omega.setRadiansValue(0);
	fParameters.phi.setRadiansValue(0);
	fParameters.kappa.setRadiansValue(0);

	fParameters.tX = TLength(0.0);
	fParameters.tY = TLength(0.0);
	fParameters.tZ = TLength(0.0);

	fParameters.scale = 1.0;

	this->setRotationPart(fParameters.omega, fParameters.phi, fParameters.kappa);
	this->setTranslationPart(fParameters.tX, fParameters.tY, fParameters.tZ);
	this->setScaleFactor(fParameters.scale);
}

void TDirectTransformation::setRotationTransformation(TReal rx, TReal ry, TReal rz){
	fParameters.omega.setRadiansValue(rx);
	fParameters.phi.setRadiansValue(ry);
	fParameters.kappa.setRadiansValue(rz);

	this->setRotationPart(fParameters.omega, fParameters.phi, fParameters.kappa);
}

void TDirectTransformation::setTransformation(TReal tx, TReal ty, TReal tz, TReal rx, TReal ry, TReal rz, TReal scale){
	fParameters.omega.setRadiansValue(rx);
	fParameters.phi.setRadiansValue(ry);
	fParameters.kappa.setRadiansValue(rz);

	fParameters.tX = TLength(tx);
	fParameters.tY = TLength(ty);
	fParameters.tZ = TLength(tz);

	fParameters.scale = scale;

	this->setRotationPart(fParameters.omega, fParameters.phi, fParameters.kappa);
	this->setTranslationPart(fParameters.tX, fParameters.tY, fParameters.tZ);
	this->setScaleFactor(fParameters.scale);
}

TReal TDirectTransformation::getScaleFactor() const{
	return fParameters.scale;
}

const TAngle& TDirectTransformation::getAngle(int axis) const{
	TAngle ang(NO_VALf, TAngle::kRadians);
	assert3D(axis);

		if(axis == 0)
			return fParameters.omega;
		else if(axis == 1)
			return fParameters.phi;
		else
			return fParameters.kappa;
}

TReal TDirectTransformation::getTranslation(int axis) const{
	assert3D(axis);
	if(axis == 0)
		return fParameters.tX;
	else if(axis == 1)
		return fParameters.tY;
	else
		return fParameters.tZ;
}



TDerivativeTransformation TDirectTransformation::differentiatedTransformationAngle(int angle) const{
	assert3D(angle);
	Eigen::Matrix4d m;
	m.setZero();

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

TDenseMatrix TDirectTransformation::getPartialDerivativeWrtPosition() const
{
	// the partial derivative wrt position is the M matrix (=rotation part) times the scale factor
	TDenseMatrix posDeriv(3, 3);
	posDeriv = (getMatrix().topLeftCorner(3,3)) * getScaleFactor();
	return posDeriv;
}

TDenseMatrix TDirectTransformation::getPartialDerivativeWrtHelmertParameters(const TPositionVector& pos) const
{
	// we use the derivative transformations to compute the partial derivatives
	TDerivativeTransformation dt1 = differentiatedTransformationTranslation(0);
	TDerivativeTransformation dt2 = differentiatedTransformationTranslation(1);
	TDerivativeTransformation dt3 = differentiatedTransformationTranslation(2);
	TDerivativeTransformation domega = differentiatedTransformationAngle(0);
	TDerivativeTransformation dphi = differentiatedTransformationAngle(1);
	TDerivativeTransformation dkappa = differentiatedTransformationAngle(2);
	TDerivativeTransformation dscale = differentiatedTransformationScaleFactor();

	auto toVector = [](TFreeVector vIn) {
		TVector vector(3);
		vector << vIn.getX(), vIn.getY(), vIn.getZ();
		return vector;
	};
	TDenseMatrix helmertDeriv(3, 7);
	helmertDeriv.setZero();
	helmertDeriv.col(0) = toVector(dt1 * pos);
	helmertDeriv.col(1) = toVector(dt2 * pos);
	helmertDeriv.col(2) = toVector(dt3 * pos);
	helmertDeriv.col(3) = toVector(domega * pos);
	helmertDeriv.col(4) = toVector(dphi * pos);
	helmertDeriv.col(5) = toVector(dkappa * pos);
	helmertDeriv.col(6) = toVector(dscale * pos);

	return helmertDeriv;
}

bool TDirectTransformation::transform(TRotationMatrix& rm) const{
	TCoordSysFactory::ECoordSys k3DCart = TCoordSysFactory::ECoordSys::k3DCartesian;
	bool result = false;

	//If status is not null and matrix is in a 3d cart. coordinates.
	if(rm.isInitialise() && rm.getCoordSys()==k3DCart){

		//(TRotationMatrix is not affected by translation and scaling), just multiplication of rotation matrices.
		TRotationMatrix transformation(TRotationMatrix::ERotationType::kRzyx, getAngle(0).getRadiansValue(), getAngle(1).getRadiansValue(), getAngle(2).getRadiansValue());
		//Transform
		rm = transformation * rm;	
		result = true;
	}

	return result;
}


