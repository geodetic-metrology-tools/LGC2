#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include "LGCAdjustablePoint.h"
#include "TXYH2CCS.h"
#include <TTreeEntry.h>

#include <TPointTransformer.h>
#include <TLGCData.h>

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
bool LGCAdjustablePoint::allfixedParam = false;

LGCAdjustablePoint::LGCAdjustablePoint(const std::string& name):
TAdjustablePoint(name)
{}

LGCAdjustablePoint::LGCAdjustablePoint(const TPositionVector& pos, bool isXfixed, bool isYfixed, bool isZHfixed, const std::string& name, TRefSystemFactory::ERefFrame referential, TDataTreeIterator positionInTree) :
TAdjustablePoint(pos,isXfixed, isYfixed, isZHfixed, name, referential),
fFramePosition(positionInTree)
{}

LGCAdjustablePoint LGCAdjustablePoint::createUninitialized(const std::string& name)
{
	LGCAdjustablePoint ap(name);
	return ap;
}

///////////////////////////////////////////////////////////////////////////
// PUBLIC ACCESS METHODS
///////////////////////////////////////////////////////////////////////////

void LGCAdjustablePoint::setProvisionalValue(const TReal& x, const TReal& y, const TReal& z) {
    fProvisionalValue = TPositionVector(x, y, z, fProvisionalValue.getCoordSys());
    fEstimatedValue = fProvisionalValue;

    // Use H instead of Z if necessary:
    if(fProvisionalValue.getCoordSys() == TCoordSysFactory::k2DPlusH){
        fProvisionalValue.setH(TLength(z, TLength::kMetres));
        TAdjustablePoint::transformEstimatedValue();
    }
}

void LGCAdjustablePoint::setCorrection(int idx, TReal value) {
	for (int i = 0; i < 3; i++){
		if (uidx[i] == idx) {
			if (i == 0 ){
            fCorrection[i]=(TLength(value));
            fEstimatedValue.setX(fEstimatedValue.getX() + TLength(value));
				fXValueSet = true;
			}
			else if(i == 1){
            fCorrection[i]=(TLength(value));
            fEstimatedValue.setY(fEstimatedValue.getY() + TLength(value));
				fYValueSet = true;
			}
			else{
            fCorrection[2]=(TLength(value));
            fEstimatedValue.setZ(fEstimatedValue.getZ() + TLength(value));
			}

			//If H value is fixed and all variables were set in this step, we need to make transformation: X1Y1Z1 -> X1Y1H0 --> X1Y1Z0new
			if (fHfixed && fXValueSet && fYValueSet){
				transformEstimatedValue();
				if (!(fixedState[0] | allfixedParam))
					fXValueSet = false;
				if (!(fixedState[1] | allfixedParam))
					fYValueSet = false;
			}
			return;
		}
	}

	throw std::logic_error("Invalid unknown index in parameter access.");
}



/*! Sets the XY covariance after calculation */
void	LGCAdjustablePoint::setXYEstimatedCovariance(TReal value){
	if (allfixedParam)
		throw std::logic_error("ALLFIXED is used. No covariance to estimated");
	else
	{
		if (!(fixedState[0]) && !(fixedState[1]))
			fCovariance.setX(TLength(value));
		else
			throw std::logic_error("Point must be variable in both X and Y.");
	}
	
}

/*! Sets the YZ covariance after calculation  */
void	LGCAdjustablePoint::setYZEstimatedCovariance(TReal value){
	if (allfixedParam)
		throw std::logic_error("ALLFIXED is used. No covariance to estimated");
	else
	{
		if (!(fixedState[1]) && !(fixedState[2]))
			fCovariance.setY(TLength(value));
		else
			throw std::logic_error("Point must be variable in both Y and Z.");
	}
}

/*! Sets the XZ covariance after calculation 	
	\param[in] value Value to be set.
*/
void	LGCAdjustablePoint::setXZEstimatedCovariance(TReal value){

	if (allfixedParam)
		throw std::logic_error("ALLFIXED is used. No covariance to estimated");
	else
	{
		if (!(fixedState[0]) && !(fixedState[2]))
			fCovariance.setZ(TLength(value));
		else
			throw std::logic_error("Point must be variable in both X and Z.");
	}
}

/*! 
    See \ref TVAdjustableObject::setFirstUidx

	\throws Throws a logic_error if no component of the point is variable, i.e. a fixed point.
*/
void LGCAdjustablePoint::setFirstUidx(int idx) {
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to a fixed point.");
	else if (allfixedParam)
		throw std::logic_error("Trying to assign unknown index to a fixed point.(ALLFIXED is used)");
	for (int i = 0; i < 3; i++)
		if (!(fixedState[i]| allfixedParam))
			uidx[i] = idx++;
}

/// Update the adjustment information of an uninitialized point
void LGCAdjustablePoint::updateFixedState(bool lx, bool ly, bool lz) {
	fixedState[0] = (lx|allfixedParam);
	fixedState[1] = (ly|allfixedParam);
	fixedState[2] = (lz|allfixedParam);

	fXValueSet = (lx|allfixedParam);
	fYValueSet = (ly|allfixedParam);
}


int LGCAdjustablePoint::getNumUnkn() const
{
	if (!allfixedParam)
		return !(int)fixedState[0] + !(int)fixedState[1] + !(int)fixedState[2];
	else
		return 0;
}

bool LGCAdjustablePoint::hasVariable() const
{
	if (!allfixedParam)
		return !fixedState[0] || !fixedState[1] || !fixedState[2];
	else
		return false;
}

int LGCAdjustablePoint::getFirstUidx() const {
	if (allfixedParam)
		throw std::logic_error("Trying to get unknown index from fixed coordinate. (ALLFIXED is used)");
	else
	{	
	for (int i = 0; i < 3; i++)
		if (!fixedState[i])
			return uidx[i];
	throw std::logic_error("Trying to get unknown index from fixed coordinate.");
	}
}

int LGCAdjustablePoint::getLastUidx() const {
	if (allfixedParam)
		throw std::logic_error("Trying to get unknown index from fixed coordinate. (ALLFIXED is used)");
	else
	{
		for (int i = 2; i >= 0; i--)
			if (!fixedState[i])
				return uidx[i];
		throw std::logic_error("Trying to get unknown index from fixed coordinate.");
	}
}

int LGCAdjustablePoint::getCoordinateUnknIndex(int d) const {
	assert3D(d);
	if (allfixedParam)
		throw std::logic_error("Trying to get unknown index from fixed coordinate. (ALLFIXED is used)");
	else if (!fixedState[d])
		return uidx[d];
	else
		throw std::logic_error("Trying to get unknown index from fixed coordinate.");
}


#ifdef USE_SERIALIZER
// Inherited via Serializable
void LGCAdjustablePoint::serialize(SerializerObject::SerializationHelper &obj) const
{
	TAdjustablePoint::serialize(obj);
	obj.addProperty("allfixedParam", allfixedParam);
	obj.addProperty("fFramePosition_ID", fFramePosition.node->data.get()->ID);
	obj.addProperty("fFramePosition_Name", fFramePosition.node->data.get()->frame.getName());
}
#endif



TFreeVector LGCAdjustablePoint::transformSigmaInRoot(const LGCAdjustablePoint& pv, const TLGCData* fData)
{
	// vector to return the modified sigma in ROOT
	TFreeVector sigmaRoot(pv.getEstimatedValue().getCoordSys());
	TPointTransformer fPointTransfo(&fData->getTree(), fData->getConfig().referential);
	// vector to transform the point in the expected sigma to calcuated the partial derivative at each step
	TPositionVector ptInSF = pv.getEstimatedValue();

	//actual frame iterator
	TDataTreeIterator frameIt = pv.getFrameTreePosition();
	// actual frame
	auto frame = pv.getFrameTreePosition().node->data.get()->frame;

	
	//transformed covariance matrix
	TSparseMatrix covRoot(3, 3);
	//at the fist iteration , point covariance are:
	covRoot.insert(0, 0) = pow2q(pv.getXEstPrecision());
	covRoot.insert(1, 1) = pow2q(pv.getYEstPrecision());
	covRoot.insert(2, 2) = pow2q(pv.getZEstPrecision());
	covRoot.insert(0, 1) = pv.getXYCovar();
	covRoot.insert(1, 0) = pv.getXYCovar();
	covRoot.insert(0, 2) = pv.getXZCovar();
	covRoot.insert(2, 0) = pv.getXZCovar();
	covRoot.insert(1, 2) = pv.getYZCovar();
	covRoot.insert(2, 1) = pv.getYZCovar();


	

	while (frame.getName() != "ROOT")
	{
		// transform point in the actual subframe
		const TLOR2LOR& lorTrafo = fPointTransfo.getLORTransformation(pv.getFrameTreePosition(), frameIt); //Transformation from "TARGET POSITION" to "node n-1"
		lorTrafo.transform(ptInSF);


		//Matrix C : partial derive restect to tx ty tz rx ry rz x y z
		TSparseMatrix C(3, 10);
		//tx
		C.insert(0, 0) = 1.0;
		//ty
		C.insert(1, 1) = 1.0;
		//tz
		C.insert(2, 2) = 1.0;
		//rx
		C.insert(0, 3) = 0.0;
		C.insert(1, 3) = ((sin(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getX()
			+ (-cos(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getY()
			+ (cos(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getZ())*frame.getEstScale();
		C.insert(2, 3) = ((sin(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) - cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getX()
			+ (-cos(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) - sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getY()
			- (cos(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getZ())*frame.getEstScale();
		//ry
		C.insert(0, 4) = (-cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))* ptInSF.getX()
			- sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))* ptInSF.getY()
			- cos(frame.getEstRotation(1))* ptInSF.getZ())*frame.getEstScale();
		C.insert(1, 4) = (cos(frame.getEstRotation(2))*cos(frame.getEstRotation(1))*sin(frame.getEstRotation(0))* ptInSF.getX()
			+ sin(frame.getEstRotation(2))*cos(frame.getEstRotation(1))*sin(frame.getEstRotation(0))* ptInSF.getY()
			- sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0))* ptInSF.getZ())*frame.getEstScale();
		C.insert(2, 4) = (cos(frame.getEstRotation(2))*cos(frame.getEstRotation(1))*cos(frame.getEstRotation(0))* ptInSF.getX()
			+ sin(frame.getEstRotation(2))*cos(frame.getEstRotation(1))*cos(frame.getEstRotation(0))* ptInSF.getY()
			- sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0))* ptInSF.getZ())*frame.getEstScale();
		//rz
		C.insert(0, 5) = (-sin(frame.getEstRotation(2))*cos(frame.getEstRotation(1))* ptInSF.getX()
			+ cos(frame.getEstRotation(2))*cos(frame.getEstRotation(1))* ptInSF.getY())*frame.getEstScale();
		C.insert(1, 5) = ((-cos(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) - sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getX()
			+ (-sin(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getY())*frame.getEstScale();
		C.insert(2, 5) = ((cos(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) - sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getX()
			+ (sin(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getY())*frame.getEstScale();
		//scale
		C.insert(0, 6) = cos(frame.getEstRotation(2))*cos(frame.getEstRotation(1))* ptInSF.getX()
			+ sin(frame.getEstRotation(2))*cos(frame.getEstRotation(1))* ptInSF.getY()
			- sin(frame.getEstRotation(1))* ptInSF.getZ();
		C.insert(1, 6) = (-sin(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getX()
			+ (cos(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) + sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getY()
			+ cos(frame.getEstRotation(1))*sin(frame.getEstRotation(0))* ptInSF.getZ();
		C.insert(2, 6) = (sin(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getX()
			+ (-cos(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getY()
			+ cos(frame.getEstRotation(1))*cos(frame.getEstRotation(0))* ptInSF.getZ();
		//x
		C.insert(0, 7) = (cos(frame.getEstRotation(2))*cos(frame.getEstRotation(1)))*frame.getEstScale();
		C.insert(1, 7) = (-sin(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))*frame.getEstScale();
		C.insert(2, 7) = (sin(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))*frame.getEstScale();
		//y
		C.insert(0, 8) = (sin(frame.getEstRotation(2))*cos(frame.getEstRotation(1)))*frame.getEstScale();
		C.insert(1, 8) = (cos(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) + sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))*frame.getEstScale();
		C.insert(2, 8) = (-cos(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))*frame.getEstScale();
		//z
		C.insert(0, 9) = (-sin(frame.getEstRotation(1)))*frame.getEstScale();
		C.insert(1, 9) = (cos(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))*frame.getEstScale();
		C.insert(2, 9) = (cos(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))*frame.getEstScale();

		//covariance matrix = (cov frame, 0 ; 0, cov point)
		TSparseMatrix covSF(10, 10);

		//Fill covariance matrix
		//point covariance are:
		covSF.insert(7, 7) = covRoot.coeff(0, 0);
		covSF.insert(8, 8) = covRoot.coeff(1, 1);
		covSF.insert(9, 9) = covRoot.coeff(2, 2);
		covSF.insert(7, 8) = covRoot.coeff(0, 1);
		covSF.insert(8, 7) = covRoot.coeff(0, 1);
		covSF.insert(7, 9) = covRoot.coeff(0, 2);
		covSF.insert(9, 7) = covRoot.coeff(0, 2);
		covSF.insert(8, 9) = covRoot.coeff(1, 2);
		covSF.insert(9, 8) = covRoot.coeff(1, 2);

		//and fill the frame covariances covSF
		if (!frame.isTranslationFixed(0))
		{
			covSF.insert(0, 0) = pow2q(frame.getEstimatedPrecisionTransl(0));
			if (!frame.isTranslationFixed(1))
			{
				covSF.insert(0, 1) = frame.getXYCovarTransl();
				covSF.insert(1, 0) = frame.getXYCovarTransl();
			}
			if (!frame.isTranslationFixed(2))
			{
				covSF.insert(0, 2) = frame.getXZCovarTransl();
				covSF.insert(2, 0) = frame.getXZCovarTransl();
			}
			//
			if (!frame.isRotationFixed(0))
			{
				covSF.insert(0, 3) = frame.getTrRotCovar(0);
				covSF.insert(3, 0) = frame.getTrRotCovar(0);
			}
			if (!frame.isRotationFixed(1))
			{
				covSF.insert(0, 4) = frame.getTrRotCovar(1);
				covSF.insert(4, 0) = frame.getTrRotCovar(1);
			}
			if (!frame.isRotationFixed(2))
			{
				covSF.insert(0, 5) = frame.getTrRotCovar(2);
				covSF.insert(5, 0) = frame.getTrRotCovar(2);
			}
			if (!frame.isScaleFixed())
			{
				covSF.insert(0, 6) = frame.getScaleCovar(0);
				covSF.insert(6, 0) = frame.getScaleCovar(0);
			}
			//
		}
		if (!frame.isTranslationFixed(1))
		{
			covSF.insert(1, 1) = pow2q(frame.getEstimatedPrecisionTransl(1));
			if (!frame.isTranslationFixed(2))
			{
				covSF.insert(1, 2) = frame.getYZCovarTransl();
				covSF.insert(2, 1) = frame.getYZCovarTransl();
			}
			//
			if (!frame.isRotationFixed(0))
			{
				covSF.insert(1, 3) = frame.getTrRotCovar(3);
				covSF.insert(3, 1) = frame.getTrRotCovar(3);
			}
			if (!frame.isRotationFixed(1))
			{
				covSF.insert(1, 4) = frame.getTrRotCovar(4);
				covSF.insert(4, 1) = frame.getTrRotCovar(4);
			}
			if (!frame.isRotationFixed(2))
			{
				covSF.insert(1, 5) = frame.getTrRotCovar(5);
				covSF.insert(5, 1) = frame.getTrRotCovar(5);
			}
			if (!frame.isScaleFixed())
			{
				covSF.insert(1, 6) = frame.getScaleCovar(1);
				covSF.insert(6, 1) = frame.getScaleCovar(1);
			}
			//
		}
		if (!frame.isTranslationFixed(2))
		{
			covSF.insert(2, 2) = pow2q(frame.getEstimatedPrecisionTransl(2));
			//
			if (!frame.isRotationFixed(0))
			{
				covSF.insert(2, 3) = frame.getTrRotCovar(6);
				covSF.insert(3, 2) = frame.getTrRotCovar(6);
			}
			if (!frame.isRotationFixed(1))
			{
				covSF.insert(2, 4) = frame.getTrRotCovar(7);
				covSF.insert(4, 2) = frame.getTrRotCovar(7);
			}
			if (!frame.isRotationFixed(2))
			{
				covSF.insert(2, 5) = frame.getTrRotCovar(8);
				covSF.insert(5, 2) = frame.getTrRotCovar(8);
			}
			if (!frame.isScaleFixed())
			{
				covSF.insert(2, 6) = frame.getScaleCovar(2);
				covSF.insert(6, 2) = frame.getScaleCovar(2);
			}
			//
		}

		if (!frame.isRotationFixed(0))
		{
			covSF.insert(3, 3) = pow2q(frame.getEstimatedPrecisionRot(0).getRadiansValue());
			if (!frame.isRotationFixed(1))
			{
				covSF.insert(3, 4) = frame.getXYCovarRot();
				covSF.insert(4, 3) = frame.getXYCovarRot();
			}
			if (!frame.isRotationFixed(2))
			{
				covSF.insert(3, 5) = frame.getXZCovarRot();
				covSF.insert(5, 3) = frame.getXZCovarRot();
			}
		}
		if (!frame.isRotationFixed(1))
		{
			covSF.insert(4, 4) = pow2q(frame.getEstimatedPrecisionRot(1).getRadiansValue());
			if (!frame.isRotationFixed(2))
			{
				covSF.insert(4, 5) = frame.getYZCovarRot();
				covSF.insert(5, 4) = frame.getYZCovarRot();
			}
		}
		if (!frame.isRotationFixed(2))
		{
			covSF.insert(5, 5) = pow2q(frame.getEstimatedPrecisionRot(2).getRadiansValue());
		}
	

		if (!frame.isScaleFixed())
		{
			covSF.insert(6, 6) = pow2q(frame.getEstimatedPrecisionScale());
			//
			if (!frame.isRotationFixed(0))
			{
				covSF.insert(6, 3) = frame.getScaleCovar(3);
				covSF.insert(3, 6) = frame.getScaleCovar(3);
			}
			if (!frame.isRotationFixed(1))
			{
				covSF.insert(6, 4) = frame.getScaleCovar(4);
				covSF.insert(4, 6) = frame.getScaleCovar(4);
			}
			if (!frame.isRotationFixed(2))
			{
				covSF.insert(6, 5) = frame.getScaleCovar(5);
				covSF.insert(5, 6) = frame.getScaleCovar(5);
			}
			//

		}

		// calculated new point covariance in the upper frame
		covRoot = C*covSF*(C.transpose());

		//frame, go up
		frameIt = frameIt.node->parent;
		frame = frameIt.node->data.get()->frame;


		// TReal a = sqrt(covRoot.coeff(0, 0));
		// TReal b = sqrt(covRoot.coeff(1, 1));
		// TReal c = sqrt(covRoot.coeff(2, 2));
	}

	//extract variance in root
	sigmaRoot.setX(TLength(sqrt(covRoot.coeff(0, 0))));
	sigmaRoot.setY(TLength(sqrt(covRoot.coeff(1, 1))));
	sigmaRoot.setZ(TLength(sqrt(covRoot.coeff(2, 2))));

	return sigmaRoot;
}

bool LGCAdjustablePoint::isInRootFrame()
{
	return fFramePosition->get()->isROOTNode();
}
