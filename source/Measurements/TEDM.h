#include "MeasDef.h"

class TLGCObsSummary;

/*!
	\ingroup Measurements
	\brief The EDM station, which groups TDSPT measurements made by a single instrument.
*/
struct TEDM {

		// Constructor of the EDM station
		TEDM(const TAdjustablePoint& pos, const TInstrumentData::TEDM& instrument) :
			instrument(instrument),
			instrumentPos(&pos),
			line(NO_VALi)
			{}

		/// All DSPT measurments made by this EDM station.
		std::vector<TDSPT> measDSPT;

		/// The instrument that is used on this station
		TInstrumentData::TEDM instrument;

		/// The point on which the instrument resides
		const TAdjustablePoint* instrumentPos; 	

		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
		TLGCObsSummary getDSPTObsSummary() const;

		/// Line of the station definition
		int  line;
};