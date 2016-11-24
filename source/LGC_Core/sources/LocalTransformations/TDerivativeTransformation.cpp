#include "TDerivativeTransformation.h"
#include "TPositionVector.h"
#include "Global.h"

TDerivativeTransformation::TDerivativeTransformation() 
	:fTransM(std::unique_ptr<Eigen::Matrix4d>(new Eigen::Matrix4d))
{
	fTransM->setZero();
}

TDerivativeTransformation::TDerivativeTransformation(const Eigen::Matrix4d& mat)
	:fTransM(std::unique_ptr<Eigen::Matrix4d>(new Eigen::Matrix4d(mat)))
{ }

TDerivativeTransformation::TDerivativeTransformation(const TDerivativeTransformation &derivTransform) : 
	fTransM(std::unique_ptr<Eigen::Matrix4d>(new Eigen::Matrix4d(*derivTransform.fTransM)))
{}

TDerivativeTransformation& TDerivativeTransformation::operator=(const TDerivativeTransformation& rhs) {
	*(fTransM) = *(rhs.fTransM);
	return *this;
}

TFreeVector TDerivativeTransformation::transform(const TPositionVector & pv) const
{
	//If status is not null and vector is in a 3d cart. coordinates.
	if(pv.isInitialise() && pv.getCoordSys() == TCoordSysFactory::ECoordSys::k3DCartesian)
	{
      Eigen::Vector4d pTemp(pv.getX().getMetresValue(), pv.getY().getMetresValue(), pv.getZ().getMetresValue(), 1.0);
	
		//Transform
		Eigen::Vector4d pResult = *fTransM * pTemp;

		//pResult is a result of the point transformation in homogeneus coordinates
		//If w (scale) coordinate is 0, than it is a point at infinity.
		zerofy(pResult[3]);
		if(pResult[3] == 0.0)
			throw std::runtime_error("Transformed TPositionVector is a point at infinity");

		//Transform point in homogeneus coordinates back to cartesian coordinates.
		return TFreeVector(pResult[0]/pResult[3], pResult[1]/pResult[3], pResult[2]/pResult[3], TCoordSysFactory::ECoordSys::k3DCartesian);
	}
	else
	{
		TFreeVector vector_null(TCoordSysFactory::ECoordSys::k3DCartesian);
		return vector_null;
	}
}


void TDerivativeTransformation::setMatrix(const Eigen::Matrix4d& mat)
{
	this->fTransM.reset(new Eigen::Matrix4d(mat));
}

const Eigen::Matrix4d& TDerivativeTransformation::getMatrix() const
{
	return *fTransM;
}

TReal TDerivativeTransformation::getMmatrixIJPosition(int row, int column) const{
	assert4D(row);
	assert4D(column);
	Eigen::Matrix4d& as = *fTransM;
	return as(row,column);
}

TFreeVector TDerivativeTransformation::operator*(const TPositionVector& pos) const
{
	return transform(pos);
}
