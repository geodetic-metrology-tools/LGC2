#ifndef _TLEVEL_H
#define _TLEVEL_H

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

//LGC
#include <MeasDef.h>
#include <LGCAdjustablePlane.h>
#include "TLGCObsSummary.h"


/*!
	\ingroup Measurements
	\brief Levelling station, which is a number of levelling measurements (TDLEV) of a single plane. The measurements are grouped by the plane.
	DHOR measurements can be optionally taken.
*/
struct TLEVEL {

		/// All DLEV measurements, measuring the plane (fmeasuredPlane).
		std::list<TDLEV> measDLEV;

		/// Tells if the Level Station has AT LEST one DHOR measurement
		bool hasDHOR;

        //! Initialise the observation summaries
        void initialiseObsSummaries();

		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
		TLGCObsSummary getDLEVObsSummary() const;		
		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
		TLGCObsSummary getDHORObsSummary() const;

		/// The plane which is measured
		LGCAdjustablePlane* fMeasuredPlane;

		/// Levelling instrument which does the measurements
		TInstrumentData::TLEVEL  instrument;

		/// Line of the measurement definition
		int  line;

        int stnId{ stnCounter_++ };

		// The station attribute is a copy of the parameter to override defaults
		TLEVEL(LGCAdjustablePlane& measPlane, const TInstrumentData::TLEVEL& instrument) :
			fMeasuredPlane(&measPlane),
			line(NO_VALi),
			instrument(instrument),
			hasDHOR(false)
			{}

    private:

        static int stnCounter_;

        TLGCObsSummary dlevSummary_;
        TLGCObsSummary dhorSummary_;

};
#endif
