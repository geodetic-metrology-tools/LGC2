#include "TAdjustableLine.h"
#include "Global.h"


//Reference point is the point on the line at the same time
TAdjustableLine::TAdjustableLine(const TPositionVector& pointOnALine, const TFreeVector& lineVect, const std::bitset<3>& pointFixedState, const std::bitset<3>& lineVectorFixedState, const std::string& name):
fName(name),
fPointProvisionalValue(pointOnALine),
fReferencePointPosition(pointOnALine),
fPointCorrection(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fPointEstimatedValue(pointOnALine),
fPointEstimatedPrecision(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fPointCovariance(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),

fLineVectorProvisionalValue(lineVect),
fLineVectorCorrection(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fLineVectorEstimatedValue(lineVect),
fLineVectorEstimatedPrecision(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fLineVectorCovariance(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),

fixedStatePoint(pointFixedState),
fixedStateLineVector(lineVectorFixedState)
{
	setDefaults();

}

//Reference point is the point on the line at the same time
TAdjustableLine::TAdjustableLine(const TPositionVector& pointOnALine,const TPositionVector& referencePoint, const TFreeVector& lineVect, const std::bitset<3>& pointFixedState, const std::bitset<3>& lineVectorFixedState, const std::string& name):
fName(name),
fPointProvisionalValue(pointOnALine),
fReferencePointPosition(referencePoint),
fPointCorrection(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fPointEstimatedValue(pointOnALine),
fPointEstimatedPrecision(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fPointCovariance(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),

fLineVectorProvisionalValue(lineVect),
fLineVectorCorrection(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fLineVectorEstimatedValue(lineVect),
fLineVectorEstimatedPrecision(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fLineVectorCovariance(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),

fixedStatePoint(pointFixedState),
fixedStateLineVector(lineVectorFixedState)
{
	setDefaults();
}

TAdjustableLine TAdjustableLine::createUninitialized(const std::string& name) {
	TAdjustableLine ap(TPositionVector(NO_VALf,  NO_VALf,  NO_VALf,TCoordSysFactory::ECoordSys::k3DCartesian), TFreeVector(NO_VALf,  NO_VALf,  NO_VALf,TCoordSysFactory::ECoordSys::k3DCartesian),std::bitset<3>(),std::bitset<3>(), name);
	return ap;
}

int TAdjustableLine::getNumUnkn() const {
	return (int)!fixedStatePoint[0] + (int)!fixedStatePoint[1] + (int)!fixedStatePoint[2] +
		  (int)!fixedStateLineVector[0] + (int)!fixedStateLineVector[1] + (int)!fixedStateLineVector[2];
}
		
int TAdjustableLine::getFirstUidx() const {	
	for (int i = 0; i < 3; i++)
		if (!fixedStatePoint[i])
			return uidx_point[i];		
	for (int i = 0; i < 3; i++)
		if (!fixedStateLineVector[i])
			return uidx_lineVector[i];		
	
	throw std::logic_error("Trying to get unknown index from fixed line.");
}

int TAdjustableLine::getLastUidx() const {
	for (int i = 2; i >= 0; i--)
		if (!fixedStateLineVector[i])
			return uidx_lineVector[i];
	for (int i = 2; i >= 0; i--)
		if (!fixedStatePoint[i])
			return uidx_point[i];

	throw std::logic_error("Trying to get unknown index from fixed line.");
}

void TAdjustableLine::setFirstUidx(int idx) {
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to fixed line.");
	
	for (int i = 0; i < 3; i++)
		if (!fixedStatePoint[i])
			uidx_point[i] = idx++;

	for (int i = 0; i < 3; i++)
		if (!fixedStateLineVector[i])
			uidx_lineVector[i] = idx++;
}

void TAdjustableLine::setCorrection(int idx, TReal value) {

   TLength val(value);

	for (int i = 0; i < 3; i++){
		if (uidx_point[i] == idx) {
			if (i == 0 ){
            fPointCorrection.setX(val);
            fPointEstimatedValue.setX(fPointEstimatedValue.getX() + val);
			}
			else if(i == 1){
            fPointCorrection.setY(val);
            fPointEstimatedValue.setY(fPointEstimatedValue.getY() + val);
			}
			else{
            fPointCorrection.setZ(val);
            fPointEstimatedValue.setZ(fPointEstimatedValue.getZ() + val);
			}
			return;
			}
	}
	for (int i = 0; i < 3; i++){
		if (uidx_lineVector[i] == idx) {
			if (i == 0 ){
            fLineVectorCorrection.setX(val);
            fLineVectorEstimatedValue.setX(fLineVectorEstimatedValue.getX() + val);
			}
			else if(i == 1){
            fLineVectorCorrection.setY(val);
            fLineVectorEstimatedValue.setY(fLineVectorEstimatedValue.getY() + val);
			}
			else{
            fLineVectorCorrection.setZ(val);
            fLineVectorEstimatedValue.setZ(fLineVectorEstimatedValue.getZ() + val);
			}
			return;
		}
	}
	throw std::logic_error("Invalid unknown index in parameter access.");
}

/*! Sets the estimated precision after calculation to a line's point.*/
void	TAdjustableLine::setPointEstimatedPrecision(int idx, TLength value){
	for (int i = 0; i < 3; i++)
		if (uidx_point[i] == idx) {
			if (i == 0 ){
				fPointEstimatedPrecision.setX(value);
			}
			else if(i == 1){
				fPointEstimatedPrecision.setY(value);
			}
			else{
				fPointEstimatedPrecision.setZ(value);
			}
			return;
		}
	throw std::logic_error("Invalid unknown index in parameter access.");
}

/*! Sets the estimated precision after calculation to a line's point.*/
void	TAdjustableLine::setLineVectorEstimatedPrecision(int idx, TReal value){
   TLength val(value);
   
   for (int i = 0; i < 3; i++)
		if (uidx_lineVector[i] == idx) {
			if (i == 0 ){
            fLineVectorEstimatedPrecision.setX(val);
			}
			else if(i == 1){
            fLineVectorEstimatedPrecision.setY(val);
			}
			else{
            fLineVectorEstimatedPrecision.setZ(val);
			}
			return;
		}
	throw std::logic_error("Invalid unknown index in parameter access.");
}


void TAdjustableLine::setDefaults(){
	uidx_point[0] = -1;
	uidx_point[1] = -1;
	uidx_point[2] = -1;

	uidx_lineVector[0] = -1;
	uidx_lineVector[1] = -1;
	uidx_lineVector[2] = -1;
}