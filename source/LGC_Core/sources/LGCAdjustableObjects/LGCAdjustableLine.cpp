#include "LGCAdjustableLine.h"
#include <LGCAdjustablePoint.h>

//Reference point is the point on the line at the same time
LGCAdjustableLine::LGCAdjustableLine(const LGCAdjustablePoint* pointOnALine, const TFreeVector& lineVect, const std::bitset<3>& lineVectorFixedState, const std::string& name) :
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
LGCAdjustableLine::LGCAdjustableLine(const LGCAdjustablePoint* pointOnALine, const TPositionVector& referencePoint, const TFreeVector& lineVect, const std::bitset<3>& lineVectorFixedState, const std::string& name) :
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

void LGCAdjustableLine::initialize(const LGCAdjustablePoint* pointOnALine, const TFreeVector& lineVect, const std::bitset<3>& lineVectorFixedState)
{
	fLinePoint = pointOnALine;
	fReferencePointPosition = pointOnALine->getProvisionalValue();
	fLineVectorProvisionalValue=lineVect;
	fLineVectorEstimatedValue=lineVect;

	fixedStateLineVector=lineVectorFixedState;
	fInit = true;
}

void LGCAdjustableLine::initialize(const LGCAdjustablePoint* pointOnALine, const TPositionVector& referencePoint, const TFreeVector& lineVect, const std::bitset<3>& lineVectorFixedState)
{
	fLinePoint = pointOnALine;
	fReferencePointPosition = referencePoint;
	fLineVectorProvisionalValue = lineVect;
	fLineVectorEstimatedValue = lineVect;

	fixedStateLineVector = lineVectorFixedState;
	fInit = true;
}


void LGCAdjustableLine::reInitialise(){
	fLineVectorEstimatedPrecision = TFreeVector(LITERAL(0.0), LITERAL(0.0), LITERAL(0.0), TCoordSysFactory::k3DCartesian);
	fLineVectorCorrection = TFreeVector(LITERAL(0.0), LITERAL(0.0), LITERAL(0.0), TCoordSysFactory::k3DCartesian);
}


LGCAdjustableLine LGCAdjustableLine::createUninitialized(const std::string& name) {
	LGCAdjustableLine ap(nullptr, TFreeVector(NO_VALf,  NO_VALf,  NO_VALf,TCoordSysFactory::ECoordSys::k3DCartesian),std::bitset<3>(), name);
	ap.fInit = false;
	return ap;
}

int LGCAdjustableLine::getNumUnkn() const {
	return (int)!fixedStateLineVector[0] + (int)!fixedStateLineVector[1] + (int)!fixedStateLineVector[2];
}
		
int LGCAdjustableLine::getFirstUidx() const {			
	for (int i = 0; i < 3; i++)
		if (!fixedStateLineVector[i])
			return uidx_lineVector[i];		
	
	throw std::logic_error("Trying to get first unknown index from fixed line.");
}

int LGCAdjustableLine::getLastUidx() const {
	for (int i = 2; i >= 0; i--)
		if (!fixedStateLineVector[i])
			return uidx_lineVector[i];

	throw std::logic_error("Trying to get last unknown index from fixed line.");
}

void LGCAdjustableLine::setFirstUidx(int idx) {
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to fixed line.");

	for (int i = 0; i < 3; i++)
		if (!fixedStateLineVector[i])
			uidx_lineVector[i] = idx++;
}

void LGCAdjustableLine::setCorrection(int idx, TReal value) {

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

void LGCAdjustableLine::setEstVal(int idx, TReal value) {

   TLength val(value);

	for (int i = 0; i < 3; i++)
	{
		if (uidx_lineVector[i] == idx)
		{
			if (i == 0)
			{
				fLineVectorEstimatedValue.setX(val);
			}
			else if (i == 1)
			{
				fLineVectorEstimatedValue.setY(val);
			}
			else
			{
				fLineVectorEstimatedValue.setZ(val);
			}
			return;
		}
	}
	throw std::logic_error("Invalid unknown index in parameter access.");
}

/*! Sets the estimated precision after calculation to a line's point.*/
void	LGCAdjustableLine::setLineVectorEstimatedPrecision(int idx, TReal value){
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


void LGCAdjustableLine::setDefaults(){
	uidx_lineVector[0] = -1;
	uidx_lineVector[1] = -1;
	uidx_lineVector[2] = -1;
}


#if USE_SERIALIZER
void LGCAdjustableLine::serialize(SerializerObject::SerializationHelper &obj) const
{
	TVAdjustableObject::serialize(obj);
	obj.addProperty("fInit", fInit);
	obj.addProperty("fixedStateLineVectorX", fixedStateLineVector[0]);
	obj.addProperty("fixedStateLineVectorY", fixedStateLineVector[1]);
	obj.addProperty("fixedStateLineVectorZ", fixedStateLineVector[2]);

	obj.addProperty("fLinePoint", fLinePoint);
	obj.addProperty("fLineVectorCorrection", fLineVectorCorrection);
	obj.addProperty("fLineVectorCovariance", fLineVectorCovariance);

	obj.addProperty("fLineVectorEstimatedPrecision", fLineVectorEstimatedPrecision);
	obj.addProperty("fLineVectorEstimatedValue", fLineVectorEstimatedValue);
	obj.addProperty("fLineVectorProvisionalValue", fLineVectorProvisionalValue);
	obj.addProperty("fName", fName);
	obj.addProperty("fReferencePointPosition", fReferencePointPosition);

	obj.addProperty("uidx_lineVector", uidx_lineVector);
}
#endif // USE_SERIALIZER
