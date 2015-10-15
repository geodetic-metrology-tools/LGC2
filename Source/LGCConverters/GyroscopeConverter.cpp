////////////////////////////////////////////////////////////////////
// GyroscopeConverter.cpp

#include "GyroscopeConverter.h"

Gyroscope* GyroscopeConverter::readGyroscope()
{
	string instrID;
	TLength* sigmaInstrCentering = new TLength();
	string angleConstKnown;
	TAngle angleConst;
	TAngle* angleSig = new TAngle();
	TLength* sigmaTargetCentering = new TLength();

	*fStream >> instrID;
	fStream->setLengthUnits(TLength::kMillimetres);
	*fStream >> *sigmaInstrCentering;
	*fStream >> angleConstKnown;
	fStream->setAngleUnits(TAngle::kCCs);
	*fStream >> angleConst;
	*fStream >> *angleSig;
	*fStream >> *sigmaTargetCentering;

	fStream->readLine();

	return new Gyroscope(instrID, sigmaInstrCentering, sigmaTargetCentering,
		new TAngleConstants(angleConst, angleConstKnown == "true" ? TAMeasurement::kFixed : TAMeasurement::kVariable),
		angleSig);
}
