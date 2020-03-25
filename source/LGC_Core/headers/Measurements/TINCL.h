/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _TINCL_H_
#define _TINCL_H_

//SURVEYLIB
#include <TAdjustableAngle.h>
//LGC
#include <Global.h>
#include <TInstrumentData.h>
#include <MeasDef.h>
#include "TLGCObsSummary.h"

/*!
	\ingroup Measurements
	\brief The Total Station (TSTN) is a TPOLAR instrument positioned on a point, which can take measurements of different types.

	TROM = round of measurments groups the particular measurements, all of them have common: constant orientation acst and calculated orientation angle v0.
*/
struct TINCL : public TStatusObject {


	/// All INCLY measurements in this ROM
	std::list<TINCLY> measINCLY;

	//! Initialise the observation summaries
	void initialiseObsSummaries();

	/*!
		\returns Summary of all the ANGL measurements.
		\note This function can be called only when the calculation is finished and the residuals of the observations are already set.
	*/
	const TLGCObsSummary& getINCLYObsSummary() const;

	const TLGCObsSummary& getINCLYObsSummaryAllFrame() const;

	/// The instrument that is used on this station
	TInstrumentData::TINCL instrument;

	/// The point on which the instrument resides
	const LGCAdjustablePoint* instrumentPos;

	/// Line of the station definition
	int  line;


	int stnId{ stnCounter_++ };

	// The station attribute is a copy of the parameter to override defaults
	TINCL(const LGCAdjustablePoint& instrumentPos, const TInstrumentData::TINCL& instrument) :
		instrument(instrument),
		instrumentPos(&instrumentPos),
		line(NO_VALi)
	{}

private:

	static int stnCounter_;

	TLGCObsSummary inclySummary_;

};

#endif