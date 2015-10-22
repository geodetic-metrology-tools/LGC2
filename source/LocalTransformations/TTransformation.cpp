#include "TTransformation.h"

TTransformation::TTransformation()  :
	fTransM(std::unique_ptr<Eigen::Matrix4d>(new Eigen::Matrix4d))
{
	fTransM->setIdentity();
}

TTransformation::TTransformation(const TTransformation &transformation) : 
	fTransM(std::unique_ptr<Eigen::Matrix4d>(new Eigen::Matrix4d(*transformation.fTransM)))
{}



TTransformation TTransformation::getInversedTransformation() const{
	TTransformation t(*this);
	*(t.fTransM) = t.fTransM->inverse().eval();
	return t;
}


TTransformation::~TTransformation() {}


void TTransformation::setIdentityTransformation(){
	fTransM->setIdentity();
}

void TTransformation::setTransformation(TReal tx, TReal ty, TReal tz, TReal rx, TReal ry, TReal rz, TReal scale){
	//XYZ order
	setRotationTransformation(rx,ry,rz);
	auto& m(*(fTransM));

	zerofy(scale);
	if(scale == 0.0)
		throw std::runtime_error("Scale factor is zero or a very small number!");
	
	m(3,3) = 1.0/scale;

	/*Set translation part*/
	m(0,3) = tx/scale;
	m(1,3) = ty/scale;
	m(2,3) = tz/scale;

// Might be interesting to be able to switch, not enabled yet 
#if 0
	//ZYX order
	auto& m(*(fTransM));
	TReal omegacos = cosq(rx);
	zerofy(omegacos);

	TReal omegasin = sinq(rx);
	zerofy(omegasin);

	TReal phicos = cosq(ry);
	zerofy(phicos);

	TReal phisin = sinq(ry);
	zerofy(phisin);

	TReal kappacos = cosq(rz);
	zerofy(kappacos);

	TReal kappasin = sinq(rz);
	zerofy(kappasin);

	m(0,0)  =  kappacos*phicos;
	m(0,1)  =  kappasin*omegacos + kappacos*phisin*omegasin;
	m(0,2)  =  kappasin*omegasin-kappacos*phisin*omegacos;
	m(1,0)  =   -kappasin*phicos;
	m(1,1)  =  -kappasin*phisin*omegasin+kappacos*omegacos;
	m(1,2)  =  kappacos*omegasin+kappasin*phisin*omegacos;
	m(2,0)  =  phisin;
	m(2,1)  =  -phicos*omegasin;
	m(2,2)  =  phicos*omegacos;


	zerofy(scale);
	if(scale == 0.0)
		throw std::runtime_error("Scale factor is zero or a very small number!");
	
	m(3,3) = 1.0/scale;

	/*Set translation part*/
	m(0,3) = tx/scale;
	m(1,3) = ty/scale;
	m(2,3) = tz/scale;
#endif
}

void TTransformation::setRotationTransformation(TReal rx, TReal ry, TReal rz){
	auto& m(*(fTransM));
	TReal omegacos = cosq(rx);
	zerofy(omegacos);

	TReal omegasin = sinq(rx);
	zerofy(omegasin);

	TReal phicos = cosq(ry);
	zerofy(phicos);

	TReal phisin = sinq(ry);
	zerofy(phisin);

	TReal kappacos = cosq(rz);
	zerofy(kappacos);

	TReal kappasin = sinq(rz);
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

TTransformation TTransformation::getIdentity()
{
	TTransformation t;
	t.setIdentityTransformation(); 
	return t;
}

void TTransformation::setZeroMatrix(){
	fTransM->setZero();
}

void TTransformation::setMatrixIJPosition(int row, int column, TReal value){
	auto& m(*(fTransM));
	m(row, column) = value;
}

TReal TTransformation::getMmatrixIJPosition(int row, int column) const{
	assert4D(row);
	assert4D(column);
	auto& m(*(fTransM));
	return m(row,column);
}

const Eigen::Matrix4d& TTransformation::getMatrix() const {
	const auto& m(*(fTransM));
	return m;
}


TTransformation& TTransformation::operator=(const TTransformation& rhs) {
	*(fTransM) = *(rhs.fTransM);
	return *this;
}

bool TTransformation::transform(TPositionVector& pv) const{
	TCoordSysFactory::ECoordSys k3DCart = TCoordSysFactory::ECoordSys::k3DCartesian;
	TANumericValue::EStatus kNull = TANumericValue::EStatus::kNull;
	bool result = false;

	//If status is not null and vector is in a 3d cart. coordinates.
	if(pv.getStatus()!=kNull && pv.getCoordSys() == k3DCart){
      Eigen::Vector4d pTemp(pv.getX().getMetresValue(), pv.getY().getMetresValue(), pv.getZ().getMetresValue(), 1.0);
	
		//Transform
		Eigen::Vector4d pResult = *(fTransM) * pTemp;

		//pResult is a result of the point transformation in homogeneus coordinates
		//If w (scale) coordinate is 0, than it is a point at infinity.
		zerofy(pResult[3]);
		if(pResult[3] == 0.0){
			throw std::runtime_error("Transformed TPositionVector is a point at infinity");
		}

		//Transform point in homogeneus coordinates back to cartesian coordinates.
		pv.setX(TLength(pResult[0]/pResult[3]));
      pv.setY(TLength(pResult[1]/pResult[3]));
      pv.setZ(TLength(pResult[2]/pResult[3]));
		result = true;
	}
	else{
		pv.setStatus(kNull);
	}
	return result;
}

bool TTransformation::transform(TFreeVector& fv) const{
	
	TCoordSysFactory::ECoordSys k3DCart = TCoordSysFactory::ECoordSys::k3DCartesian;
	TANumericValue::EStatus kNull = TANumericValue::EStatus::kNull;
	bool result = false;

	Eigen::Matrix4d mat(*this->fTransM);		

	//If status is not null and vector is in a 3d cart. coordinates.
	if(fv.getStatus()!=kNull && fv.getCoordSys()==k3DCart){
      Eigen::Vector4d pTemp(fv.getX().getMetresValue(), fv.getY().getMetresValue(), fv.getZ().getMetresValue(), 1.0);
	
		//TFreeVector is not affected by translation
		mat(0,3) = 0;
		mat(1,3) = 0;
		mat(2,3) = 0;

		//Transform
		Eigen::Vector4d pResult = mat * pTemp;

		//pResult is a result of the point transformation in homogeneus coordinates
		//If w (scale) coordinate is 0, than it is a point at infinity
		zerofy(pResult[3]);
		if(pResult[3] == 0.0){
			throw std::runtime_error("Transformed TFreeVector is a point at infinity");
		}

		//Transform point in homogeneus coordinates to cartesian coordinates
		fv.setX(TLength(pResult[0]/pResult[3]));
      fv.setY(TLength(pResult[1]/pResult[3]));
      fv.setZ(TLength(pResult[2]/pResult[3]));
		result = true;
	}
	else{
		fv.setStatus(kNull);
	}
	return result;
}


TTransformation TTransformation::operator*(const TTransformation& trans) const {
	TTransformation t(*this);
	*(t.fTransM) = *(fTransM) * *(trans.fTransM);
	return t;
}

TTransformation& TTransformation::operator*=(const TTransformation& trans) {
	*this = *this * trans;
	return  *this;
}

TPositionVector TTransformation::operator*(const TPositionVector& pos) const
{
	TPositionVector result(pos);
	transform(result);
	return result;
}

TFreeVector TTransformation::operator*(const TFreeVector& pos) const
{
	TFreeVector result(pos);
	transform(result);
	return result;
}