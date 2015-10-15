#ifndef _TCAM_H_
#define _TCAM_H_

#include <array>
#include "TUVD.h"
#include "TUVEC.h"
#include <Global.h>
#include <TInstrumentData.h>

class TAdjustablePoint;
struct TUVECObsSummary;
struct TUVDObsSummary;

/*!
	\ingroup Measurements
	\brief A camera station, camera positioned on a specific position,
	       which can take multiple unit vector (TUVEC) and unit vector + distance (TUVD) measurements.
*/
struct TCAM {

		/*!
			Creates a camera station that is centered on a given point.

			\param pos A reference to the point under the camera. Must be valid throughout the program.

			\param instrument A reference to the camera instrument. The object is stored as a deep copy.
		*/
		TCAM( const TAdjustablePoint& pos, const TInstrumentData::TCAMD& instrument) :
			instrument(instrument),
			instrumentPos(&pos),
			line(NO_VALi)
		    {}

		/// All UVD measurements made by this camera
		std::vector<TUVD> measUVD;
		/// All UVEC measurments made by this camera
		std::vector<TUVEC> measUVEC;

		/// The instrument that is used on this station
		TInstrumentData::TCAMD     instrument;

		/// The point on which the instrument resides
		const TAdjustablePoint* instrumentPos; 

		/// Line of the camera definition
		int  line;

		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
		TUVDObsSummary getUVDObsSummary()const;
		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
		TUVECObsSummary getUVECObsSummary()const;
	};

#endif