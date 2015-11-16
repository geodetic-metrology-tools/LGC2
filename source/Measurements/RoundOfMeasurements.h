#ifndef Round_Of_Measurements
#define Round_Of_Measurements

#include "MeasDef.h"
#include "TAdjustablePlane.h"
#include "TAdjustableLine.h"
class TLGCObsSummary;

/*!
	\ingroup Measurements
	\brief This class represents a round of ECHO (TECHO) measurements, which are measuring a single Plane.
*/
struct TECHOROM {

		/// All ECHO measurements, measuring the 'fmeasuredPlane'
		std::vector<TECHO> measECHO;

		//Measured vertical plane
		TAdjustablePlane* fMeasuredPlane;

		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
		TLGCObsSummary getECHOObsSummary() const;

		/// Line of the measurement definition
		int  line;

		/// the station attribute is a copy of the parameter to override defaults
		TECHOROM(TAdjustablePlane& measPlane) :
			fMeasuredPlane(&measPlane),
			line(NO_VALi)
			{}
};

/*!
\ingroup Measurements
\brief This class represents a round of ECSP (TECSP) measurements, which are measuring a single Plane.
*/
struct TECSPROM {

	/// All ECHO measurements, measuring the 'fmeasuredPlane'
	std::vector<TECSP> measECSP;

	//Measured vertical plane
	TAdjustablePlane* fMeasuredPlane;

	/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
	TLGCObsSummary getECSPObsSummary() const;

	/// Line of the measurement definition
	int  line;

	/// the station attribute is a copy of the parameter to override defaults
	TECSPROM(TAdjustablePlane& measPlane) :
		fMeasuredPlane(&measPlane),
		line(NO_VALi)
	{}
};

/*!
\ingroup Measurements
\brief This class represents a round of ECVE (TECVE) measurements, which are measuring a single Plane.
*/
struct TECVEROM {

	/// All ECHO measurements, measuring the 'fmeasuredPlane'
	std::vector<TECVE> measECVE;

	//Measured vertical plane
	TAdjustableLine* fMeasuredLine;

	/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
	TLGCObsSummary getECVEObsSummary() const;

	/// Line of the measurement definition
	int  line;

	/// the station attribute is a copy of the parameter to override defaults
	TECVEROM(TAdjustableLine& measLine) :
		fMeasuredLine(&measLine),
		line(NO_VALi)
	{}
};

/*!
	\ingroup Measurements
	\brief This class represents a round of ORIE (TORIE) measurements made from a single position of the instrument.
*/
struct TORIEROM {

		/// All ORIE measurements
		std::vector<TORIE> measORIE;

		/// The position of the instrument
		const TAdjustablePoint* instrumentPos;

		/// The instrument that is used on this station
		TInstrumentData::TPOLAR     instrument;

		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
		TLGCObsSummary getORIEObsSummary() const;

		TAngle fConstantAngle;

		/// Line of the measurement definition
		int  line;

		/// the station attribute is a copy of the parameter to override defaults
		TORIEROM(const TAdjustablePoint& pos, const TInstrumentData::TPOLAR& instrument) :
			instrumentPos(&pos),
			instrument(instrument),
			line(NO_VALi),
			fConstantAngle(0.0, TAngle::EUnits::kGons)
		{
		}
};

#endif