// TheodoliteTarget.h


#ifndef SU_THEODOLITE_TARGET
#define SU_THEODOLITE_TARGET

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Target.h"
#include "TAngle.h"
#include "TLength.h"
#include "TDistConstants.h"

class TheodoliteTarget : public Target
{

public:

	TheodoliteTarget(const string& tID, const TAngle* as, const TAngle* zds, const TLength* ds, const TLength* ppmS, const TDistConstants* dc, const TLength* dcs,
		const TLength* tcs, const TLength* th, const TLength* ths, int dcun) : Target(tID, tcs, th, ths)
	{
		angleSigma = as;
		zenithDistanceSigma = zds;
		distanceSigma = ds;
		ppm = ppmS;
		distanceCorrection = dc;
		distanceCorrectionSigma = dcs;
		distanceCorrectionUnknownNumber = dcun;
	}

	~TheodoliteTarget()
	{
        delete angleSigma;
        delete zenithDistanceSigma;
        delete distanceSigma;
        delete ppm;
        delete distanceCorrection;
        delete distanceCorrectionSigma;
	}

	const TAngle* getAngleSigma() const { return angleSigma; }

	const TAngle* getZenithDistanceSigma() const { return zenithDistanceSigma; }

	const TLength* getDistanceSigma() const { return distanceSigma; }

	const TLength* getPPM() const { return ppm; }

	const TDistConstants* getDistanceCorrection() const { return distanceCorrection; }

	const TLength* getDistanceCorrectionSigma() const { return distanceCorrectionSigma; }

	int getDistanceCorrectionUnknownNumber() const { return distanceCorrectionUnknownNumber; }

private:

	const TAngle* angleSigma;
	const TAngle* zenithDistanceSigma;
	const TLength* distanceSigma;
	const TLength* ppm;
	const TDistConstants* distanceCorrection;
	const TLength* distanceCorrectionSigma;

	int distanceCorrectionUnknownNumber;
};

#endif //SU_THEODOLITE_TARGET
