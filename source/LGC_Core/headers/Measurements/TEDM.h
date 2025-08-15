/*
 * SPDX-FileCopyrightText: 2025 CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _TEDM_H
#define _TEDM_H

//LGC
#include <MeasDef.h>
#include "TLGCObsSummary.h"


/*!
	\ingroup Measurements
	\brief The EDM station, which groups TDSPT measurements made by a single instrument.
*/
struct TEDM : public TStatusObject {

		// Constructor of the EDM station
		TEDM(const LGCAdjustablePoint& pos, const TInstrumentData::TEDM& instrument) :
			instrument(instrument),
			instrumentPos(&pos),
			line(NO_VALi)
			{}

		/// All DSPT measurments made by this EDM station.
		std::list<TDSPT> measDSPT;

		/// The instrument that is used on this station
		TInstrumentData::TEDM instrument;

		/// The point on which the instrument resides
		const LGCAdjustablePoint* instrumentPos; 	

        //! Initialise the observation summaries
        void initialiseObsSummaries();

		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
        const TLGCObsSummary& getDSPTObsSummary() const;
		const TLGCObsSummary& getDSPTObsSummary(std::string text) noexcept;


		/// Line of the station definition
		int  line;

        int stnId{ stnCounter_++ };

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override;
#endif

    private:

        static int stnCounter_;

        TLGCObsSummary dsptSummary_;
};
#endif