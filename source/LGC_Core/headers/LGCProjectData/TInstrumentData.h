/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _INSTRUMENTDATA_H_
#define _INSTRUMENTDATA_H_


//STL
#include <map>
#include <Global.h>
//SURVEYLIB
#include <TAdjustableLength.h>
#include <TAdjustableAngle.h>
/*!
	\ingroup LGCProjectData

	Stores the instrument data that is given in the input file. 
	This data is then used to create proper instrument objects (TSTN, CAM, TLEVEL or TEDM).
	Values given in the input file can be in various units, but they are transformed during reading into metres[m] (lengths) or radians[rads] (angles), in whose they are stored.
*/
class TInstrumentData {
	public:
		class TPOLAR {
		public:
            struct TTarget {

                std::string ID;
                TAngle sigmaAngl;	            // [rad]
                TAngle sigmaZenD;	            // [rad]
                TLength sigmaDist;	         // [m]
                TLength ppmDist;		         // [m]
                bool  distCorrectionUnknown;
                TLength distCorrectionValue;  // [m]
                TLength sigmaDCorr;           // [m]
                TLength sigmaTargetCentering; // [m]
                TLength targetHt;             // [m]
                TLength sigmaTargetHt;        // [m]
                TAdjustableLength* distCorrectionAdjustable; // Adjustable object for "distCorrectionValue"
				TLength sigmaCombinedMetre;		   // [m]
				TAngle sigmaCombinedRad;		   // [rad]
				TAngle sigmaCombinedPLRAngl;		   // [rad]
				TAngle sigmaCombinedPLRZenD;		   // [rad]
				TLength sigmaCombinedPLRDist;		   // [m]
            };

            TPOLAR() = default;
            TPOLAR(
                const std::string &instr_id,
                const std::string &defTgt,
                const TLength &instrH,
                const TLength &sigmaInstrH,
                const TLength &sigmaInstrCent,
                const TAngle &cstAngle)
                : ID(instr_id)
                , defTarget(defTgt)
                , instrHeight(instrH)
                , sigmaInstrHeight(sigmaInstrH)
                , sigmaInstrCentering(sigmaInstrCent)
                , constAngle(cstAngle) {}
            
            TPOLAR(const TPOLAR& instr)
			{
                *this = instr; // Use the assignment operator to have the targets deep copied
			}
            TPOLAR& operator=(const TPOLAR &other){
                if(this == &other) return *this;

                ID = other.ID;
                defTarget = other.defTarget;
                instrHeight = other.instrHeight;
                sigmaInstrHeight = other.sigmaInstrHeight;
                sigmaInstrCentering = other.sigmaInstrCentering;
                constAngle = other.constAngle;
                targets = other.targets;

                for(auto &tgt : targets)
                    // Replace the target in the memory
                    tgt.second.reset(new TTarget(*tgt.second));

                return *this;
            }

			std::string ID;
			std::string defTarget;
			TLength instrHeight;          // [m]
			TLength sigmaInstrHeight;     // [m]
			TLength sigmaInstrCentering;  // [m]
			TAngle constAngle;            // [rad]
            std::map<std::string, std::shared_ptr<TTarget>> targets; /// allows the lookup of targets for this instrument based on the target ID.
		};

		//Definition of camera parameters
		struct TCAMD {
            struct TTarget {
                std::string ID;
                TReal sigmaX;	               // [] unitless
                TReal sigmaY;	               // [] unitless
                TLength sigmaDist;	         // [m]
                TLength sigmaTargetCentering; // [m]
				TLength sigmaCombinedX;		   // [m]
				TLength sigmaCombinedY;		   // [m]
				TLength sigmaCombinedDist;		   // [m]
            };

            TCAMD() = default;
            TCAMD(
                const std::string &instr_id,
                const std::string &defTgt,
                const TLength &sigmaInstrCent)
                : ID(instr_id)
                , defTarget(defTgt)
                , sigmaInstrCentering(sigmaInstrCent) {}

            TCAMD(const TCAMD& instr)
            {
                *this = instr; // Use the assignment operator to have the targets deep copied
            }

            TCAMD& operator=(const TCAMD &other){
                if(this == &other) return *this;

                ID = other.ID;
                defTarget = other.defTarget;
                sigmaInstrCentering = other.sigmaInstrCentering;
                targets = other.targets;

                for(auto &tgt : targets)
                    // Replace the target in the memory
                    tgt.second.reset(new TTarget(*tgt.second));

                return *this;
            }

			std::string ID;
			std::string defTarget;
			TLength sigmaInstrCentering; // [m]
            std::map<std::string, std::shared_ptr<TTarget>> targets; /// allows the lookup of targets for this instrument based on the target ID.
		};

		struct TEDM {
            struct TTarget {
                std::string ID;
                TLength sigmaDSpt;            // [m] sigma of the distance
                TLength ppmDSpt;              // [m]
                bool  distCorrectionUnknown;
                TLength distCorrectionValue;  // [m]
                TLength sigmaDCorr;           // [m]
                TLength sigmaTargetCentering; // [m]
                TLength targetHt;             // [m]
                TLength sigmaTargetHt;        // [m]
                TAdjustableLength* distCorrectionAdjustable;
				TLength sigmaCombined;		   // [m]
            };

            TEDM() = default;
            TEDM(
                const std::string &instr_id,
                const std::string &defTgt,
                const TLength &instrH,
                const TLength &sigmaInstrH,
                const TLength &sigmaInstrCent)
                : ID(instr_id)
                , defTarget(defTgt)
                , instrHeight(instrH)
                , sigmaInstrHeight(sigmaInstrH)
                , sigmaInstrCentering(sigmaInstrCent) {}

            TEDM(const TEDM& instr)
            {
                *this = instr; // Use the assignment operator to have the targets deep copied
            }

            TEDM& operator=(const TEDM &other){
                if(this == &other) return *this;

                ID = other.ID;
                defTarget = other.defTarget;
                instrHeight = other.instrHeight;
                sigmaInstrHeight = other.sigmaInstrHeight;
                sigmaInstrCentering = other.sigmaInstrCentering;
                targets = other.targets;

                for(auto &tgt : targets)
                    // Replace the target in the memory
                    tgt.second.reset(new TTarget(*tgt.second));

                return *this;
            }

			std::string ID;
			std::string defTarget;
			TLength instrHeight;         // [m]
			TLength sigmaInstrHeight;    // [m]
			TLength sigmaInstrCentering; // [m]
            std::map<std::string, std::shared_ptr<TTarget>> targets; /// allows the lookup of targets for this instrument based on the target ID.
		};

		struct TLEVEL {
            struct TTarget {
                std::string ID;
                TLength sigmaD;               // [m]
                TLength ppmD;                 // [m]
                TLength distCorrectionValue;  // [m]
                TLength sigmaDCorr;           // [m]
                TLength staffHt;              // [m] i.e vertical offset of the staff = staff height 
                TLength sigmaStaffHt;         // [m] standard deviation of the vertical offset of the staff
				TLength sigmaCombined;
            };

            TLEVEL() = default;
            TLEVEL(
                const std::string &instr_id,
                const std::string &defStaff,
                const bool &collAnglUnknown,
                const TAngle &collAnglVal,
                TAdjustableAngle* collAnglAdj)
                : ID(instr_id)
                , defStaffID(defStaff)
                , collAngleUnknown(collAnglUnknown)
                , collAngleValue(collAnglVal)
                , collAngleAdjustable(collAnglAdj) {}

            TLEVEL(const TLEVEL& instr)
            {
                *this = instr; // Use the assignment operator to have the targets deep copied
            }

            TLEVEL& operator=(const TLEVEL &other){
                if(this == &other) return *this;

                ID = other.ID;
                defStaffID = other.defStaffID;
                collAngleUnknown = other.collAngleUnknown;
                collAngleValue = other.collAngleValue;
                collAngleAdjustable = other.collAngleAdjustable;
                targets = other.targets;

                for(auto &tgt : targets)
                    // Replace the target in the memory
                    tgt.second.reset(new TTarget(*tgt.second));

                return *this;
            }

			std::string ID;
			std::string defStaffID;
			bool collAngleUnknown;
			TAngle collAngleValue;           // [rad]
			TAdjustableAngle* collAngleAdjustable;
            std::map<std::string, std::shared_ptr<TTarget>> targets; /// allows the lookup of targets for this instrument based on the target ID.
		};

		struct TSCALE {
		 std::string ID;
         TLength sigmaD;               // [m]
         TLength ppmD;                 // [m]
         TLength distCorrectionValue;  // [m]
         TLength sigmaDCorr;           // [m]
         TLength sigmaInstrCentering;  // [m]
		 TLength sigmaCombined;		   // [m]
		};

		struct TINCL {
			std::string ID;
			TAngle sigmaAngl;	          // [rad]
			TAngle angleCorrectionValue;  // [rad]
			TAngle sigmaCorrectionValue;  // [rad]
			TAngle refAngleCorrectionValue;  // [rad]
			TAngle refSigmaCorrectionValue;  // [rad]
			TAngle sigmaCombined;		  // [rad]
		};
		
		/// All available polar instruments, accessible by their ID. See \ref getDevice for failsave lookup.
        std::map<std::string, std::shared_ptr<TPOLAR>> fPOLAR;
		/// All available camera instruments, accessible by their ID. See \ref getDevice for failsave lookup.
        std::map<std::string, std::shared_ptr<TCAMD>> fCAMD;
		/// All available distance meters, accessible by their ID. See \ref getDevice for failsave lookup.
        std::map<std::string, std::shared_ptr<TEDM>>   fEDM;
		/// All available leveling instruments, accessible by their ID. See \ref getDevice for failsave lookup.
        std::map<std::string, std::shared_ptr<TLEVEL>> fLEVEL;
		/// All available scale devices, accessible by their ID. See \ref getDevice for failsave lookup.
        std::map<std::string, std::shared_ptr<TSCALE>> fSCALE;
		/// All available scale devices, accessible by their ID. See \ref getDevice for failsave lookup.
		std::map<std::string, std::shared_ptr<TINCL>> fINCL;

		/*!
			Get a (const) reference to an instrument by passing the desired map and an instrument ID.
			This failsafe lookup function throws if the desired instrument is not found.
			
			\param m The map in wich to look, e.g. \ref fPOLAR. 
			\param id The desired instrument ID.
			\returns A reference to a const instrument object to read the values.
			\throws Throws a runtime_error if the instrument is not found
		*/
		template<typename T>
        const T& getDevice(const std::map<std::string, std::shared_ptr<T>>& m, const std::string& id) const {
			auto it(m.find(id));
			if (it == m.end())
				throw std::runtime_error("Could not find device " + id + ".");

			return *it->second;
		}
};
#endif
