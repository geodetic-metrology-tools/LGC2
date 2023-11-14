/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _TLEVEL_H
#define _TLEVEL_H


//LGC
#include <MeasDef.h>
#include <LGCAdjustablePlane.h>
#include "TLGCObsSummary.h"


/*!
	\ingroup Measurements
	\brief Levelling station, which is a number of levelling measurements (TDLEV) of a single plane. The measurements are grouped by the plane.
	DHOR measurements can be optionally taken.
*/
struct TLEVEL : public TStatusObject {

		/// All DLEV measurements, measuring the plane (fmeasuredPlane).
		std::list<TDLEV> measDLEV;

		/// Tells if the Level Station has AT LEST one DHOR measurement
		bool hasDHOR;

        //! Initialise the observation summaries
        void initialiseObsSummaries();

		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
        const TLGCObsSummary& getDLEVObsSummary() const;
		const TLGCObsSummary& getDLEVObsSummary(std::string text) noexcept;
		
		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
        const TLGCObsSummary& getDHORObsSummary() const;
		const TLGCObsSummary& getDHORObsSummary(std::string text) noexcept;

		/// The plane which is measured
		LGCAdjustablePlane* fMeasuredPlane;

        /// The reference point
        LGCAdjustablePoint const * fRefPt;

		/// Levelling instrument which does the measurements
		TInstrumentData::TLEVEL  instrument;

		/// Line of the measurement definition
		int  line;

        int stnId{ stnCounter_++ };

		// The station attribute is a copy of the parameter to override defaults
		TLEVEL(const LGCAdjustablePoint* refPt, const TInstrumentData::TLEVEL& instrument) :
			fMeasuredPlane(nullptr),
            fRefPt(refPt),
			line(NO_VALi),
			instrument(instrument),
			hasDHOR(false)
			{}
			
#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(ObjectSerializer &obj) const override;
#endif
			
    private:

        static int stnCounter_;

        TLGCObsSummary dlevSummary_;
        TLGCObsSummary dhorSummary_;

};
#endif
