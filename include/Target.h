// Target.h


#ifndef SU_TARGET
#define SU_TARGET

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TLength.h"

class Target {

public:

	const std::string& getTargetID() const { return targetID; }
	const TLength* getTargetHeight() const { return targetHeight; }
	const TLength* getTargetHeightSigma() const { return targetHeightSigma; }
	const TLength* getTargetCenteringSigma() const { return targetCenteringSigma; }

protected:	

	Target(const std::string& tID, const TLength* tcs, const TLength* th, const TLength* ths)
	{
		targetID = tID;
		targetHeight = th;
		targetHeightSigma = ths;
		targetCenteringSigma = tcs;
	}

	virtual ~Target()
	{
		delete targetHeight;
		delete targetHeightSigma;
		delete targetCenteringSigma;
	}

	std::string targetID;
	const TLength* targetHeight;
	const TLength* targetHeightSigma;
	const TLength* targetCenteringSigma;
};

#endif //SU_TARGET
