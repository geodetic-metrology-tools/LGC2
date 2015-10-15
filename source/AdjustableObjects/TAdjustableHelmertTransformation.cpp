#include "TAdjustableHelmertTransformation.h"


//TAdjustableHelmertTransformation::TAdjustableHelmertTransformation(TTransformation* t, const std::bitset<3>& fixedTranslations, const std::bitset<3>& fixedRotations, const std::bitset<1>& fixedScale) :
//	transfo(t),
//	fixedTranslations(fixedTranslations),
//	fixedRotations(fixedRotations),
//	fixedScale(fixedScale),
//	fEstPrecisionScale()
//{
//	setDefaults();
//}

TAdjustableHelmertTransformation::TAdjustableHelmertTransformation(const string& name, const std::bitset<3>& fixedTranslations, const std::bitset<3>& fixedRotations, const std::bitset<1>& fixedScale) :
	fixedTranslations(fixedTranslations),
	fixedRotations(fixedRotations),
	fixedScale(fixedScale),
	fEstPrecisionScale(),
	name(name)
{	
	setDefaults();
}


int TAdjustableHelmertTransformation::getNumUkn() const {
	return (int)!fixedRotations[0] + (int)!fixedRotations[1] + (int)!fixedRotations[2] +
		   (int)!fixedTranslations[0] + (int)!fixedTranslations[1] + (int)!fixedTranslations[2] +
		   (int)!fixedScale[0];
}

int TAdjustableHelmertTransformation::getTranslationUnknIndex(int d) const{ 
	assert3D(d);
	if(!fixedTranslations[d])
		return uidx_trans[d];
	throw std::logic_error("Trying to get unknown index from fixed translation.");
}

int TAdjustableHelmertTransformation::getRotationUnknIndex(int d) const{ 
	assert3D(d);
	if(!fixedRotations[d])
		return uidx_rot[d];
	throw std::logic_error("Trying to get unknown index from fixed rotation.");
}

int TAdjustableHelmertTransformation::getScaleUnknIndex() const{
	if(!fixedScale[0])
		return uidx_scale;
	throw std::logic_error("Trying to get unknown index from fixed scale.");
}


int TAdjustableHelmertTransformation::getFirstUidx() const {	
	for (int i = 0; i < 3; i++)
		if (!fixedTranslations[i])
			return uidx_trans[i];	

	for (int i = 0; i < 3; i++)
		if (!fixedRotations[i])
			return uidx_rot[i];		

	if(!fixedScale[0])
		return uidx_scale;
	
	throw std::logic_error("Trying to get unknown index from fixed transformation.");
}

int TAdjustableHelmertTransformation::getLastUidx() const {
	if(!fixedScale[0])
		return uidx_scale;

	for (int i = 2; i >= 0; i--)
		if (!fixedRotations[i])
			return uidx_rot[i];
	for (int i = 2; i >= 0; i--)
		if (!fixedTranslations[i])
			return uidx_trans[i];

	throw std::logic_error("Trying to get unknown index from fixed transformation.");
}

TLength TAdjustableHelmertTransformation::getTranslationStandDev(int d) const{
	assert3D(d);
	if(!fTransStandDev[d].isNull())
		return fTransStandDev[d];
	throw std::runtime_error("Standard deviations of the translation's component not assigned");
}

LGC::TAngle TAdjustableHelmertTransformation::getRotationStandDev(int d) const{
	assert3D(d);
	if(!fRotStandDev[d].getSLAngle().isNull())
		return fRotStandDev[d];
	throw std::runtime_error("Standard deviations of the rotation's component not assigned");
}

TLength TAdjustableHelmertTransformation::getScaleStandDev()const{
	if(!fScaleStandDev.isNull())
		return fScaleStandDev;
	throw std::runtime_error("Standard deviations of the scale not assigned");
}

bool TAdjustableHelmertTransformation::hasRotationStandDev(int d) const{
	assert3D(d);
	return !fRotStandDev[d].getSLAngle().isNull();
}

bool TAdjustableHelmertTransformation::hasTranslStandDev(int d)const{
	assert3D(d);
	return !fTransStandDev[d].isNull();
}

bool TAdjustableHelmertTransformation::hasScaleStandDev() const{
	return !fScaleStandDev.isNull();
}

//If at least one of the parametres has standard deviation, then it is measured.
bool TAdjustableHelmertTransformation::hasStandDev(){
	return (!fTransStandDev[0].isNull() || !fTransStandDev[1].isNull() || !fTransStandDev[2].isNull() || !fRotStandDev[0].getSLAngle().isNull() || !fRotStandDev[1].getSLAngle().isNull()
		|| !fRotStandDev[2].getSLAngle().isNull() || !fScaleStandDev.isNull());
}

void TAdjustableHelmertTransformation::setCorrection(int idx, TReal value) {
	for (int i = 0; i < 3; i++)
		if (uidx_trans[i] == idx) {
			//trafo.setTranslation(i, trafo.getTranslation(i) + value);
			setTranslationCorrection (i, value);
			std::cout<<"T"<<i<<" : "<<getTranslation(i) <<std::endl;
			return;
		}

	for (int i = 0; i < 3; i++)
		if (uidx_rot[i] == idx) {
			//trafo.setAngle(i, LGC::TAngle(LGC::TAngle::kRadians, trafo.getAngle(i).rad() + value));
			setRotationCorrection(i,LGC::TAngle(LGC::TAngle::kRadians, value));
			std::cout<<"R"<<i<<" : "<< getAngle(i).gon() <<std::endl;
			return;
		}

	if(uidx_scale == idx)
		setScaleCorrection(value);
		//trafo.setScaleFactor(trafo.getScaleFactor() + value);

	throw std::logic_error("Invalid unknown index in parameter access.");
}

void TAdjustableHelmertTransformation::setParam(const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz){
	fEstParameter.omega = fProvParameter.omega = rx;
	fEstParameter.phi = fProvParameter.phi = ry;
	fEstParameter.kappa = fProvParameter.kappa = rz;
}

void TAdjustableHelmertTransformation::setParam(const TReal tx, const TReal ty, const TReal tz){
	fEstParameter.t1 = fProvParameter.t1 = tx;
	fEstParameter.t2 = fProvParameter.t2 = ty;
	fEstParameter.t3 = fProvParameter.t3 = tz;
}

void TAdjustableHelmertTransformation::setParam(const TReal scl){
	fEstParameter.scale = fProvParameter.scale = scl;
}

void TAdjustableHelmertTransformation::setParam(const TReal tx, const TReal ty, const TReal tz, const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz, const TReal scl){ 
	setParam(rx, ry, rz);
	setParam(tx, ty, tz);
	setParam(scl);
}

void TAdjustableHelmertTransformation::setTranslationCorrection (int idx, TReal value){
	if (idx == 0)
		fEstParameter.t1 = fEstParameter.t1 + value;
	else if (idx == 1)
		fEstParameter.t2 = fEstParameter.t2 + value;
	else if (idx == 2)
		fEstParameter.t3 = fEstParameter.t3 + value;
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
	return;
}

void TAdjustableHelmertTransformation::setRotationCorrection (int idx, LGC::TAngle value){
	if (idx == 0)
		fEstParameter.omega = fEstParameter.omega + value;
	else if (idx == 1)
		fEstParameter.phi = fEstParameter.phi + value;
	else if (idx == 2)
		fEstParameter.kappa = fEstParameter.kappa + value;
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
	return;

}

void TAdjustableHelmertTransformation::setScaleCorrection (TReal value){
	fEstParameter.scale = fEstParameter.scale + value;
}


LGC::TAngle TAdjustableHelmertTransformation::getEstimatedPrecisionRot(int d)const{
	if(fEstPrecisionRotation[d].getSLAngle().isNull())
		throw std::logic_error("No rotation precision assigned");
	return fEstPrecisionRotation[d];

}

TLength TAdjustableHelmertTransformation::getEstimatedPrecisionTransl(int d)const{
	if(fEstPrecisionTranslation[d].isNull())
		throw std::logic_error("No translation precision assigned");
	return fEstPrecisionTranslation[d];

}

TLength TAdjustableHelmertTransformation::getEstimatedPrecisionScale() const{
	if(fEstPrecisionScale.isNull())
		throw std::logic_error("No scale precision assigned");
	return fEstPrecisionScale;
}


/*!Return the estimated XY covariance*/
TLength			TAdjustableHelmertTransformation::getXYCovarTransl() const{
	if(fCovarianceTranslation[0].isNull())
		throw std::logic_error("No XY covariance assigned.");
	return fCovarianceTranslation[0];
}

/*!Return the estimated YZ covariance*/
TLength				TAdjustableHelmertTransformation::getYZCovarTransl() const{
	if(fCovarianceTranslation[1].isNull())
		throw std::logic_error("No YZ covariance assigned.");
	return fCovarianceTranslation[1];
}

/*!Return the estimated XZ covariance*/
TLength				TAdjustableHelmertTransformation::getXZCovarTransl() const{
	if(fCovarianceTranslation[2].isNull())
		throw std::logic_error("No XZ covariance assigned.");
	return fCovarianceTranslation[2];
}


/*!Return the estimated XY covariance*/
LGC::TAngle		TAdjustableHelmertTransformation::getXYCovarRot() const{
		if(fCovarianceRotation[0].getSLAngle().isNull())
		throw std::logic_error("No XY covariance assigned.");
	return fCovarianceRotation[0];
}

/*!Return the estimated YZ covariance*/
LGC::TAngle		TAdjustableHelmertTransformation::getYZCovarRot() const{
		if(fCovarianceRotation[1].getSLAngle().isNull())
		throw std::logic_error("No YZ covariance assigned.");
	return fCovarianceRotation[1];
}

/*!Return the estimated XZ covariance*/
LGC::TAngle		TAdjustableHelmertTransformation::getXZCovarRot() const{
		if(fCovarianceRotation[2].getSLAngle().isNull())
		throw std::logic_error("No XZ covariance assigned.");
	return fCovarianceRotation[2];
}


void TAdjustableHelmertTransformation::setFirstUidx(int idx) {
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to fixed transformation.");
	
	for (int i = 0; i < 3; i++)
		if (!fixedTranslations[i])
			uidx_trans[i] = idx++;

	for (int i = 0; i < 3; i++)
		if (!fixedRotations[i])
			uidx_rot[i] = idx++;

	if(!fixedScale[0])
		uidx_scale = idx++;
}

void TAdjustableHelmertTransformation::setTranslationStandDev(int d, TReal stDev){
	assert3D(d);
	fTransStandDev[d].setMetresValue(stDev);
}

void TAdjustableHelmertTransformation::setRotationStandDev(int d, LGC::TAngle stDev){
	assert3D(d);
	fRotStandDev[d] = stDev;
}

void TAdjustableHelmertTransformation::setScaleStandDev(TReal stDev){
	fScaleStandDev.setMetresValue(stDev);
}



void TAdjustableHelmertTransformation::setDefaults(){
		uidx_rot[0] = -1;
		uidx_rot[1] = -1;
		uidx_rot[2] = -1;

		uidx_trans[0] = -1;
		uidx_trans[1] = -1;
		uidx_trans[2] = -1;

		uidx_scale = -1;
}

void TAdjustableHelmertTransformation::setDefaultsParams(){
		fProvParameter.omega.set(LGC::TAngle::EUnits::kRadians,0);
		fProvParameter.phi.set(LGC::TAngle::EUnits::kRadians,0);
		fProvParameter.kappa.set(LGC::TAngle::EUnits::kRadians,0);
		fProvParameter.t1 = 0.0;
		fProvParameter.t2 = 0.0;
		fProvParameter.t3 = 0.0;
		fProvParameter.scale = 1;

		fEstParameter.omega.set(LGC::TAngle::EUnits::kRadians,0);
		fEstParameter.phi.set(LGC::TAngle::EUnits::kRadians,0);
		fEstParameter.kappa.set(LGC::TAngle::EUnits::kRadians,0);
		fEstParameter.t1 = 0.0;
		fEstParameter.t2 = 0.0;
		fEstParameter.t3 = 0.0;
		fEstParameter.scale = 1;
}


void	TAdjustableHelmertTransformation::setXYTranslationCovariance(TReal value){
	if (!fixedTranslations[0] && !fixedTranslations[1])
		fCovarianceTranslation[0].setMetresValue(value);
	else
		throw std::logic_error("TAdjustableHelmertTransformation::setXYTranslationCovariance, translation must be variable in both X and Y.");
}

void	TAdjustableHelmertTransformation::setYZTranslationCovariance(TReal value){
	if (!fixedTranslations[1] && !fixedTranslations[2])
		fCovarianceTranslation[1].setMetresValue(value);
	else
		throw std::logic_error("TAdjustablePlane::setYZTranslationCovariance, translation must be variable in both Y and Z.");
}

void	TAdjustableHelmertTransformation::setXZTranslationCovariance(TReal value){
	if (!fixedTranslations[0] && !fixedTranslations[2])
		fCovarianceTranslation[2].setMetresValue(value);
	else
		throw std::logic_error("TAdjustablePlane::setXZTranslationCovariance, translation must be variable in both X and Z.");
}


void	TAdjustableHelmertTransformation::setXYRotationCovariance(TReal value){
	if (!fixedRotations[0] && !fixedRotations[1])
		fCovarianceRotation[0].set(LGC::TAngle::kRadians, value);
	else
		throw std::logic_error("TAdjustableHelmertTransformation::setXYRotationCovariance, rotation must be variable in both X and Y.");
}

void	TAdjustableHelmertTransformation::setYZRotationCovariance(TReal value){
	if (!fixedRotations[1] && !fixedRotations[2])
		fCovarianceRotation[1].set(LGC::TAngle::kRadians, value);
	else
		throw std::logic_error("TAdjustablePlane::setYZRotationCovariance, rotation must be variable in both Y and Z.");
}

void	TAdjustableHelmertTransformation::setXZRotationCovariance(TReal value){
	if (!fixedRotations[0] && !fixedRotations[2])
		fCovarianceRotation[2].set(LGC::TAngle::kRadians, value);
	else
		throw std::logic_error("TAdjustablePlane::setXZRotationCovariance, rotation must be variable in both X and Z.");
}

void	TAdjustableHelmertTransformation::setEstimatedPrecision(int idx, TReal value){
	if (uidx_scale == idx){
		fEstPrecisionScale.setMetresValue(value);
		return;

	}

	for (int i = 0; i < 3; i++){
		if (uidx_rot[i] == idx) {
			if (i == 0 ){
				fEstPrecisionRotation[0].set(LGC::TAngle::kRadians, value);}
			else if(i == 1){
				fEstPrecisionRotation[1].set(LGC::TAngle::kRadians, value);}
			else{
				fEstPrecisionRotation[2].set(LGC::TAngle::kRadians, value);}
			return;
		}
		if (uidx_trans[i] == idx) {
			if (i == 0 ){
				fEstPrecisionTranslation[0].setMetresValue(value);}
			else if(i == 1){
				fEstPrecisionTranslation[1].setMetresValue(value);}
			else{
				fEstPrecisionTranslation[2].setMetresValue(value);}
			return;
		}
	}

	throw std::logic_error("Invalid unknown index in parameter access.");
}

//TTransformation* TAdjustableHelmertTransformation::getTransformation()
//{
//	return transfo;
//}
//
//TTransformation* TAdjustableHelmertTransformation::getTransformation() const
//{
//	return transfo;
//}

TReal TAdjustableHelmertTransformation::getTranslation(int axis) const{
	assert3D(axis);
	if(axis == 0)
		return fEstParameter.t1;
	else if(axis == 1)
		return fEstParameter.t2;
	else
		return fEstParameter.t3;
}

LGC::TAngle TAdjustableHelmertTransformation::getAngle(int axis) const{
	LGC::TAngle ang(LGC::TAngle::kRadians, NO_VALf);
	assert3D(axis);

		if(axis == 0)
			return fEstParameter.omega;
		else if(axis == 1)
			return fEstParameter.phi;
		else
			return fEstParameter.kappa;
}