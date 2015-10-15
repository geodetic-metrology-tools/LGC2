#ifndef _TCAM_H_
#define _TCAM_H_

#include <array>
#include "TAMeas.h"
#include <Global.h>
#include <TInstrumentData.h>
#include "AdjustableObjects\TAdjustableAngle.h"
#include "AdjustableObjects\TAdjustableScalar.h"
#include "MeasDef.h"

class TAdjustablePoint;
struct TPLR3DObsSummary;
struct TDIR3DObsSummary;

struct TCAM {

		/// All PLR3D measurments made by this station
		std::vector<TPLR3D> measPLR3D;
		/// All DIR3D measurments made by this station
		std::vector<TDIR3D> measDIR3D;


		TPLR3DObsSummary getPLR3DObsSummary()const;
		TDIR3DObsSummary getDIR3DObsSummary()const;

		/// The instrument that is used on this station
		TInstrumentData::TPOLAR     instrument;

		/// The point on which the instrument resides
		const TAdjustablePoint* instrumentPos; 

		/// Line of the camera definition
		int  line;

		/*!
			Creates a total station that is centered on a given point with instrument data from the input file

			\param pos A reference to the point under the station. Must be valid throughout the program.

			\param instrument A reference to the instrument data. The object is stored as a deep copy.
		*/
		TCAM( const TAdjustablePoint& pos, const TInstrumentData::TPOLAR& instrument) :
			instrument(instrument),
			instrumentPos(&pos),
			line(NO_VALi)
		    {}
	};

#endif