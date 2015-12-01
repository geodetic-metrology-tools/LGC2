#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include "TAdjustablePoint.h"
#include "TXYH2CCS.h"

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////

//Private constructor for creating uninitialized object
TAdjustablePoint::TAdjustablePoint(const std::string& name):
fName(name),
fProvisionalValue(NO_VALf,  NO_VALf,  NO_VALf,TCoordSysFactory::k3DCartesian),
fEstimatedValue(fProvisionalValue),
fCovariance(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fHfixed(false),
fReferential(TRefSystemFactory::ERefFrame::kNotInGraph),
fSpatialStatus(TSpatialStatus::kUnknown)
{
	setDefaults(true, true, true);
}

TAdjustablePoint::TAdjustablePoint(const TPositionVector& pos, bool isXfixed, bool isYfixed, bool isZHfixed, const std::string& name, TRefSystemFactory::ERefFrame referential, TDataTreeIterator positionInTree):
fName(name),
fProvisionalValue(pos),
fEstimatedValue(fProvisionalValue),
fCovariance(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fFramePosition(positionInTree),
fReferential(referential),
fHfixed(false),
fSpatialStatus(TSpatialStatus::kUnknown)
{
	if(pos.getCoordSys() == TCoordSysFactory::k2DPlusH ){ // If position is given in 2D + H system
		if(fReferential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
			TXYH2CCS::XYHs2CCS(fEstimatedValue);
		else if(fReferential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
			TXYH2CCS::XYHg2000Machine2CCS(fEstimatedValue);
		else if (fReferential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
			TXYH2CCS::XYHg1985Machine2CCS(fEstimatedValue);

		//If referential is a 2D+H system and H is fixed, than set fHfixed to TRUE, because we need to resetting original H when setting contributions
		if(isZHfixed == true)
			fHfixed = true;
	}

	setDefaults(isXfixed, isYfixed, isZHfixed);
}

TAdjustablePoint TAdjustablePoint::createUninitialized(const std::string& name) {
	TAdjustablePoint ap(name);
	return ap;
}

///////////////////////////////////////////////////////////////////////////
// PUBLIC ACCESS METHODS
///////////////////////////////////////////////////////////////////////////
TReal TAdjustablePoint::getStandDev(int d)const{
	assert3D(d);
	if(!isnan( fStandardDeviations[d]))
		return fStandardDeviations[d];
	throw std::runtime_error("Standard deviation of the point not assigned");
}

TLength	TAdjustablePoint::getErrorEllMajorAxis() const
{
	TReal vxy = getXYCovar()*M2MM;
	TReal sx2 = powq(getXEstPrecision().getMMetresValue(), 2);
	TReal sy2 = powq(getYEstPrecision().getMMetresValue(), 2);
	TReal gdAxe = (LITERAL(1.0)/sqrtq(LITERAL(2.0))) * sqrtq( sx2 + sy2 + sqrtq( powq((sy2 - sx2), 2) + (LITERAL(4.0) * vxy * vxy) ) );
	
	TLength res;
	res.setMMetresValue(gdAxe);
	return res;
}

TLength		TAdjustablePoint::getErrorEllMinorAxis() const
{
	TReal vxy = getXYCovar()*M2MM;
	TReal sx2 = powq(getXEstPrecision().getMMetresValue(), 2);
	TReal sy2 = powq(getYEstPrecision().getMMetresValue(), 2);
	TReal ptAxe = (LITERAL(1.0)/sqrtq(LITERAL(2.0))) * sqrtq( sx2 + sy2 - sqrtq( powq((sy2 - sx2), 2) + (LITERAL(4.0) * vxy *vxy) ) );
	
	TLength res;
	res.setMMetresValue(ptAxe);
	return res;
}

TAngle TAdjustablePoint::getErrorEllGis() const
{
	TReal vxy = getXYCovar()*M2MM;
	TReal sx2 = powq(getXEstPrecision().getMMetresValue(), 2);
	TReal sy2 = powq(getYEstPrecision().getMMetresValue(), 2);
	TAngle gis (( LITERAL(0.5)) * TAngle::aTan2(2*vxy , (sy2 - sx2) ) );
	
	TReal testAffichage = gis.getGonsValue();
	TAngle deuxCentGrad;
	deuxCentGrad.setGonsValue(200);
	if(testAffichage > 200)
	{
		gis = gis - deuxCentGrad;
	}

	return gis;
}


TAdjustablePoint::ErrorEllipsoid TAdjustablePoint::getErrorEllipsoid() const {
	Eigen::Matrix3d m;

	m << pow2(getXEstPrecision()), getXYCovar(), getXZCovar(), getXYCovar(), 
        pow2(getYEstPrecision()), getYZCovar(), getXZCovar(), getYZCovar(), 
        pow2(getZEstPrecision());

	Eigen::EigenSolver<Eigen::Matrix3d> ev(m);
	
	const auto& evals(ev.eigenvalues());
	const auto& evecs(ev.eigenvectors());
	
	/* The length of the semimajor axes of the 95% confidence ellipsoid are
	   Fv * [eigenvalue_0, eigenvalue_1, eigenvalue_2] with Fv being the 95% 
	   percentile of the Fisher distribution for three degrees of freedom (approx. 7.80).
	   We are interested in the one-sigma-level, so our multiplier actualy is 1.0.

	   We need the eigenvectors as well since they are the direction sof the ellipsoid's axes.
	   
	   Source:
	   Parameter Estimation and Inverse Problems
	   Richard C. Aster, Brian Borchers, Clifford H. Thurber
	   Academic Press, 2013 - 360 pages
	   (on page 36)
	*/

	ErrorEllipsoid ell = {
		// Indexing is (row, column): Each column corresponds to one eigenvector
		{evecs(0,0).real(), evecs(1,0).real(), evecs(2,0).real()},
		{evecs(0,1).real(), evecs(1,1).real(), evecs(2,1).real()},
		{evecs(0,2).real(), evecs(1,2).real(), evecs(2,2).real()},
		sqrtq(evals(0).real()),
		sqrtq(evals(1).real()),
		sqrtq(evals(2).real()),
	};

	return ell;
}


void TAdjustablePoint::setCorrection(int idx, TReal value) {
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
				if(!fixedState[0])
					fXValueSet = false;
				if(!fixedState[1])
					fYValueSet = false;
			}
			return;
		}
	}

	throw std::logic_error("Invalid unknown index in parameter access.");
}

	/*! Sets the estimated precision after calculation */
void	TAdjustablePoint::setEstimatedPrecision(int idx, TReal value){
	for (int i = 0; i < 3; i++){
		if (uidx[i] == idx) {
			if (i == 0 ){
            fEstimatedPrecision[i]=TLength(value);
			}
			else if(i == 1){
            fEstimatedPrecision[i]=TLength(value);
			}
			else{
            fEstimatedPrecision[2]=TLength(value);
			}
			return;
		}
	}
	throw std::logic_error("Invalid unknown index in parameter access.");
}

/*! Sets the XY covariance after calculation */
void	TAdjustablePoint::setXYEstimatedCovariance(TReal value){
	if (!fixedState[0] && !fixedState[1])
      fCovariance.setX(TLength(value));
	else
		throw std::logic_error("Point must be variable in both X and Y.");
	
}

/*! Sets the YZ covariance after calculation  */
void	TAdjustablePoint::setYZEstimatedCovariance(TReal value){
	if (!fixedState[1] && !fixedState[2])
		fCovariance.setY(TLength(value));
	else
		throw std::logic_error("Point must be variable in both Y and Z.");
}

/*! Sets the XZ covariance after calculation 	
	\param[in] value Value to be set.
*/
void	TAdjustablePoint::setXZEstimatedCovariance(TReal value){
	if (!fixedState[0] && !fixedState[2])
      fCovariance.setZ(TLength(value));
	else
		throw std::logic_error("Point must be variable in both X and Z.");
}

/*! 
    See \ref TVAdjustableObject::setFirstUidx

	\throws Throws a logic_error if no component of the point is variable, i.e. a fixed point.
*/
void TAdjustablePoint::setFirstUidx(int idx) {
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to a fixed point.");
	for (int i = 0; i < 3; i++)
		if (!fixedState[i])
			uidx[i] = idx++;
}

/// Update the adjustment information of an uninitialized point
void TAdjustablePoint::updateFixedState(bool lx, bool ly, bool lz) {
	fixedState[0] = lx;
	fixedState[1] = ly;
	fixedState[2] = lz;

	fXValueSet = lx;
	fYValueSet = ly;
}


void TAdjustablePoint::setStandardDeviations(TReal stDevX, TReal stDevY, TReal stDevZ){
	fStandardDeviations[0] = stDevX;
	fStandardDeviations[1] = stDevY;
	fStandardDeviations[2] = stDevZ;
}

void TAdjustablePoint::reInitialise(){
	TFreeVector zeroVec(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian);
	fCorrection[0] = fCorrection[1] = fCorrection[2] =TLength(0.0);
	fEstimatedValue = fProvisionalValue;

	/*If the provisional value was in XYH, transform the estimated value into XYZ*/
	if(fProvisionalValue.getCoordSys() == TCoordSysFactory::k2DPlusH ){ // If position is given in 2D + H system
		if(fReferential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
			TXYH2CCS::XYHs2CCS(fEstimatedValue);
		else if(fReferential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
			TXYH2CCS::XYHg2000Machine2CCS(fEstimatedValue);
		else if (fReferential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
			TXYH2CCS::XYHg1985Machine2CCS(fEstimatedValue);
	}

	fEstimatedPrecision[0] = fEstimatedPrecision[1] = fEstimatedPrecision[2] = TLength(0.0);

	fCovariance = zeroVec;
}
///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
///////////////////////////////////////////////////////////////////////////

void TAdjustablePoint::setDefaults(bool lx, bool ly, bool lz) {				
	fixedState[0] = lx;
	fixedState[1] = ly;
	fixedState[2] = lz;

	uidx[0] = -1;
	uidx[1] = -1;
	uidx[2] = -1;

	fXValueSet = lx;
	fYValueSet = ly;

	fStandardDeviations[0] = NO_VALf;
	fStandardDeviations[1] = NO_VALf;
	fStandardDeviations[2] = NO_VALf;

	fCorrection[0] = TLength(0.0);
	fCorrection[1] = TLength(0.0);
	fCorrection[2] = TLength(0.0);

	fEstimatedPrecision[0] = TLength(0.0);
	fEstimatedPrecision[1] = TLength(0.0);
	fEstimatedPrecision[2] = TLength(0.0);

	if (lx && ly && lz)
		fSpatialStatus = TSpatialStatus::kCala;
	else if (lx && ly && !lz)
		fSpatialStatus = TSpatialStatus::kVz;
	else if (lx && !ly && lz)
		fSpatialStatus = TSpatialStatus::kVy;
	else if (!lx && ly && lz)
		fSpatialStatus = TSpatialStatus::kVx;
	else if (!lx && !ly && lz)
		fSpatialStatus = TSpatialStatus::kVxy;
	else if (lx && !ly && !lz)
		fSpatialStatus = TSpatialStatus::kVyz;
	else if (!lx && ly && !lz)
		fSpatialStatus = TSpatialStatus::kVxz;
	else if (!lx && !ly && !lz)
		fSpatialStatus = TSpatialStatus::kVxyz;

}

void TAdjustablePoint::transformEstimatedValue(){
	//Set original H value and calculate Z0star for next iteration.
	fEstimatedValue.setCoordSys(TCoordSysFactory::ECoordSys::k2DPlusH);
	fEstimatedValue.setH(fProvisionalValue.getH());
	if(fReferential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
		TXYH2CCS::XYHs2CCS(fEstimatedValue);
	else if(fReferential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
		TXYH2CCS::XYHg2000Machine2CCS(fEstimatedValue);
	else if (fReferential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
		TXYH2CCS::XYHg1985Machine2CCS(fEstimatedValue);
}

TReal TAdjustablePoint::getHEstValue() const{
	if(fReferential == TRefSystemFactory::ERefFrame::kLocalRefFrame)
		throw std::runtime_error("Point is defined in local system, no geoid assigned.");

	TPositionVector pvEst = fEstimatedValue;
	if(fReferential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
		TXYH2CCS::CCS2XYHs(pvEst);
	else if(fReferential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
		TXYH2CCS::CCS2XYHg2000Machine(pvEst);
	else if (fReferential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
		TXYH2CCS::CCS2XYHg1985Machine(pvEst);

	return pvEst.getH().getMetresValue();
}
