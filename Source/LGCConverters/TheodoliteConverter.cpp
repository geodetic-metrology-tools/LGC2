////////////////////////////////////////////////////////////////////
// TheodoliteConverter.cpp


#include "TheodoliteConverter.h"

TTheodolite* TheodoliteConverter::readTheodolite(int& lineNumber, int& nextUnknownsNumber)
{
	string instrID;
	string defaultTargetID;
	TLength* instrHt = new TLength();
	TLength* sigmaInstrHt = new TLength();
	TLength* sigmaInstrCentering = new TLength();
	TAngle constAngl;

	*fStream >> instrID;
	*fStream >> defaultTargetID;
	fStream->setLengthUnits(TLength::kMetres);
	*fStream >> *instrHt;
	fStream->setLengthUnits(TLength::kMillimetres);
	*fStream >> *sigmaInstrHt;
	*fStream >> *sigmaInstrCentering;
	fStream->setAngleUnits(TAngle::kCCs);
	*fStream >> constAngl;

	TTheodolite* result = new TTheodolite(instrID, instrHt, sigmaInstrHt,
		sigmaInstrCentering, new TAngleConstants(constAngl, TAMeasurement::kFixed));

	lineNumber++;
	fStream->readLine();

	while (fStream->peek() != '*')
	{
		readTheodoliteTargets(result, defaultTargetID, nextUnknownsNumber);
		lineNumber++;
	}

	if (result->getDefaultTarget() == NULL)
	{
		// TODO: some error
	}

	return result;
}

void TheodoliteConverter::readTheodoliteTargets(TTheodolite* theo, const string& defaultTargetID, int& nextUnknownsNumber)
{		
	string targetID;
	TAngle* sigmaAngl = new TAngle();
	TAngle* sigmaZenD = new TAngle();
	TLength* sigmaDist = new TLength();
	TLength* ppmDist = new TLength();
	string distCorrKnown;
	TLength distCorrectionValue;
	TLength* sigmaDCorr = new TLength();
	TLength* sigmaTargetCentering = new TLength();
	TLength* targetHt = new TLength();
	TLength* sigmaTargetHt = new TLength();

	*fStream >> targetID;
	fStream->setAngleUnits(TAngle::kCCs);
	*fStream >> *sigmaAngl;
	*fStream >> *sigmaZenD;
	fStream->setLengthUnits(TLength::kMillimetres);
	*fStream >> *sigmaDist;
	*fStream >> *ppmDist;
	*fStream >> distCorrKnown;
	fStream->setLengthUnits(TLength::kMetres);
	*fStream >> distCorrectionValue;
	fStream->setLengthUnits(TLength::kMillimetres);
	*fStream >> *sigmaDCorr;
	*fStream >> *sigmaTargetCentering;
	fStream->setLengthUnits(TLength::kMetres);
	*fStream >> *targetHt;
	fStream->setLengthUnits(TLength::kMillimetres);
	*fStream >> *sigmaTargetHt;

	TheodoliteTarget *target = new TheodoliteTarget(targetID, sigmaAngl, sigmaZenD,
		sigmaDist, ppmDist,
		new TDistConstants(distCorrectionValue, distCorrKnown == "true" ? TAMeasurement::kFixed : TAMeasurement::kVariable),
		sigmaDCorr, sigmaTargetCentering,
		targetHt, sigmaTargetHt, nextUnknownsNumber++);

	theo->addTarget(target, defaultTargetID == targetID);

	fStream->readLine();
}
