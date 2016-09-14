#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include "LGCAdjustablePoint.h"
#include "TXYH2CCS.h"

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
bool&LGCAdjustablePoint::allfixedParam = False;

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
