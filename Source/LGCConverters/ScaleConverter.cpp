////////////////////////////////////////////////////////////////////
// TheodoliteConverter.cpp


#include "ScaleConverter.h"

Scale* ScaleConverter::readScale(int& nextUnknownsNumber)
{
	string instrID;
	TLength* sigmaDSpt = new TLength();
	TLength* ppmDSpt = new TLength();
	TLength distCorrectionValue;
	TLength* sigmaDCorr = new TLength();
	TLength* sigmaInstrCentering = new TLength();
	TLength* targetOffset = new TLength();
	TLength* sigmaTargetOffset = new TLength();

	*fStream >> instrID;
	fStream->setLengthUnits(TLength::kMillimetres);
	*fStream >> *sigmaDSpt;
	*fStream >> *ppmDSpt;
	fStream->setLengthUnits(TLength::kMetres);
	*fStream >> distCorrectionValue;
	fStream->setLengthUnits(TLength::kMillimetres);
	*fStream >> *sigmaDCorr;
	*fStream >> *sigmaInstrCentering;
	*fStream >> *targetOffset;
	*fStream >> *sigmaTargetOffset;

	Scale* result = new Scale(instrID, sigmaDSpt, ppmDSpt,
		new TDistConstants(distCorrectionValue, TAMeasurement::kFixed),
		sigmaDCorr, sigmaInstrCentering,
		targetOffset, sigmaTargetOffset, nextUnknownsNumber++);

	fStream->readLine();

	return result;
}
