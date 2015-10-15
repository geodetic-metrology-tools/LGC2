#ifndef _TCAM_H_
#define _TCAM_H_

#include <array>
#include "TUVD.h"
#include "TUVEC.h"
#include <Global.h>
#include <TInstrumentData.h>
#include "AdjustableObjects\TAdjustableAngle.h"
#include "AdjustableObjects\TAdjustableScalar.h"
#include "MeasDef.h"

class TAdjustablePoint;
struct TUVECObsSummary;
struct TUVDObsSummary;

struct TCAM {
		/// All UVD measurments made by this station
		std::vector<TUVD> measUVD;
		/// All UVEC measurments made by this station
		std::vector<TUVEC> measUVEC;
		
		TUVDObsSummary getUVDObsSummary()const;
		TUVECObsSummary getUVECObsSummary()const;

		/// The instrument that is used on this station
		TInstrumentData::TCAMD     instrument;

		/// The point on which the instrument resides
		const TAdjustablePoint* instrumentPos; 

		/// Line of the camera definition
		int  line;

		/*!
			Creates a total station that is centered on a given point with instrument data from the input file

			\param pos A reference to the point under the station. Must be valid throughout the program.

			\param instrument A reference to the instrument data. The object is stored as a deep copy.
		*/
		TCAM( const TAdjustablePoint& pos, const TInstrumentData::TCAMD& instrument) :
			instrument(instrument),
			instrumentPos(&pos),
			line(NO_VALi)
		    {}
	};

#endif