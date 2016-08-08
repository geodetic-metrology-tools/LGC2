#include "MeasDef.h"
#include "LGCAdjustablePlane.h"

class TLGCObsSummary;

/*!
	\ingroup Measurements
	\brief Levelling station, which is a number of levelling measurements (TDLEV) of a single plane. The measurements are grouped by the plane.
	DHOR measurements can be optionally taken.
*/
struct TLEVEL {

		/// All DLEV measurements, measuring the plane (fmeasuredPlane).
		std::vector<TDLEV> measDLEV;

		/// Tells if the Level Station has AT LEST one DHOR measurement
		bool hasDHOR;

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

		// The station attribute is a copy of the parameter to override defaults
		TLEVEL(LGCAdjustablePlane& measPlane, const TInstrumentData::TLEVEL& instrument) :
			fMeasuredPlane(&measPlane),
			line(NO_VALi),
			instrument(instrument),
			hasDHOR(false)
			{}
};