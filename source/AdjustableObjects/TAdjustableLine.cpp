#include "TAdjustableLine.h"
#include "Global.h"


//Reference point is the point on the line at the same time
TAdjustableLine::TAdjustableLine(const TAdjustablePoint* pointOnALine, const TFreeVector& lineVect, const std::bitset<3>& lineVectorFixedState, const std::string& name) :
fName(name),
fReferencePointPosition(TCoordSysFactory::k3DCartesian),
fLinePoint(pointOnALine),
fLineVectorProvisionalValue(lineVect),
fLineVectorCorrection(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fLineVectorEstimatedValue(lineVect),
fLineVectorEstimatedPrecision(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fLineVectorCovariance(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fixedStateLineVector(lineVectorFixedState)
{
	setDefaults();

	if (pointOnALine)
		fReferencePointPosition = pointOnALine->getProvisionalValue();

}

//Reference point is the point on the line at the same time
TAdjustableLine::TAdjustableLine(const TAdjustablePoint* pointOnALine, const TPositionVector& referencePoint, const TFreeVector& lineVect, const std::bitset<3>& lineVectorFixedState, const std::string& name) :
fName(name),
fLinePoint(pointOnALine),
fReferencePointPosition(referencePoint),
fLineVectorProvisionalValue(lineVect),
fLineVectorCorrection(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fLineVectorEstimatedValue(lineVect),
fLineVectorEstimatedPrecision(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fLineVectorCovariance(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian),
fixedStateLineVector(lineVectorFixedState)
{
	setDefaults();
}

void TAdjustableLine::initialize(const TAdjustablePoint* pointOnALine, const TFreeVector& lineVect, const std::bitset<3>& lineVectorFixedState)
{
	fLinePoint = pointOnALine;
	fReferencePointPosition = pointOnALine->getProvisionalValue();
	fLineVectorProvisionalValue=lineVect;
	fLineVectorEstimatedValue=lineVect;

	fixedStateLineVector=lineVectorFixedState;
	fInit = true;
}

void TAdjustableLine::initialize(const TAdjustablePoint* pointOnALine, const TPositionVector& referencePoint, const TFreeVector& lineVect, const std::bitset<3>& lineVectorFixedState)
{
	fLinePoint = pointOnALine;
	fReferencePointPosition = referencePoint;
	fLineVectorProvisionalValue = lineVect;
	fLineVectorEstimatedValue = lineVect;

	fixedStateLineVector = lineVectorFixedState;
	fInit = true;
}


TAdjustableLine TAdjustableLine::createUninitialized(const std::string& name) {
	TAdjustableLine ap(nullptr, TFreeVector(NO_VALf,  NO_VALf,  NO_VALf,TCoordSysFactory::ECoordSys::k3DCartesian),std::bitset<3>(), name);
	ap.fInit = false;
	return ap;
}

int TAdjustableLine::getNumUnkn() const {
	return (int)!fixedStateLineVector[0] + (int)!fixedStateLineVector[1] + (int)!fixedStateLineVector[2];
}
		
int TAdjustableLine::getFirstUidx() const {			
	for (int i = 0; i < 3; i++)
		if (!fixedStateLineVector[i])
			return uidx_lineVector[i];		
	
	throw std::logic_error("Trying to get first unknown index from fixed line.");
}

int TAdjustableLine::getLastUidx() const {
	for (int i = 2; i >= 0; i--)
		if (!fixedStateLineVector[i])
			return uidx_lineVector[i];

	throw std::logic_error("Trying to get last unknown index from fixed line.");
}

void TAdjustableLine::setFirstUidx(int idx) {
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to fixed line.");

	for (int i = 0; i < 3; i++)
		if (!fixedStateLineVector[i])
			uidx_lineVector[i] = idx++;
}

void TAdjustableLine::setCorrection(int idx, TReal value) {

   TLength val(value);

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
	uidx_lineVector[0] = -1;
	uidx_lineVector[1] = -1;
	uidx_lineVector[2] = -1;
}