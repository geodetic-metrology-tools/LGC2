#include "MeasDef.h"

struct TEDM {

		/// All DSPT measurments made by this EDM station
		std::vector<TDSPT> measDSPT;

		/// the instrument that is used on this station
		TInstrumentData::TEDM instrument;

		/// the point on which the instrument resides
		const TAdjustablePoint* instrumentPos; 	

		/// Adjustable instrument height
		const TAdjustableScalar* instrumentHeightAdjustable;

		/// Line of the station definition
		int  line;

		// the station attribute is a copy of the parameter to override defaults
		TEDM(const TAdjustablePoint& pos, const TInstrumentData::TEDM& instrument) :
			instrument(instrument),
			instrumentPos(&pos),
			line(NO_VALi),
			instrumentHeightAdjustable(NULL)
			{}
	};