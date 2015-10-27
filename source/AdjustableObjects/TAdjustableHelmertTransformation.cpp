#include "TAdjustableHelmertTransformation.h"
#include "Global.h"

TAdjustableHelmertTransformation::TAdjustableHelmertTransformation() {
	
	fixedTranslations = bitset<3>();
	fixedRotations = bitset<3>();
	fixedScale = bitset<1>();
	name = "";

	setDefaults();
	setDefaultsParams();
}

TAdjustableHelmertTransformation::TAdjustableHelmertTransformation(const std::bitset<3>& fixedTranslations, const std::bitset<3>& fixedRotations, const std::bitset<1>& fixedScale, const string& name) :
	fixedTranslations(fixedTranslations),
	fixedRotations(fixedRotations),
	fixedScale(fixedScale),
	name(name)
{	
	setDefaults();
	setDefaultsParams();
}

TAdjustableHelmertTransformation::TAdjustableHelmertTransformation(const TransformParameters& transfParam, const std::bitset<3>& fixedTranslations, const std::bitset<3>& fixedRotations, const std::bitset<1>& fixedScale, const string& name) :
	fixedTranslations(fixedTranslations),
	fixedRotations(fixedRotations),
	fixedScale(fixedScale),
	name(name),
	fProvParameter(transfParam),
	fEstParameter(transfParam)
{	
	setDefaults();
}

int TAdjustableHelmertTransformation::getNumUnkn() const {
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

const TLength& TAdjustableHelmertTransformation::getTranslationStandDev(int d) const{
	assert3D(d);
	if(!fTransStandDev[d].isNull())
		return fTransStandDev[d];
	throw std::runtime_error("Standard deviations of the translation's component not assigned");
}

const TAngle& TAdjustableHelmertTransformation::getRotationStandDev(int d) const{
	assert3D(d);
	if(!fRotStandDev[d].isNull())
		return fRotStandDev[d];
	throw std::runtime_error("Standard deviations of the rotation's component not assigned");
}

TReal TAdjustableHelmertTransformation::getScaleStandDev()const{
	if(!isnan(fScaleStandDev))
      throw std::runtime_error("Standard deviations of the scale not assigned");
	return fScaleStandDev;
}

bool TAdjustableHelmertTransformation::hasRotationStandDev(int d) const{
	assert3D(d);
	return !fRotStandDev[d].isNull();
}

bool TAdjustableHelmertTransformation::hasTranslStandDev(int d)const{
	assert3D(d);
	return !fTransStandDev[d].isNull();
}

bool TAdjustableHelmertTransformation::hasScaleStandDev() const{
   return !isnan(fScaleStandDev);
}

bool TAdjustableHelmertTransformation::hasStandDev(){
	return (!fTransStandDev[0].isNull() || !fTransStandDev[1].isNull() || !fTransStandDev[2].isNull() || !fRotStandDev[0].isNull() || !fRotStandDev[1].isNull()
      || !fRotStandDev[2].isNull() || !isnan(fScaleStandDev));
}

void TAdjustableHelmertTransformation::setCorrection(int idx, TReal value) {
	for (int i = 0; i < 3; i++)
		if (uidx_trans[i] == idx) {
			setTranslationCorrection (i, TLength(value));
			return;
		}

	for (int i = 0; i < 3; i++)
		if (uidx_rot[i] == idx) {
			setRotationCorrection(i,TAngle(value, TAngle::kRadians));
			return;
		}

	if(uidx_scale == idx){
		setScaleCorrection(value);
		return;
	}

	throw std::logic_error("Invalid unknown index in parameter access.");
}

void TAdjustableHelmertTransformation::setParam(const TAngle& rx, const TAngle& ry, const TAngle& rz){
	fEstParameter.omega = fProvParameter.omega = rx;
	fEstParameter.phi = fProvParameter.phi = ry;
	fEstParameter.kappa = fProvParameter.kappa = rz;
}

void TAdjustableHelmertTransformation::setParam(const TLength tx, const TLength ty, const TLength tz){
	fEstParameter.tX = fProvParameter.tX = tx;
	fEstParameter.tY = fProvParameter.tY = ty;
	fEstParameter.tZ = fProvParameter.tZ = tz;
}

void TAdjustableHelmertTransformation::setParam(const TReal scl){
	fEstParameter.scale = fProvParameter.scale = scl;
}

void TAdjustableHelmertTransformation::setParam(const TLength tx, const TLength ty, const TLength tz, const TAngle& rx, const TAngle& ry, const TAngle& rz, const TReal scl){ 
	setParam(rx, ry, rz);
	setParam(tx, ty, tz);
	setParam(scl);
}

void TAdjustableHelmertTransformation::setTranslationCorrection (int idx, TLength value){
	if (idx == 0)
		fEstParameter.tX = fEstParameter.tX + value;
	else if (idx == 1)
		fEstParameter.tY = fEstParameter.tY + value;
	else if (idx == 2)
		fEstParameter.tZ = fEstParameter.tZ + value;
	else
		throw std::logic_error("Invalid unknown index in parameter access.");
	return;
}

void TAdjustableHelmertTransformation::setRotationCorrection (int idx, const TAngle& value){
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


const TAngle& TAdjustableHelmertTransformation::getEstimatedPrecisionRot(int d)const{
	if(fEstPrecisionRotation[d].getGonsValue()==NO_VALf)
		throw std::logic_error("No rotation precision assigned");
	return fEstPrecisionRotation[d];

}

const TLength& TAdjustableHelmertTransformation::getEstimatedPrecisionTransl(int d)const{
	if(fEstPrecisionTranslation[d].isNull())
		throw std::logic_error("No translation precision assigned");
	return fEstPrecisionTranslation[d];

}

TReal TAdjustableHelmertTransformation::getEstimatedPrecisionScale() const{
	if(isnan(fEstPrecisionScale))
		throw std::logic_error("No scale precision assigned");
	return fEstPrecisionScale;
}


const TReal&	TAdjustableHelmertTransformation::getXYCovarTransl() const{
	if(fCovarianceTranslation[0]==NO_VALf)
		throw std::logic_error("No XY covariance assigned.");
	return fCovarianceTranslation[0];
}

const TReal&	TAdjustableHelmertTransformation::getYZCovarTransl() const{
	if(fCovarianceTranslation[1]==NO_VALf)
		throw std::logic_error("No YZ covariance assigned.");
	return fCovarianceTranslation[1];
}


const TReal&	TAdjustableHelmertTransformation::getXZCovarTransl() const{
	if(fCovarianceTranslation[2]==NO_VALf)
		throw std::logic_error("No XZ covariance assigned.");
	return fCovarianceTranslation[2];
}

bool TAdjustableHelmertTransformation::isTranslationFixed(int d) const { 
				assert3D(d);
				return fixedTranslations[d]; 
			}

bool TAdjustableHelmertTransformation::isRotationFixed(int d) const { 
	assert3D(d);
	return fixedRotations[d]; 
}

const TReal& TAdjustableHelmertTransformation::getXYCovarRot() const{
		if(fCovarianceRotation[0]==NO_VALf)
		throw std::logic_error("No XY covariance assigned.");
	return fCovarianceRotation[0];
}


const TReal& TAdjustableHelmertTransformation::getYZCovarRot() const{
		if(fCovarianceRotation[1]==NO_VALf)
		throw std::logic_error("No YZ covariance assigned.");
	return fCovarianceRotation[1];
}

const TReal& TAdjustableHelmertTransformation::getXZCovarRot() const{
		if(fCovarianceRotation[2]==NO_VALf)
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

void TAdjustableHelmertTransformation::setTranslationStandDev(int d, TLength stDev){
	assert3D(d);
	fTransStandDev[d].setMetresValue(stDev);
}

void TAdjustableHelmertTransformation::setRotationStandDev(int d, TAngle stDev){
	assert3D(d);
	fRotStandDev[d] = stDev;
}

void TAdjustableHelmertTransformation::setScaleStandDev(TReal stDev){
	fScaleStandDev = stDev;
}


void TAdjustableHelmertTransformation::setDefaults(){

		fRotStandDev[0] = fRotStandDev[1] = fRotStandDev[2] = TAngle();
		fTransStandDev[0] = fTransStandDev[1] = fTransStandDev[2] = TLength(); 
		fScaleStandDev = NO_VALf;

		uidx_rot[0] = -1;
		uidx_rot[1] = -1;
		uidx_rot[2] = -1;

		uidx_trans[0] = -1;
		uidx_trans[1] = -1;
		uidx_trans[2] = -1;

		uidx_scale = -1;
}

void TAdjustableHelmertTransformation::setDefaultsParams(){
		fProvParameter.omega.setRadiansValue(0.0);
		fProvParameter.phi.setRadiansValue(0.0);
		fProvParameter.kappa.setRadiansValue(0.0);
		fProvParameter.tX = TLength(0.0);
		fProvParameter.tY = TLength(0.0);
		fProvParameter.tZ = TLength(0.0);
		fProvParameter.scale = 1;

		fEstParameter.omega.setRadiansValue(0.0);
		fEstParameter.phi.setRadiansValue(0.0);
		fEstParameter.kappa.setRadiansValue(0.0);
		fEstParameter.tX = TLength(0.0);
		fEstParameter.tY = TLength(0.0);
		fEstParameter.tZ = TLength(0.0);
		fEstParameter.scale = 1;
}


void	TAdjustableHelmertTransformation::setXYTranslationCovariance(TReal value){
	if (!fixedTranslations[0] && !fixedTranslations[1])
		fCovarianceTranslation[0] = value;
	else
		throw std::logic_error("TAdjustableHelmertTransformation::setXYTranslationCovariance, translation must be variable in both X and Y.");
}

void	TAdjustableHelmertTransformation::setYZTranslationCovariance(TReal value){
	if (!fixedTranslations[1] && !fixedTranslations[2])
		fCovarianceTranslation[1] = value;
	else
		throw std::logic_error("TAdjustablePlane::setYZTranslationCovariance, translation must be variable in both Y and Z.");
}

void	TAdjustableHelmertTransformation::setXZTranslationCovariance(TReal value){
	if (!fixedTranslations[0] && !fixedTranslations[2])
		fCovarianceTranslation[2] = value;
	else
		throw std::logic_error("TAdjustablePlane::setXZTranslationCovariance, translation must be variable in both X and Z.");
}


void	TAdjustableHelmertTransformation::setXYRotationCovariance(TReal value){
	if (!fixedRotations[0] && !fixedRotations[1])
		fCovarianceRotation[0] = value;
	else
		throw std::logic_error("TAdjustableHelmertTransformation::setXYRotationCovariance, rotation must be variable in both X and Y.");
}

void	TAdjustableHelmertTransformation::setYZRotationCovariance(TReal value){
	if (!fixedRotations[1] && !fixedRotations[2])
		fCovarianceRotation[1] = value;
	else
		throw std::logic_error("TAdjustablePlane::setYZRotationCovariance, rotation must be variable in both Y and Z.");
}

void	TAdjustableHelmertTransformation::setXZRotationCovariance(TReal value){
	if (!fixedRotations[0] && !fixedRotations[2])
		fCovarianceRotation[2] = value;
	else
		throw std::logic_error("TAdjustablePlane::setXZRotationCovariance, rotation must be variable in both X and Z.");
}

void	TAdjustableHelmertTransformation::setEstimatedPrecision(int idx, TReal value){
	if (uidx_scale == idx){
		fEstPrecisionScale = value;
		return;
	}

	for (int i = 0; i < 3; i++){
		if (uidx_rot[i] == idx) {
			if (i == 0 ){
				fEstPrecisionRotation[0].setRadiansValue(value);}
			else if(i == 1){
				fEstPrecisionRotation[1].setRadiansValue(value);}
			else{
				fEstPrecisionRotation[2].setRadiansValue(value);}
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


TLength TAdjustableHelmertTransformation::getEstTranslation(int axis) const{
	assert3D(axis);
	if(axis == 0)
		return fEstParameter.tX;
	else if(axis == 1)
		return fEstParameter.tY;
	else
		return fEstParameter.tZ;
}


TLength TAdjustableHelmertTransformation::getProvTranslation(int axis) const{
	assert3D(axis);
	if(axis == 0)
		return fProvParameter.tX;
	else if(axis == 1)
		return fProvParameter.tY;
	else
		return fProvParameter.tZ;
}

const TAngle& TAdjustableHelmertTransformation::getEstRotation(int axis) const{
	TAngle ang(NO_VALf, TAngle::kRadians);
	assert3D(axis);

		if(axis == 0)
			return fEstParameter.omega;
		else if(axis == 1)
			return fEstParameter.phi;
		else
			return fEstParameter.kappa;
}


const TAngle& TAdjustableHelmertTransformation::getProvRotation(int axis) const{
	TAngle ang(NO_VALf, TAngle::kRadians);
	assert3D(axis);

		if(axis == 0)
			return fProvParameter.omega;
		else if(axis == 1)
			return fProvParameter.phi;
		else
			return fProvParameter.kappa;
}

void TAdjustableHelmertTransformation::reInitialise(){
	setParam(fProvParameter.tX, fProvParameter.tY, fProvParameter.tZ);
	setParam(fProvParameter.omega, fProvParameter.phi, fProvParameter.kappa);
	setParam(fProvParameter.scale);

	fCovarianceTranslation[0] = NO_VALf;
	fCovarianceTranslation[1] = NO_VALf;
	fCovarianceTranslation[2] = NO_VALf;

	fCovarianceRotation[0] = NO_VALf;
	fCovarianceRotation[1] = NO_VALf;
	fCovarianceRotation[2] = NO_VALf;

	fEstPrecisionScale = NO_VALf;

	fEstPrecisionTranslation[0].setMetresValue(0.0);
	fEstPrecisionTranslation[1].setMetresValue(0.0);
	fEstPrecisionTranslation[2].setMetresValue(0.0);

	fEstPrecisionRotation[0].setRadiansValue(0.0);
	fEstPrecisionRotation[1].setRadiansValue(0.0);
	fEstPrecisionRotation[2].setRadiansValue(0.0);
}