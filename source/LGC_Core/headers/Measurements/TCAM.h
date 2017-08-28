#ifndef _TCAM_H_
#define _TCAM_H_

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

//STL
#include <array>
//LGC
#include <TUVD.h>
#include <TUVEC.h>
#include <Global.h>
#include <TInstrumentData.h>
#include "TLGCObsSummary.h"

class LGCAdjustablePoint;

/*!
	\ingroup Measurements
	\brief A camera station, camera positioned on a specific position,
	       which can take multiple unit vector (TUVEC) and unit vector + distance (TUVD) measurements.
*/
struct TCAM : public TStatusObject {

		/*!
			Creates a camera station that is centered on a given point.

			\param pos A reference to the point under the camera. Must be valid throughout the program.

			\param instrument A reference to the camera instrument. The object is stored as a deep copy.
		*/
		TCAM( const LGCAdjustablePoint& pos, const TInstrumentData::TCAMD& instrument) :
			instrument(instrument),
			instrumentPos(&pos),
			line(NO_VALi)
		    {}

		/// All UVD measurements made by this camera
		std::list<TUVD> measUVD;
		/// All UVEC measurments made by this camera
		std::list<TUVEC> measUVEC;

        bool uvdActive{ true }; ///< activation status of the UVD rom
        bool uvecActive{ true }; ///< activation status of the UVEC rom

		/// The instrument that is used on this station
		TInstrumentData::TCAMD     instrument;

		/// The point on which the instrument resides
		const LGCAdjustablePoint* instrumentPos; 

		/// Line of the camera definition
		int  line;

        int stnId{ stnCounter_++ };

        //! Initialise the observation summaries
        void initialiseObsSummaries();

		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
		const TUVDObsSummary& getUVDObsSummary() const;
		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
		const TUVECObsSummary& getUVECObsSummary() const;

    private:
        
        static int stnCounter_;

        TUVDObsSummary uvdSummary_;
        TUVECObsSummary uvecSummary_;

	};

#endif