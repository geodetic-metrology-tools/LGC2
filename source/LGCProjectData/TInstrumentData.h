#ifndef _INSTRUMENTDATA_H_
#define _INSTRUMENTDATA_H_

#include <map>
#include "Global.h"
#include "TAdjustableScalar.h"
#include "TAdjustableAngle.h"

/*!
	\ingroup LGCProjectData

	Stores the instrument data that is given in the input file. 
	This data is then used to create proper instrument objects (TSTN, CAM, TLEVEL or TEDM).
	Values given in the input file can be in various units, but they are transformed during reading into metres[m] (lengths) or radians[rads] (angles), in whose they are stored.
*/
class TInstrumentData {
	public:
		struct TPOLAR {
			std::string ID;
			std::string defTarget;
			TReal instrHeight; // meters [m]
			TReal sigmaInstrHeight; // [m]
			TReal sigmaInstrCentering; // [m]
			TReal constAngle; // [rad]
						
			struct TTarget {
				std::string ID;
				TReal sigmaAngl;	// [rad]
				TReal sigmaZenD;	// [rad]
				TReal sigmaDist;	// [m]
				TReal ppmDist;		// [m]
				bool  distCorrectionUnknown;
				TReal distCorrectionValue; //[m]
				TReal sigmaDCorr;   // [m]
				TReal sigmaTargetCentering; //[m]
				TReal targetHt;  // [m]
				TReal sigmaTargetHt; // [m]
				TAdjustableScalar* distCorrectionAdjustable; // Adjustable object for "distCorrectionValue"
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
				TReal sigmaX;	// [] unitless
				TReal sigmaY;	// [] unitless
				TReal sigmaDist;	// [m]
				TReal sigmaTargetCentering; //[m]
			};

			/// allows the lookup of targets for this total station based on the target ID.
			std::map<const std::string, TTarget> targets;
		};

		struct TEDM {
			std::string ID;
			std::string defTarget;
			TReal instrHeight; //[m]
			TReal sigmaInstrHeight; // [m]
			TReal sigmaInstrCentering; // [m]

			struct TTarget {
				std::string ID;
				TReal sigmaDSpt; //[m] sigma of the distance
				TReal ppmDSpt; //[m]
				bool  distCorrectionUnknown;
				TReal distCorrectionValue; //[m]
				TReal sigmaDCorr; //[m]
				TReal sigmaTargetCentering; //[m]
				TReal targetHt; //[m]
				TReal sigmaTargetHt; //[m]
				TAdjustableScalar* distCorrectionAdjustable;
			};
			
			/// allows the lookup of targets for this distance meter based on the target ID.
			std::map<const std::string, TTarget> targets;
		};

		struct TLEVEL {
			std::string ID;
			std::string defStaffID;
			bool collAngleUnknown;
			TReal collAngleValue; //[rad]
			TAdjustableAngle* collAngleAdjustable;

			struct TTarget {
				std::string ID;
				TReal sigmaD; //[m]
				TReal ppmD; //[m]
				TReal distCorrectionValue; //[m]
				TReal sigmaDCorr; //[m]
				TReal staffHt;  // i.e vertical offset of the staff = staff height [m]
				TReal sigmaStaffHt; // standard deviation of the vertical offset of the staff [m]
			};
			
			/// allows the lookup of targets for this leveling station based on the target ID.
			std::map<const std::string, TTarget> targets;
		};

		struct TSCALE {
			std::string ID;
			TReal sigmaD; // [m]
			TReal ppmD; // [m]
			TReal distCorrectionValue; // [m]
			TReal sigmaDCorr; // [m]
			TReal sigmaInstrCentering; // [m]
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
