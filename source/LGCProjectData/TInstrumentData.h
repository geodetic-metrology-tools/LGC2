#ifndef _INSTRUMENTDATA_H_
#define _INSTRUMENTDATA_H_

#include <map>
#include "Global.h"
#include "TAdjustableLength.h"
#include "TAdjustableAngle.h"

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
			TPOLAR(){}
			TPOLAR(const TPOLAR& par)
			{
				*this = par;
			}

			TPOLAR& operator=(const TPOLAR& par)
			{
				ID = par.ID;
				defTarget = par.defTarget;
				instrHeight = par.instrHeight;
				sigmaInstrHeight = par.sigmaInstrHeight;
				sigmaInstrCentering = par.sigmaInstrCentering;
				constAngle = par.constAngle;
				for(auto const& target : par.targets)
					targets.emplace(target);

				return *this;
			}

			std::string ID;
			std::string defTarget;
			TLength instrHeight;          // [m]
			TLength sigmaInstrHeight;     // [m]
			TLength sigmaInstrCentering;  // [m]
			TAngle constAngle;            // [rad]

					
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
			};

			/// allows the lookup of targets for this total station based on the target ID.
			std::map<const std::string, TTarget> targets;
		};

		//Definition of camera parameters
		struct TCAMD {
			std::string ID;
			std::string defTarget;
						
			struct TTarget {
				std::string ID;
				TReal sigmaX;	               // [] unitless
				TReal sigmaY;	               // [] unitless
				TLength sigmaDist;	         // [m]
				TLength sigmaTargetCentering; // [m]
			};

			/// allows the lookup of targets for this total station based on the target ID.
			std::map<const std::string, TTarget> targets;
		};

		struct TEDM {
			std::string ID;
			std::string defTarget;
			TLength instrHeight;         // [m]
			TLength sigmaInstrHeight;    // [m]
			TLength sigmaInstrCentering; // [m]

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
			};
			
			/// allows the lookup of targets for this distance meter based on the target ID.
			std::map<const std::string, TTarget> targets;
		};

		struct TLEVEL {
			std::string ID;
			std::string defStaffID;
			bool collAngleUnknown;
			TAngle collAngleValue;           // [rad]
			TAdjustableAngle* collAngleAdjustable;

			struct TTarget {
			std::string ID;
            TLength sigmaD;               // [m]
            TLength ppmD;                 // [m]
            TLength distCorrectionValue;  // [m]
            TLength sigmaDCorr;           // [m]
            TLength staffHt;              // [m] i.e vertical offset of the staff = staff height 
            TLength sigmaStaffHt;         // [m] standard deviation of the vertical offset of the staff
			};
			
			/// allows the lookup of targets for this leveling station based on the target ID.
			std::map<const std::string, TTarget> targets;
		};

		struct TSCALE {
		 std::string ID;
         TLength sigmaD;               // [m]
         TLength ppmD;                 // [m]
         TLength distCorrectionValue;  // [m]
         TLength sigmaDCorr;           // [m]
         TLength sigmaInstrCentering;  // [m]
		};
		
		/// All available polar instruments, accessible by their ID. See \ref getDevice for failsave lookup.
		std::map<const std::string, TPOLAR> fPOLAR;
		/// All available camera instruments, accessible by their ID. See \ref getDevice for failsave lookup.
		std::map<const std::string, TCAMD> fCAMD;
		/// All available distance meters, accessible by their ID. See \ref getDevice for failsave lookup.
		std::map<const std::string, TEDM>   fEDM;
		/// All available leveling instruments, accessible by their ID. See \ref getDevice for failsave lookup.
		std::map<const std::string, TLEVEL> fLEVEL;
		/// All available scale devices, accessible by their ID. See \ref getDevice for failsave lookup.
		std::map<const std::string, TSCALE> fSCALE;

		/*!
			Get a (const) reference to an instrument by passing the desired map and an instrument ID.
			This failsafe lookup function throws if the desired instrument is not found.
			
			\param m The map in wich to look, e.g. \ref fPOLAR. 
			\param id The desired instrument ID.
			\returns A reference to a const instrument object to read the values.
			\throws Throws a runtime_error if the instrument is not found
		*/
		template<typename T>
		const T& getDevice(const std::map<const std::string, T>& m, const std::string& id) const {
			auto it(m.find(id));
			if (it == m.end())
				throw std::runtime_error("Could not find device " + id + ".");

			return it->second;
		}
};
#endif
