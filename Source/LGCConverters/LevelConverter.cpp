////////////////////////////////////////////////////////////////////
// LevelConverter.cpp


#include "LevelConverter.h"

Level* LevelConverter::readLevel(int& lineNumber, int& nextUnknownsNumber)
{
	string instrID;
	string defaultStaffID;

	*fStream >> instrID;
	*fStream >> defaultStaffID;

	Level* result = new Level(instrID);
	lineNumber++;
	fStream->readLine();

	while (fStream->peek() != '*')
	{
		readLevelTargets(result, defaultStaffID, nextUnknownsNumber);
		lineNumber++;
	}

	return result;
}

void LevelConverter::readLevelTargets(Level* level, const string& defaultStaffID, int& nextUnknownsNumber)
{  
	string staffID;
	TLength* sigmaDist = new TLength();
	TLength* ppmDist = new TLength();
	string distCorrKnown;
	TLength distCorrectionValue;
	TLength* sigmaDCorr = new TLength();
	TLength* targetHt = new TLength();
	TLength* sigmaTargetHt = new TLength();

	*fStream >> staffID;
	fStream->setLengthUnits(TLength::kMillimetres);
	*fStream >> *sigmaDist;
	*fStream >> *ppmDist;
	*fStream >> distCorrKnown;
	fStream->setLengthUnits(TLength::kMetres);
	*fStream >> distCorrectionValue;
	fStream->setLengthUnits(TLength::kMillimetres);
	*fStream >> *sigmaDCorr;
	fStream->setLengthUnits(TLength::kMetres);
	*fStream >> *targetHt;
	fStream->setLengthUnits(TLength::kMillimetres);
	*fStream >> *sigmaTargetHt;

	Staff *staff = new Staff(staffID, sigmaDist, ppmDist,
		new TDistConstants(distCorrectionValue, distCorrKnown == "true" ? TAMeasurement::kFixed : TAMeasurement::kVariable),
		sigmaDCorr, targetHt, sigmaTargetHt, nextUnknownsNumber++);

	level->addStaff(staff, defaultStaffID == staffID);

	fStream->readLine();
}
