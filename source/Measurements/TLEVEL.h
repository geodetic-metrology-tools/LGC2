#include "MeasDef.h"

struct TLEVEL {

		/// All DLEV measurements, measuring the 'fmeasuredPlane'
		std::vector<TDLEV> measDLEV;

		/// Additional horiozontal distance measurements, between Reference point and Staff of the Level instrument (target), in a case that are provided (optional)
		std::vector<TDHOR> measDHOR;

		//Measured plane
		TAdjustablePlane* fMeasuredPlane;

		/// Levelling instrument which does measurements
		TInstrumentData::TLEVEL  instrument;

		/// Line of the measurement definition
		int  line;

		// the station attribute is a copy of the parameter to override defaults
		TLEVEL(TAdjustablePlane& measPlane, const TInstrumentData::TLEVEL& instrument) :
			fMeasuredPlane(&measPlane),
			line(NO_VALi),
			instrument(instrument)
			{}
};


struct TECHOROM {

		/// All DLEV measurements, measuring the 'fmeasuredPlane'
		std::vector<TECHO> measECHO;

		//Measured plane
		TAdjustablePlane* fMeasuredPlane;

		/// Line of the measurement definition
		int  line;

		// the station attribute is a copy of the parameter to override defaults
		TECHOROM(TAdjustablePlane& measPlane) :
			fMeasuredPlane(&measPlane),
			line(NO_VALi)
			{}
};