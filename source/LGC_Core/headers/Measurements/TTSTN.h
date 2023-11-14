/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _TTSTN_H_
#define _TTSTN_H_

//SURVEYLIB
#include <TAdjustableAngle.h>
//LGC
#include <Global.h>
#include <TInstrumentData.h>
#include <MeasDef.h>
#include "TLGCObsSummary.h"
#include <LGCAdjustablePoint.h>

/*!
	\ingroup Measurements
	\brief The Total Station (TSTN) is a TPOLAR instrument positioned on a point, which can take measurements of different types.

	TROM = round of measurments groups the particular measurements, all of them have common: constant orientation acst and calculated orientation angle v0.
*/
struct TTSTN : public TStatusObject {
		/*!
			\ingroup Measurements
			\brief (ROM = a round of total station measurements) groups all the measurements which are made in one round.

			Every ROM can has specific constant orientation and specific V0 to be adjusted.
			The ROM keeps the measurements which can be made by the TSTN in vectors.
		*/
		struct TROM : public TStatusObject {

			TAngle acst; ///< constant angle/orientation of the station, defaults to zero.

			TAdjustableAngle* v0; ///< V0 angle/orientation, defaults to zero, to be calculated during the LS calculation, always variable.

			std::string defaultTargetId;  ///< Default target id for this round of measurements.
			
            int romId{ romCounter_++ };

			/// All PLR3D measurements in this ROM
			std::list<TPLR3D> measPLR3D;
			/// All horizonthal angle measurements in this ROM
			std::list<TANGL>  measANGL;
			/// All zenithal distance measurments in this ROM
			std::list<TZEND>  measZEND;
			/// All spatial distane measurments in this ROM
			std::list<TLINE>  measDIST;
			/// All horizonthal distance measurments in this ROM
			std::list<TLINE>  measDHOR;
			/// All Ecarte-Theodolite plan measurements in this ROM
			std::list<TECTH>  measECTH;
			/// All Ecarte-Theodolite line measurements in this ROM
			std::list<TECDIR>  measECDIR;

            bool plrActive{ true }; ///< activation status of the PLR3D rom
            bool anglActive{ true }; ///< activation status of the ANGL rom
            bool zendActive{ true }; ///< activation status of the ZEND rom
            bool distActive{ true }; ///< activation status of the DIST rom
            bool dhorActive{ true }; ///< activation status of the DHOR rom
            bool ecthActive{ true }; ///< activation status of the ECTH rom
            bool ecdirActive{ true }; ///< activation status of the ECDIR rom

            //! Initialise the observation summaries
            void initialiseObsSummaries();

			/*!
				\returns Summary of all the PLR3D measurements.
				\note This function can be called only when the calculation is finished and the residuals of the observations are already set.
			*/
			const TPOLARObsSummary& getPLR3DObsSummary() const;
			const TPOLARObsSummary& getPLR3DObsSummary(std::string text) noexcept;

			/*!
				\returns Summary of all the ANGL measurements.
				\note This function can be called only when the calculation is finished and the residuals of the observations are already set.
			*/
            const TLGCObsSummary& getANGLObsSummary() const;
			const TLGCObsSummary& getANGLObsSummary(std::string text) noexcept;

			/*!
				\returns Summary of all the ZEND measurements.
				\note This function can be called only when the calculation is finished and the residuals of the observations are already set.
			*/
            const TLGCObsSummary& getZENDObsSummary() const;
			const TLGCObsSummary& getZENDObsSummary(std::string text) noexcept;

			/*!
				\returns Summary of all the DIST measurements.
				\note This function can be called only when the calculation is finished and the residuals of the observations are already set.
			*/
            const TLGCObsSummary& getDISTObsSummary() const;
			const TLGCObsSummary& getDISTObsSummary(std::string text) noexcept;

			/*!
				\returns Summary of all the DHOR measurements.
				\note This function can be called only when the calculation is finished and the residuals of the observations are already set.
			*/
            const TLGCObsSummary& getDHORObsSummary() const;
			const TLGCObsSummary& getDHORObsSummary(std::string text) noexcept;

			/*!
				\returns Summary of all the ECTH measurements.
				\note This function can be called only when the calculation is finished and the residuals of the observations are already set.
			*/
            const TLGCObsSummary& getECTHObsSummary() const;
			const TLGCObsSummary& getECTHObsSummary(std::string text) noexcept;

			/*!
			\returns Summary of all the ECSP measurements.
			\note This function can be called only when the calculation is finished and the residuals of the observations are already set.
			*/
            const TLGCObsSummary& getECDIRObsSummary() const;
			const TLGCObsSummary& getECDIRObsSummary(std::string text) noexcept;

			/// Each ROM has a default target that is inherited to the measurements
			TROM(std::string defTargetId, TAdjustableAngle* v0)
                : acst(0.0, TAngle::EUnits::kGons)
                , v0(v0)
                , defaultTargetId(defTargetId) {}

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(ObjectSerializer &obj) const override;
#endif

        private:

            static int romCounter_;

            TPOLARObsSummary plr3dSummary_;
            TLGCObsSummary anglSummary_;
            TLGCObsSummary zendSummary_;
            TLGCObsSummary distSummary_;
            TLGCObsSummary dhorSummary_;
            TLGCObsSummary ecthSummary_;
            TLGCObsSummary ecdirSummary_;

		};


		/// The instrument that is used on this station
		TInstrumentData::TPOLAR instrument;

		/// The point on which the instrument resides
		const LGCAdjustablePoint* instrumentPos; 

		/// Adjustable instrument height, the height can be fixed or variable
		const TAdjustableLength* instrumentHeightAdjustable;

		/// Tells whether the instrument is be able to rotate freely in all three dimensions
        bool rot3D{ false };

        /// Tells whether the instrument height is fixed or not
        bool ihfix{ false };

		///The optional rotation angle about the X axis.
		const TAdjustableAngle* rotX;
		///The optional rotation angle about the Y axis.
		const TAdjustableAngle* rotY;

		/// Line of the station definition
		int  line;

		/// All round of measurements for this instrument. In each ROM a point on different position is measured.
		std::list<std::shared_ptr<TROM>> roms;

        int stnId{ stnCounter_++ };

		/*!
			Creates a total station that is centered on a given point with instrument data from the input file

			\param pos A reference to the point under the station. Must be valid throughout the program.

			\param instrument A reference to the instrument data. The object is stored as a deep copy.
		*/
		TTSTN( const LGCAdjustablePoint& pos, const TInstrumentData::TPOLAR& instrument) :
			instrument(instrument),
			instrumentPos(&pos),
			rot3D(false),
            ihfix(false),
			line(NO_VALi),
			instrumentHeightAdjustable(nullptr),
			rotX(nullptr),
			rotY(nullptr)

		{}

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override;
#endif

    private:

        static int stnCounter_;

	};

#endif