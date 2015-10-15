/////////////////////////////////////////////////////////////////////////////
//THelmertTransformation.cpp
//
/** Classe pour une transformation helmert X1 = Fact*R*X+T*/
//
// Copyright 2000-2014 CERN SU, M.Jones. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "THelmertTransformation.h"
#include<iostream>
#include<Eigen/Dense>

////////////////////////////////////////////////////////////////



struct THelmertTransformation::D {
	// Transformation matrix in homogeneous coordinates
	std::unique_ptr<Eigen::Matrix4d> m;
	//Name of the transformation
	std::string name;

	D() :
	m(std::unique_ptr<Eigen::Matrix4d>(new Eigen::Matrix4d)){}
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


THelmertTransformation::THelmertTransformation(const std::string& name) :
	d(std::unique_ptr<D>(new D)){

	auto& m(*(d->m));
	m.setIdentity();
	d->name = name;

	line = -1;
}


THelmertTransformation::THelmertTransformation(const THelmertTransformation &transformation):
	d(std::unique_ptr<D>(new D)){
	line = transformation.line;
	d->name = transformation.getName();
	*(d->m) = *(transformation.d->m);
}

THelmertTransformation::~THelmertTransformation() {}

void THelmertTransformation::setName(const std::string& name){
	d->name = name;
}

void THelmertTransformation::setScaleFactor(TReal scale){
	zerofy(scale);
	if(scale == 0.0){
		throw std::runtime_error("Scale factor is zero or a very small number!");
	}
	auto& m(*(d->m));
	m(3,3) = 1.0/scale;
}
#if 0
void THelmertTransformation::setAngle(int axis, const LGC::TAngle& angle){
	assert3D(axis);
	LGC::TAngle xAngle;
	LGC::TAngle yAngle;
	LGC::TAngle zAngle;

	if (axis == 0){
	    yAngle = getAngle(1);
		zAngle = getAngle(2);
		setRotationPart(angle, yAngle , zAngle);
	}
	else if(axis == 1){
		xAngle = getAngle(0);
		zAngle = getAngle(2);
		setRotationPart(xAngle, angle , zAngle);
	}
	else{
		xAngle = getAngle(0);
		yAngle = getAngle(1);
		setRotationPart(xAngle, yAngle , angle);
	}
}
#endif

void THelmertTransformation::setAngle(int axis, const LGC::TAngle& angle){
	assert3D(axis);

	RotAngles angles = getAngles();
	
	if (axis == 0){
		setRotationPart(angle, angles.phi , angles.kappa, angles.kR);
	}
	else if(axis == 1){
		setRotationPart(angles.omega, angle , angles.kappa, angles.kR);
	}
	else{
		setRotationPart(angles.omega, angles.phi , angle, angles.kR);
	}
}


void THelmertTransformation::setTranslation(int axis, TReal t){
	assert3D(axis);
	auto& m(*(d->m));
	m(axis,3) = t;
}


void THelmertTransformation::setIdentityTransformation(){
	auto& m(*(d->m));

	m(0,0) = 1.0;
	m(0,1) = 0.0;
	m(0,2) = 0.0;
	m(0,3) = 0.0;
	m(1,0) = 0.0;
	m(1,1) = 1.0;
	m(1,2) = 0.0;
	m(1,3) = 0.0;
	m(2,0) = 0.0;
	m(2,1) = 0.0;
	m(2,2) = 1.0;
	m(2,3) = 0.0;
	m(3,0) = 0.0; 
	m(3,1) = 0.0;
	m(3,2) = 0.0;
	m(3,3) = 1.0;
}


const std::string& THelmertTransformation::getName() const{
	return d->name;
}

TReal THelmertTransformation::getScaleFactor() const{
	auto& m(*(d->m));
	return 1.0/m(3,3);
}

#if 0
LGC::TAngle THelmertTransformation::getAngle(int axis) const{
	LGC::TAngle ang(LGC::TAngle::kRadians, NO_VALf);
	assert3D(axis);
	auto& m(*(d->m));

	if (axis == 0){
		if (isZero(m(2,2))  && isZero(m(2,1))){
			TReal a = atan2(-m(1,0), m(1,1));
			return  ang.atan2(m(1,2)*cos(a),m(1,1)); //Can not restore rotation around X-axis, using Ryxz order with different angles
		}
		else
			return ang.atan2(- m(2,1), m(2,2));
	}

	if (axis == 1){
		if( isZero(m(2,0)) &&  isZero(m(0,0)) && isZero(m(1,0)))
			return ang.atan2(- m(0,2), m(2,2)); //Can not restore rotation around Y-axis, using Ryxz order with different angles
		else 
			return ang.atan2( m(2,0), sqrtq(pow2(m(0,0)) + pow2(m(1,0))));
	}

	if (axis == 2){
		if(isZero(m(0,0)) && isZero(m(1,0)))
			return ang.atan2(- m(1,0), m(1,1)); //Can not restore rotation around Z-axis, using Ryxz order with different angles
		else
			return ang.atan2(-m(1,0), m(0,0));
	}
	return ang;
}
#endif

LGC::TAngle THelmertTransformation::getAngle(int axis) const{
	LGC::TAngle ang(LGC::TAngle::kRadians, NO_VALf);
	assert3D(axis);
	auto& m(*(d->m));

	if( (isZero(m(2,2))  && isZero(m(2,1))) || ( isZero(m(2,0)) &&  isZero(m(0,0)) && isZero(m(1,0))) || (isZero(m(0,0)) && isZero(m(1,0)))){
		if(axis == 0)
			return ang.atan2(m(1,2), sqrtq(pow2(m(1,0)) + pow2(m(1,1))));
		else if(axis == 1)
			return ang.atan2(- m(0,2), m(2,2));
		else
			return ang.atan2(- m(1,0), m(1,1));
	}
	else{
		if(axis == 0)
			return ang.atan2(- m(2,1), m(2,2));
		else if(axis == 1)
			return ang.atan2( m(2,0), sqrtq(pow2(m(0,0)) + pow2(m(1,0))));
		else
			return ang.atan2(-m(1,0), m(0,0));
	}
}

RotAngles THelmertTransformation::getAngles() const{
	RotAngles angles;

	auto& m(*(d->m));
	//If following condition is true, ZYX matr. representation produces a failure case, and we need to use YXZ representation
	if( (isZero(m(2,2))  && isZero(m(2,1))) || ( isZero(m(2,0)) &&  isZero(m(0,0)) && isZero(m(1,0))) || (isZero(m(0,0)) && isZero(m(1,0)))){
		angles. omega = angles.omega.atan2(m(1,2), sqrtq(pow2(m(1,0)) + pow2(m(1,1))));
		angles.phi = angles.phi.atan2(- m(0,2), m(2,2));
		angles.kappa = angles.kappa.atan2(- m(1,0), m(1,1));
		angles.kR = kRyxz;
	}
	else{
		angles.omega = angles.omega.atan2(- m(2,1), m(2,2));
		angles.phi = angles.phi.atan2( m(2,0), sqrtq(pow2(m(0,0)) + pow2(m(1,0))));
		angles.kappa = angles.kappa.atan2(-m(1,0), m(0,0));
		angles.kR = kRzyx;
	}
	return angles;
}

TReal THelmertTransformation::getTranslation(int axis) const{
	assert3D(axis);
	auto& m(*(d->m));
	return m(axis,3);
}

TReal THelmertTransformation::getMmatrixIJPosition(int row, int column) const{
	assert4D(row);
	assert4D(column);
	auto& m(*(d->m));
	return m(row,column);
}

bool THelmertTransformation::transform(TPositionVector& pv) const{
	TCoordSysFactory::ECoordSys k3DCart = TCoordSysFactory::ECoordSys::k3DCartesian;
	TANumericValue::EStatus kNull = TANumericValue::EStatus::kNull;
	bool result = false;

	//If status is not null and vector is in a 3d cart. coordinates.
	if(pv.getStatus()!=kNull && pv.getCoordSys() == k3DCart){
	Eigen::Vector4d pTemp(pv.getX().getValue() ,pv.getY().getValue(),pv.getZ().getValue(),1.0);
	//Transform
	Eigen::Vector4d pResult = *(d->m) * pTemp;

	//pResult is a result of the point transformation in homogeneus coordinates
	//If w (scale) coordinate is 0, than it is a point at infinity.
	zerofy(pResult[3]);
	if(pResult[3] == 0.0){
		throw std::runtime_error("Transformed TPositionVector is a point at infinity");
	}

	//Transform point in homogeneus coordinates back to cartesian coordinates.
	pv.setX(pResult[0]/pResult[3]);
	pv.setY(pResult[1]/pResult[3]);
	pv.setZ(pResult[2]/pResult[3]);
	result = true;
	}
	else{
		pv.setStatus(kNull);
	}
	return result;
}

bool THelmertTransformation::transform(TFreeVector& fv) const{
	TCoordSysFactory::ECoordSys k3DCart = TCoordSysFactory::ECoordSys::k3DCartesian;
	TANumericValue::EStatus kNull = TANumericValue::EStatus::kNull;
	bool result = false;
	//Copy of the transformation made, we need to change its translation part (TFreeVector is not affected by translation) but not to affect the transformation itself
	THelmertTransformation transformation(*this);

	//If status is not null and vector is in a 3d cart. coordinates.
	if(fv.getStatus()!=kNull && fv.getCoordSys()==k3DCart){
	Eigen::Vector4d pTemp(fv.getX().getValue() ,fv.getY().getValue(),fv.getZ().getValue(),1.0);
	//TFreeVector is not affected by translation
	transformation.setTranslationPart(0.0,0.0,0.0);
	//Transform
	Eigen::Vector4d pResult = *(transformation.d->m) * pTemp;

	//pResult is a result of the point transformation in homogeneus coordinates
	//If w (scale) coordinate is 0, than it is a point at infinity
	zerofy(pResult[3]);
	if(pResult[3] == 0.0){
		throw std::runtime_error("Transformed TFreeVector is a point at infinity");
	}

	//Transform point in homogeneus coordinates to cartesian coordinates
	fv.setX(pResult[0]/pResult[3]);
	fv.setY(pResult[1]/pResult[3]);
	fv.setZ(pResult[2]/pResult[3]);
	result = true;
	}
	else{
		fv.setStatus(kNull);
	}
	return result;
}

bool THelmertTransformation::transform(TRotationMatrix& rm) const{
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

THelmertTransformation& THelmertTransformation::invert(){
	*(d->m) = d->m->inverse().eval();
	return *this;
}

THelmertTransformation THelmertTransformation::operator*(const THelmertTransformation& trans){
	THelmertTransformation t(*this);
	*(t.d->m) = *(d->m) * *(trans.d->m);
	return t;
}

THelmertTransformation& THelmertTransformation::operator=(const THelmertTransformation& rhs) {
	*(d->m) = *(rhs.d->m);
	d->name = rhs.d->name;
	return *this;
}

THelmertTransformation*  THelmertTransformation::clone() const
{// Return a pointer to a clone of this transformation
	return new THelmertTransformation( *this );
}

void THelmertTransformation::setRotationPart(const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz, ERotationType rt){
	TReal xcos = rx.cos();
	zerofy(xcos);

	TReal xsin = rx.sin();
	zerofy(xsin);

	TReal ycos = ry.cos();
	zerofy(ycos);

	TReal ysin = ry.sin();
	zerofy(ysin);

	TReal zcos = rz.cos();
	zerofy(zcos);

	TReal zsin = rz.sin();
	zerofy(zsin);

	auto& m(*(d->m));

	switch(rt){
	case(kRzyx):
		m(0,0) =  zcos*ycos;
		m(0,1) =  zsin*xcos+zcos*ysin*xsin;
		m(0,2)  =  zsin*xsin-zcos*ysin*xcos;
		m(1,0)  = -zsin*ycos;
		m(1,1)  =  zcos*xcos-zsin*ysin*xsin;
		m(1,2)  =  zcos*xsin+zsin*ysin*xcos;
		m(2,0)  =  ysin;
		m(2,1)  = -ycos*xsin;
		m(2,2)  =  ycos*xcos;
		break; 
	case (kRyxz):
		m(0,0) =  zcos*ycos - ysin*xsin*zsin;
		m(0,1) =  ycos*zsin + ysin*xsin*zcos;
		m(0,2)  =  -ysin*xcos;
		m(1,0)  = -zsin*xcos;
		m(1,1)  =  zcos*xcos;
		m(1,2)  =  xsin;
		m(2,0)  =  ysin*zcos + xsin*ycos*zsin;
		m(2,1)  =  ysin*zsin - xsin*ycos*zcos;
		m(2,2)  =  xcos*ycos;
		break;
	}
}

void THelmertTransformation::setTranslationPart(const TReal tx, const TReal ty, const TReal tz){
	auto& m(*(d->m));
	m(0,3) = tx;
	m(1,3) = ty;
	m(2,3) = tz;
}

THelmertTransformation THelmertTransformation::differentiatedTransformationAngle(int angle) const{
	assert3D(angle);
	THelmertTransformation  t(*this);
	auto& m(*(t.d->m));

	RotAngles angles = this->getAngles();


	TReal omegaCos = angles.omega.cos();
	zerofy(omegaCos);

	TReal omegaSin = angles.omega.sin();
	zerofy(omegaSin);

	TReal phiCos = angles.phi.cos();
	zerofy(phiCos);

	TReal phiSin = angles.phi.sin();
	zerofy(phiSin);

	TReal kappaCos = angles.kappa.cos();
	zerofy(kappaCos);

	TReal kappaSin = angles.kappa.sin();
	zerofy(kappaSin);

	t.setZeroMatrix();

if(angles.kR == kRzyx){
	switch (angle) {
	case 0:
			m(0,1) = - omegaSin*kappaSin + omegaCos*kappaCos*phiSin;
			m(0,2) = omegaCos*kappaSin+omegaSin*kappaCos*phiSin;
			m(1,1) = -omegaSin*kappaCos-omegaCos*kappaSin*phiSin;
			m(1,2) = omegaCos*kappaCos-omegaSin*kappaSin*phiSin;
			m(2,1) = -omegaCos*phiCos;
			m(2,2) = -omegaSin*phiCos;
			break;
	case 1:
			m(0,0) = -phiSin*kappaCos;
			m(0,1) = phiCos*omegaSin*kappaCos;
			m(0,2) = -phiCos*kappaCos*omegaCos;
			m(1,0) = phiSin*kappaSin;
			m(1,1) = -phiCos*kappaSin*omegaSin;
			m(1,2) = phiCos*kappaSin*omegaCos;
			m(2,0) = phiCos;
			m(2,1) = omegaSin*phiSin;
			m(2,2) = -phiSin*omegaCos;
			break;
	case 2:
   			m(0,0) = -kappaSin*phiCos;
			m(0,1) = kappaCos*omegaCos-kappaSin*phiSin*omegaSin;
			m(0,2) = kappaCos*omegaSin+kappaSin*phiSin*omegaCos;
			m(1,0) = -kappaCos*phiCos;
			m(1,1) = -kappaSin*omegaCos-kappaCos*phiSin*omegaSin;
			m(1,2) = -kappaSin*omegaSin+kappaCos*phiSin*omegaCos;
			break;
	}
}
else if(angles.kR == kRyxz)
	throw std::runtime_error("Partial derivatives for rotation order Ryxz, not yet implemented");
//Implement partial derivatives with respect to each angle for YXZ rotation order

	return t;
}

THelmertTransformation THelmertTransformation::differentiatedTransformationAngleInverse(int angle) const{
	assert3D(angle);
	THelmertTransformation  t(*this);
	auto& m(*(t.d->m));


	auto inverseTrafo(this->inverse());


	RotAngles angles = inverseTrafo.getAngles();


	TReal omegaCos = angles.omega.cos();
	zerofy(omegaCos);

	TReal omegaSin = angles.omega.sin();
	zerofy(omegaSin);

	TReal phiCos = angles.phi.cos();
	zerofy(phiCos);

	TReal phiSin = angles.phi.sin();
	zerofy(phiSin);

	TReal kappaCos = angles.kappa.cos();
	zerofy(kappaCos);

	TReal kappaSin = angles.kappa.sin();
	zerofy(kappaSin);

	t.setZeroMatrix();

	//TReal transl1 = getTranslation(0);
	//TReal transl2 = getTranslation(1);
	//TReal transl3 = getTranslation(2);

	TReal transl1 = inverseTrafo.getTranslation(0);
	TReal transl2 = inverseTrafo.getTranslation(1);
	TReal transl3 = inverseTrafo.getTranslation(2);


if(angles.kR == kRzyx){
	switch (angle) {
	case 0:
			m(1,0) = - omegaSin*kappaSin + omegaCos*kappaCos*phiSin;
			m(2,0) = omegaCos*kappaSin + omegaSin*kappaCos*phiSin;
			m(1,1) = -omegaSin*kappaCos-omegaCos*kappaSin*phiSin;
			m(2,1) = omegaCos*kappaCos-omegaSin*kappaSin*phiSin;
			m(1,2) = -omegaCos*phiCos;
			m(2,2) = -omegaSin*phiCos;
			break;
	case 1:
			m(0,0) = -phiSin*kappaCos;
			m(1,0) = phiCos*omegaSin*kappaCos;
			m(2,0) = -phiCos*kappaCos*omegaCos;
			m(0,1) = phiSin*kappaSin;
			m(1,1) = -phiCos*kappaSin*omegaSin;
			m(2,1) = phiCos*kappaSin*omegaCos;
			m(0,2) = phiCos;
			m(1,2) = omegaSin*phiSin;
			m(2,2) = -phiSin*omegaCos;
			break;
	case 2:
   			m(0,0) = -kappaSin*phiCos;
			m(1,0) = kappaCos*omegaCos-kappaSin*phiSin*omegaSin;
			m(2,0) = kappaCos*omegaSin+kappaSin*phiSin*omegaCos;
			m(0,1) = -kappaCos*phiCos;
			m(1,1) = -kappaSin*omegaCos-kappaCos*phiSin*omegaSin;
			m(2,1) = -kappaSin*omegaSin+kappaCos*phiSin*omegaCos;
			break;
	}

	m(0,3) = - (m(0,0)*transl1 + m(0,1)*transl2 + m(0,2)*transl3);
	m(1,3) = - (m(1,0)*transl1 + m(1,1)*transl2 + m(1,2)*transl3);
	m(2,3) = - (m(2,0)*transl1 + m(2,1)*transl2 + m(2,2)*transl3);
}
else if(angles.kR == kRyxz)
	throw std::runtime_error("Partial derivatives for rotation order Ryxz, not yet implemented");
	//Implement partial derivatives with respect to each angle for YXZ rotation order

	return t;
}

THelmertTransformation THelmertTransformation::differentiatedTransformationTranslation(int ti) const{
	assert3D(ti);
	THelmertTransformation t(*this);
	auto& m(*(t.d->m));

	t.setZeroMatrix();

	if(ti == 0)
		m(0,3) = 1.0/getScaleFactor();
	else if(ti == 1)
		m(1,3) = 1.0/getScaleFactor();
	else if(ti == 2)
		m(2,3) = 1.0/getScaleFactor();

	return t;
}

THelmertTransformation THelmertTransformation::differentiatedTransformationTranslationInverse(int ti) const{
	assert3D(ti);
	THelmertTransformation t(*this);
	auto& m(*(t.d->m));
	t.setZeroMatrix();

	auto inverseTrafo(this->inverse());
	auto& mOrig(*(inverseTrafo.d->m));

	if(ti == 0){
		m(0,3) = -mOrig(0,0);
		m(1,3) = -mOrig(0,1);
		m(2,3) = -mOrig(0,2);
	}
	else if(ti == 1){
		m(0,3) = -mOrig(1,0);
		m(1,3) = -mOrig(1,1);
		m(2,3) = -mOrig(1,2);
	}
	else if(ti == 2){
		m(0,3) = -mOrig(2,0);
		m(1,3) = -mOrig(2,1);
		m(2,3) = -mOrig(2,2);
	}
	return t;
}


THelmertTransformation THelmertTransformation::differentiatedTransformationScaleFactor() const{
	THelmertTransformation t(*this);
	auto& m(*(t.d->m));

	t.setZeroMatrix();

	TReal scale = this->getScaleFactor();
	TReal scaleCoeff = -((1.0)/(scale*scale));

	m(0,3) = scaleCoeff*getTranslation(0);
	m(1,3) = scaleCoeff*getTranslation(1);
	m(2,3) = scaleCoeff*getTranslation(2);
	m(3,3) = scaleCoeff;
	return t;
}


THelmertTransformation THelmertTransformation::differentiatedTransformationScaleFactorInverse() const{
	THelmertTransformation t(*this);
	auto& m(*(t.d->m));

	t.setZeroMatrix();
	m(3,3) = 1.0;
	return t;
}

void THelmertTransformation::setZeroMatrix(){
	auto& m(*(d->m));
	m(0,0) = 0.0;
	m(0,1) = 0.0;
	m(0,2) = 0.0;
	m(0,3) = 0.0;
	m(1,0) = 0.0;
	m(1,1) = 0.0;
	m(1,2) = 0.0;
	m(1,3) = 0.0;
	m(2,0) = 0.0;
	m(2,1) = 0.0;
	m(2,2) = 0.0;
	m(2,3) = 0.0;
	m(3,0) = 0.0; 
	m(3,1) = 0.0;
	m(3,2) = 0.0;
	m(3,3) = 0.0;
}

void THelmertTransformation::setMatrixIJPosition(int row, int column, TReal value){
	auto& m(*(d->m));
	m(row, column) = value;

}