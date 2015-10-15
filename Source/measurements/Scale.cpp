// 
// Scale.cpp : implementation file
//

#include "Scale.h"


Scale::Scale(const string& id, const TLength* const spDSigma, const TLength* ppmE,
			 const TDistConstants* dc, const TLength* sdc, const TLength* sic, const TLength* to, const TLength* sto, int dcun)
	: TAFreeInstrument(id, sic)
{	
	distanceConstant = dc;

	distanceConstantSigma = sdc;
	spatialDistanceSigma = spDSigma;
	ppm = ppmE;
	targetOffset = to;
	targetOffsetSigma = sto;

	distanceCorrectionUnknownNumber = dcun;
}

Scale::~Scale()
{
	delete distanceConstant;
	delete distanceConstantSigma;
	delete spatialDistanceSigma;
	delete ppm;
	delete targetOffset;
	delete targetOffsetSigma;
}
