////////////////////////////////////////////////////////////////////
// EDMConverter.cpp


#include "EDMConverter.h"

EDM* EDMConverter::readEDM(int& lineNumber, int& nextUnknownsNumber)
{
	string instrID;
	string defaultTargetID;
	TLength* instrHt = new TLength();
	TLength* sigmaInstrHt = new TLength();
	TLength* sigmaInstrCentering = new TLength();

	*fStream >> instrID;
	*fStream >> defaultTargetID;
	fStream->setLengthUnits(TLength::kMetres);
	*fStream >> *instrHt;
	fStream->setLengthUnits(TLength::kMillimetres);
	*fStream >> *sigmaInstrHt;
	*fStream >> *sigmaInstrCentering;

	EDM* result = new EDM(instrID, instrHt, sigmaInstrHt, sigmaInstrCentering);
	lineNumber++;
	fStream->readLine();

	while (fStream->peek() != '*')
	{
		readEDMTargets(result, defaultTargetID, nextUnknownsNumber);
		lineNumber++;
	}

	if (result->getDefaultTarget() == NULL)
	{
		// TODO: some error
	}

	return result;
}

void EDMConverter::readEDMTargets(EDM* edm, const string& defaultTargetID, int& nextUnknownsNumber)
{
	string targetID;
	TLength* sigmaDist = new TLength();
	TLength* ppmDist = new TLength();
	string distCorrKnown;
	TLength distCorrectionValue;
	TLength* sigmaDCorr = new TLength();
	TLength* sigmaTargetCentering = new TLength();
	TLength* targetHt = new TLength();
	TLength* sigmaTargetHt = new TLength();

	*fStream >> targetID;
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

	EDMTarget *target = new EDMTarget(targetID,	sigmaDist, ppmDist,
		new TDistConstants(distCorrectionValue, distCorrKnown == "true" ? TAMeasurement::kFixed : TAMeasurement::kVariable),
		sigmaDCorr, sigmaTargetCentering,
		targetHt, sigmaTargetHt, nextUnknownsNumber++);

	edm->addTarget(target, defaultTargetID == targetID);

	fStream->readLine();
}
