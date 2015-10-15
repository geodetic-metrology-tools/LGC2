#ifndef _TTSTN_H_
#define _TTSTN_H_

#include <array>
#include "TAMeas.h"
#include <Global.h>
#include <TInstrumentData.h>
#include "MeasDef.h"

class TAdjustablePoint;
class TAdjustableAngle;
class TAdjustableScalar;
class TLGCObsSummary;
struct T3ObsSummary;
struct T2ObsSummary;

struct TTSTN {
		/// A round of total station measurements
		struct TROM {
			LGC::TAngle acst; ///< constant angle/orientation of the station, defaults to zero

			TAdjustableAngle* v0; ///< V0 angle/orientation, defaults to zero, to be set after LS calculation, variable

			const TInstrumentData::TPOLAR::TTarget* defaultTarget; 
			
			/// All PLR3D measurments in this ROM
			std::vector<TPLR3D> measPLR3D;
			/// All DIR3D measurments in this ROM, THIS MEASUREMENT IS DEPRECIATED, WILL BE SOON DELETED COMPLETELY
			std::vector<TDIR3D> measDIR3D;
			/// All horizonthal angle measurements in this ROM
			std::vector<TANGL>  measANGL;
			/// All zenith distance measurments in this ROM
			std::vector<TZEND>  measZEND;
			/// All spatial distane measurments in this ROM
			std::vector<TLINE>  measDIST;
			/// All horizonthal distance measurments in this ROM
			std::vector<TLINE>  measDHOR;
			/// All Ecarte-Theodolite measurments in this ROM
			std::vector<TECTH>  measECTH;

			T3ObsSummary getPLR3DObsSummary()const;
			T2ObsSummary getDIR3DObsSummary()const;


			/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
			TLGCObsSummary getANGLObsSummary() const;
			/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
			TLGCObsSummary getZENDObsSummary() const;
			/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
			TLGCObsSummary getDISTObsSummary() const;
			/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
			TLGCObsSummary getDHORObsSummary() const;

			/// Each ROM has a default target that is inherited to the measurements
			TROM(const TInstrumentData::TPOLAR::TTarget& defTarget, TAdjustableAngle* v0):
				defaultTarget(&defTarget), v0(v0), acst(LGC::TAngle::EUnits::kGons, 0.0){}
		};


		/// The instrument that is used on this station
		TInstrumentData::TPOLAR     instrument;

		/// The point on which the instrument resides
		const TAdjustablePoint* instrumentPos; 

		/// Adjustable instrument height, contains information if the instrument height is fixed for this station
		const TAdjustableScalar* instrumentHeightAdjustable;

		/// Tells whether the instrument is be able to rotate freely in all three dimensions
		bool rot3D;

		//The 2 rotations of the station. These angles are used when rot3D option is used. Rotation around Z-axis is made by V0 angle defined in ROM.
		const TAdjustableAngle* rotX;
		const TAdjustableAngle* rotY;

		/// Line of the station definition
		int  line;

		/// All round of measurements for this instrument. Each ROM takes place on a certain point
		std::vector<TROM> roms;

		/*!
			Creates a total station that is centered on a given point with instrument data from the input file

			\param pos A reference to the point under the station. Must be valid throughout the program.

			\param instrument A reference to the instrument data. The object is stored as a deep copy.
		*/
		TTSTN( const TAdjustablePoint& pos, const TInstrumentData::TPOLAR& instrument) :
			instrument(instrument),
			instrumentPos(&pos),
			rot3D(false),
			line(NO_VALi),
			instrumentHeightAdjustable(nullptr),
			rotX(nullptr),
			rotY(nullptr)
			{}
	};

#endif