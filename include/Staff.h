// Staff.h


#ifndef SU_STAFF
#define SU_STAFF

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Target.h"
#include "TAngle.h"
#include "TLength.h"
#include "TDistConstants.h"

class Staff : public Target
{

public:

	Staff(const string& tID, const TLength* ds, const TLength* ppmS, const TDistConstants* dc, const TLength* dcs,
		const TLength* th, const TLength* ths, int dcun) : Target(tID, NULL, th, ths)
	{
		distanceSigma = ds;
		ppm = ppmS;
		distanceCorrection = dc;
		distanceCorrectionSigma = dcs;
		distanceCorrectionUnknownNumber = dcun;
	}

	~Staff()
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

#endif //SU_STAFF
