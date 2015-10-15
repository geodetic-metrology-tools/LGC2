// EDMTarget.h


#ifndef SU_EDM_TARGET
#define SU_EDM_TARGET

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Target.h"
#include "TAngle.h"
#include "TLength.h"
#include "TDistConstants.h"

class EDMTarget : public Target
{

public:

	EDMTarget(const string& tID, const TLength* ds, const TLength* ppmS, const TDistConstants* dc, const TLength* dcs,
		const TLength* tcs, const TLength* th, const TLength* ths, int dcun) : Target(tID, tcs, th, ths)
	{
		distanceSigma = ds;
		ppm = ppmS;
		distanceCorrection = dc;
		distanceCorrectionSigma = dcs;
		distanceCorrectionUnknownNumber = dcun;
	}

	~EDMTarget()
	{
        delete distanceSigma;
        delete ppm;
        delete distanceCorrection;
        delete distanceCorrectionSigma;
	}

	const TLength* getDistanceSigma() const { return distanceSigma; }

	const TLength* getPPM() const { return ppm; }

	const TDistConstants* getDistanceCorrection() const { return distanceCorrection; }

	const TLength* getDistanceCorrectionSigma() const { return distanceCorrectionSigma; }

	int getDistanceCorrectionUnknownNumber() const { return distanceCorrectionUnknownNumber; }

private:  

	const TLength* distanceSigma;
	const TLength* ppm;
	const TDistConstants* distanceCorrection;
	const TLength* distanceCorrectionSigma;

	int distanceCorrectionUnknownNumber;
};

#endif //SU_EDM_TARGET
