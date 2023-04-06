/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
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

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER


/*!
	\ingroup LGCProjectData

	Stores the instrument data that is given in the input file. 
	This data is then used to create proper instrument objects (TSTN, CAM, TLEVEL or TEDM).
	Values given in the input file can be in various units, but they are transformed during reading into metres[m] (lengths) or radians[rads] (angles), in whose they are stored.
*/
#if USE_SERIALIZER
class TInstrumentData : public Serializable
#else
class TInstrumentData
#endif // USE_SERIALIZER
{
	public:
#if USE_SERIALIZER
		class TPOLAR : public Serializable
#else
		class TPOLAR
#endif // USE_SERIALIZER
	{
		public:
#if USE_SERIALIZER
			struct TTarget : public Serializable
#else
			struct TTarget
#endif // USE_SERIALIZER
		    {
				TTarget(std::string ID = "",
				    TAngle sigmaAngl = TAngle(),
				    TAngle sigmaZenD = TAngle(),
				    TLength sigmaDist = TLength(),
				    TLength ppmDist = TLength(),
				    bool distCorrectionUnknown = false,
				    TLength distCorrectionValue = TLength(),
				    TLength sigmaDCorr = TLength(),
				    TLength sigmaTargetCentering = TLength(),
				    TLength targetHt = TLength(),
				    TLength sigmaTargetHt = TLength(),
				    TAdjustableLength *distCorrectionAdjustable = nullptr,
				    TLength sigmaCombinedDist = TLength(),
				    TAngle sigmaCombinedAngle = TAngle(),
				    TAngle sigmaCombinedPLRAngl = TAngle(),
				    TAngle sigmaCombinedPLRZenD = TAngle(),
				    TLength sigmaCombinedPLRDist = TLength()) :
				    ID(ID),
				    sigmaAngl(sigmaAngl),
				    sigmaZenD(sigmaZenD),
				    sigmaDist(sigmaDist),
				    ppmDist(ppmDist),
				    distCorrectionUnknown(distCorrectionUnknown),
				    distCorrectionValue(distCorrectionValue),
				    sigmaDCorr(sigmaDCorr),
				    sigmaTargetCentering(sigmaTargetCentering),
				    targetHt(targetHt),
				    sigmaTargetHt(sigmaTargetHt),
				    distCorrectionAdjustable(distCorrectionAdjustable),
				    sigmaCombinedDist(sigmaCombinedDist),
				    sigmaCombinedAngle(sigmaCombinedAngle),
				    sigmaCombinedPLRAngl(sigmaCombinedPLRAngl),
				    sigmaCombinedPLRZenD(sigmaCombinedPLRZenD),
				    sigmaCombinedPLRDist(sigmaCombinedPLRDist){};
					
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
				TLength sigmaCombinedDist;		   // [m]
				TAngle sigmaCombinedAngle;		   // [rad]
				TAngle sigmaCombinedPLRAngl;		   // [rad]
				TAngle sigmaCombinedPLRZenD;		   // [rad]
				TLength sigmaCombinedPLRDist;		   // [m]
            
#if USE_SERIALIZER
				// Inherited via Serializable
				virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER
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

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER

			std::string ID;
			std::string defTarget;
			TLength instrHeight;          // [m]
			TLength sigmaInstrHeight;     // [m]
			TLength sigmaInstrCentering;  // [m]
			TAngle constAngle;            // [rad]
            std::map<std::string, std::shared_ptr<TTarget>> targets; /// allows the lookup of targets for this instrument based on the target ID.
		};

		//Definition of camera parameters
#if USE_SERIALIZER
		struct TCAMD : public Serializable
#else
		struct TCAMD
#endif // USE_SERIALIZER
		{
#if USE_SERIALIZER
			struct TTarget : public Serializable
#else
			struct TTarget
#endif // USE_SERIALIZER
            {
				TTarget(std::string ID = "",
					TReal sigmaX = 0,
					TReal sigmaY = 0,
					TLength sigmaDist = TLength(),
					TLength sigmaTargetCentering = TLength(),
					TLength sigmaCombinedX = TLength(),
					TLength sigmaCombinedY = TLength(),
					TLength sigmaCombinedDist = TLength()) :
					ID(ID),
					sigmaX(sigmaX),
					sigmaY(sigmaY),
					sigmaDist(sigmaDist),
					sigmaTargetCentering(sigmaTargetCentering),
					sigmaCombinedX(sigmaCombinedX),
					sigmaCombinedY(sigmaCombinedY),
					sigmaCombinedDist(sigmaCombinedDist){};

                std::string ID;
                TReal sigmaX;	               // [] unitless
                TReal sigmaY;	               // [] unitless
                TLength sigmaDist;	         // [m]
                TLength sigmaTargetCentering; // [m]
				TLength sigmaCombinedX;		   // [m]
				TLength sigmaCombinedY;		   // [m]
				TLength sigmaCombinedDist;		   // [m]

#if USE_SERIALIZER
				// Inherited via Serializable
				virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER
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

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER

			std::string ID;
			std::string defTarget;
			TLength sigmaInstrCentering; // [m]
            std::map<std::string, std::shared_ptr<TTarget>> targets; /// allows the lookup of targets for this instrument based on the target ID.
		};

#if USE_SERIALIZER
		struct TEDM : public Serializable
#else
		struct TEDM
#endif // USE_SERIALIZER
		{
#if USE_SERIALIZER
			struct TTarget : public Serializable
#else
			struct TTarget
#endif // USE_SERIALIZER
			{
				TTarget(std::string ID = "",
					TLength sigmaDSpt = TLength(),
					TLength ppmDSpt = TLength(),
					bool distCorrectionUnknown = false,
					TLength distCorrectionValue = TLength(),
					TLength sigmaDCorr = TLength(),
					TLength sigmaTargetCentering = TLength(),
					TLength targetHt = TLength(),
					TLength sigmaTargetHt = TLength(),
					TAdjustableLength *distCorrectionAdjustable = nullptr,
					TLength sigmaCombinedDist = TLength()) :
					ID(ID),
					sigmaDSpt(sigmaDSpt),
					ppmDSpt(ppmDSpt),
					distCorrectionUnknown(distCorrectionUnknown),
					distCorrectionValue(distCorrectionValue),
					sigmaDCorr(sigmaDCorr),
					sigmaTargetCentering(sigmaTargetCentering),
					targetHt(targetHt),
					sigmaTargetHt(sigmaTargetHt),
					distCorrectionAdjustable(distCorrectionAdjustable),
					sigmaCombinedDist(sigmaCombinedDist){};

				std::string ID;
				TLength sigmaDSpt; // [m] sigma of the distance
				TLength ppmDSpt; // [m]
				bool distCorrectionUnknown;
				TLength distCorrectionValue; // [m]
				TLength sigmaDCorr; // [m]
				TLength sigmaTargetCentering; // [m]
				TLength targetHt; // [m]
				TLength sigmaTargetHt; // [m]
				TAdjustableLength *distCorrectionAdjustable;
				TLength sigmaCombinedDist; // [m]

#if USE_SERIALIZER
				// Inherited via Serializable
				virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER
			};

			TEDM() = default;
			TEDM(const std::string &instr_id, const std::string &defTgt, const TLength &instrH, const TLength &sigmaInstrH, const TLength &sigmaInstrCent) :
				ID(instr_id), defTarget(defTgt), instrHeight(instrH), sigmaInstrHeight(sigmaInstrH), sigmaInstrCentering(sigmaInstrCent)
			{
			}

			TEDM(const TEDM &instr)
			{
				*this = instr; // Use the assignment operator to have the targets deep copied
			}

			TEDM &operator=(const TEDM &other)
			{
				if (this == &other)
					return *this;

				ID = other.ID;
				defTarget = other.defTarget;
				instrHeight = other.instrHeight;
				sigmaInstrHeight = other.sigmaInstrHeight;
				sigmaInstrCentering = other.sigmaInstrCentering;
				targets = other.targets;

				for (auto &tgt : targets)
					// Replace the target in the memory
					tgt.second.reset(new TTarget(*tgt.second));

				return *this;
			}

			std::string ID;
			std::string defTarget;
			TLength instrHeight; // [m]
			TLength sigmaInstrHeight; // [m]
			TLength sigmaInstrCentering; // [m]
			std::map<std::string, std::shared_ptr<TTarget>> targets; /// allows the lookup of targets for this instrument based on the target ID.

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER
		};

#if USE_SERIALIZER
		struct TLEVEL : public Serializable
#else
		struct TLEVEL
#endif // USE_SERIALIZER
		{
#if USE_SERIALIZER
			struct TTarget : public Serializable
#else
			struct TTarget
#endif // USE_SERIALIZER
			{
				TTarget(std::string ID = "",
					TLength sigmaD = TLength(),
					TLength ppmD = TLength(),
					TLength distCorrectionValue = TLength(),
					TLength sigmaDCorr = TLength(),
					TLength staffHt = TLength(),
					TLength sigmaStaffHt = TLength(),
					TLength sigmaCombinedDist = TLength()) :
					ID(ID),
					sigmaD(sigmaD),
					ppmD(ppmD),
					distCorrectionValue(distCorrectionValue),
					sigmaDCorr(sigmaDCorr),
					staffHt(staffHt),
					sigmaStaffHt(sigmaStaffHt),
					sigmaCombinedDist(sigmaCombinedDist){};

				std::string ID;
				TLength sigmaD; // [m]
				TLength ppmD; // [m]
				TLength distCorrectionValue; // [m]
				TLength sigmaDCorr; // [m]
				TLength staffHt; // [m] i.e vertical offset of the staff = staff height
				TLength sigmaStaffHt; // [m] standard deviation of the vertical offset of the staff
				TLength sigmaCombinedDist;

#if USE_SERIALIZER
				// Inherited via Serializable
				virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER
			};

			TLEVEL() = default;
			TLEVEL(const std::string &instr_id, const std::string &defStaff, const bool &collAnglUnknown, const TAngle &collAnglVal, TAdjustableAngle *collAnglAdj) :
				ID(instr_id), defStaffID(defStaff), collAngleUnknown(collAnglUnknown), collAngleValue(collAnglVal), collAngleAdjustable(collAnglAdj)
			{
			}

			TLEVEL(const TLEVEL &instr)
			{
				*this = instr; // Use the assignment operator to have the targets deep copied
			}

			TLEVEL &operator=(const TLEVEL &other)
			{
				if (this == &other)
					return *this;

				ID = other.ID;
				defStaffID = other.defStaffID;
				collAngleUnknown = other.collAngleUnknown;
				collAngleValue = other.collAngleValue;
				collAngleAdjustable = other.collAngleAdjustable;
				targets = other.targets;

				for (auto &tgt : targets)
					// Replace the target in the memory
					tgt.second.reset(new TTarget(*tgt.second));

				return *this;
			}

			std::string ID;
			std::string defStaffID;
			bool collAngleUnknown;
			TAngle collAngleValue; // [rad]
			TAdjustableAngle *collAngleAdjustable;
			std::map<std::string, std::shared_ptr<TTarget>> targets; /// allows the lookup of targets for this instrument based on the target ID.

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER
		};

#if USE_SERIALIZER
		struct TSCALE : public Serializable
#else
		struct TSCALE
#endif // USE_SERIALIZER
		{
			TSCALE(std::string ID = "",
				TLength sigmaD = TLength(),
				TLength ppmD = TLength(),
				TLength distCorrectionValue = TLength(),
				TLength sigmaDCorr = TLength(),
				TLength sigmaInstrCentering = TLength(),
				TLength sigmaCombinedDist = TLength()) :
				ID(ID),
				sigmaD(sigmaD),
				ppmD(ppmD),
				distCorrectionValue(distCorrectionValue),
				sigmaDCorr(sigmaDCorr),
				sigmaInstrCentering(sigmaInstrCentering),
				sigmaCombinedDist(sigmaCombinedDist){};

			std::string ID;
			TLength sigmaD; // [m]
			TLength ppmD; // [m]
			TLength distCorrectionValue; // [m]
			TLength sigmaDCorr; // [m]
			TLength sigmaInstrCentering; // [m]
			TLength sigmaCombinedDist; // [m]

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER
		};

#if USE_SERIALIZER
		struct TINCL : public Serializable
#else
		struct TINCL
#endif // USE_SERIALIZER
		{
			TINCL(std::string ID = "",
				TAngle sigmaAngl = TAngle(),
				TAngle angleCorrectionValue = TAngle(),
				TAngle sigmaCorrectionValue = TAngle(),
				TAngle refAngleCorrectionValue = TAngle(),
				TAngle refSigmaCorrectionValue = TAngle(),
				TAngle sigmaCombinedAngle = TAngle()) :
				ID(ID),
				sigmaAngl(sigmaAngl),
				angleCorrectionValue(angleCorrectionValue),
				sigmaCorrectionValue(sigmaCorrectionValue),
				refAngleCorrectionValue(refAngleCorrectionValue),
				refSigmaCorrectionValue(refSigmaCorrectionValue),
				sigmaCombinedAngle(sigmaCombinedAngle){};

			std::string ID;
			TAngle sigmaAngl; // [rad]
			TAngle angleCorrectionValue; // [rad]
			TAngle sigmaCorrectionValue; // [rad]
			TAngle refAngleCorrectionValue; // [rad]
			TAngle refSigmaCorrectionValue; // [rad]
			TAngle sigmaCombinedAngle; // [rad]

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER
		};

#if USE_SERIALIZER
		struct THLSR : public Serializable
#else
		struct THLSR
#endif // USE_SERIALIZER
		{
			THLSR(std::string ID = "",
				TLength sigmaDist = TLength(),
				TLength sigmaInstrHeight = TLength(),
				TLength sigmaInstrCentering = TLength(),
				TLength sigmaWS = TLength(),
				TLength sigmaCombinedDist = TLength()) :
				ID(ID), sigmaDist(sigmaDist), sigmaInstrHeight(sigmaInstrHeight), sigmaInstrCentering(sigmaInstrCentering), sigmaWS(sigmaWS), sigmaCombinedDist(sigmaCombinedDist){};

			std::string ID;
			TLength sigmaDist;
			TLength sigmaInstrHeight;
			TLength sigmaInstrCentering;
			TLength sigmaWS;
			TLength sigmaCombinedDist;

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER
		};

#if USE_SERIALIZER
		struct TWPSR : public Serializable
#else
		struct TWPSR
#endif // USE_SERIALIZER
		{
			TWPSR(std::string ID = "",
			TLength sigmaX = TLength(),
			TLength sigmaZ = TLength(),
			TLength sigmaInstrCenteringX= TLength(),
			TLength sigmaInstrCenteringZ= TLength(),
			TLength sagWire= TLength(),
			TLength sigmaSagWire= TLength(),
			TLength sigmaWire= TLength(),
			TLength sigmaCombinedX= TLength(),
			TLength sigmaCombinedZ= TLength()):
				ID(ID),
				sigmaX(sigmaX),
				sigmaZ(sigmaZ),
				sigmaInstrCenteringX(sigmaInstrCenteringX),
				sigmaInstrCenteringZ(sigmaInstrCenteringZ),
				sagWire(sagWire),
				sigmaSagWire(sigmaSagWire),
				sigmaWire(sigmaWire),
				sigmaCombinedX(sigmaCombinedX),
				sigmaCombinedZ(sigmaCombinedZ)
			{};

			std::string ID;
			TLength sigmaX; // [m]
			TLength sigmaZ; // [m]
			TLength sigmaInstrCenteringX; // [m]
			TLength sigmaInstrCenteringZ; // [m]
			TLength sagWire; // [m]
			TLength sigmaSagWire; // [m]
			TLength sigmaWire; // [m]
			TLength sigmaCombinedX; // [m]
			TLength sigmaCombinedZ; // [m]

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER
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
		/// All available hls instruments, accessible by their ID. See \ref getDevice for failsave lookup.
		std::map<std::string, std::shared_ptr<THLSR>> fHLSR;
		/// All available wps instruments, accessible by their ID. See \ref getDevice for failsave lookup.
		std::map<std::string, std::shared_ptr<TWPSR>> fWPSR;

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

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER
};


#if USE_SERIALIZER
    inline void TInstrumentData::serialize(SerializerObject::SerializationHelper &obj) const
    {
		if (!fCAMD.empty())
			obj.addProperty("fCAMD", fCAMD);
		if (!fEDM.empty())
			obj.addProperty("fEDM", fEDM);
		if (!fHLSR.empty())
			obj.addProperty("fHLSR", fHLSR);
		if (!fINCL.empty())
			obj.addProperty("fINCL", fINCL);
		if (!fLEVEL.empty())
			obj.addProperty("fLEVEL", fLEVEL);
		if (!fPOLAR.empty())
			obj.addProperty("fPOLAR", fPOLAR);		
		if (!fSCALE.empty())
			obj.addProperty("fSCALE", fSCALE);
		if (!fWPSR.empty())
			obj.addProperty("fWPSR", fWPSR);
    }

	inline void TInstrumentData::TPOLAR::serialize(SerializerObject::SerializationHelper &obj) const
	{
		obj.addProperty("constAngle", constAngle.getRadiansValue());
		obj.addProperty("defTarget", defTarget);
		obj.addProperty("ID", ID);
		obj.addProperty("instrHeight", instrHeight.getMetresValue());
		obj.addProperty("sigmaInstrCentering", sigmaInstrCentering.getMetresValue());
		obj.addProperty("sigmaInstrHeight", sigmaInstrHeight.getMetresValue());
		obj.addProperty("targets", targets);
	}

	
	inline void TInstrumentData::TPOLAR::TTarget::serialize(SerializerObject::SerializationHelper &obj) const
	{
		obj.addProperty("distCorrectionAdjustable", distCorrectionAdjustable);
		obj.addProperty("distCorrectionUnknown", distCorrectionUnknown);
		obj.addProperty("distCorrectionValue", distCorrectionValue.getMetresValue());
		obj.addProperty("ID", ID);
		obj.addProperty("ppmDist", ppmDist.getMetresValue());

		obj.addProperty("sigmaAngl", sigmaAngl.getRadiansValue());
		obj.addProperty("sigmaCombinedAngle", sigmaCombinedAngle.getRadiansValue());
		obj.addProperty("sigmaCombinedDist", sigmaCombinedDist.getMetresValue());
		obj.addProperty("sigmaCombinedPLRAngl", sigmaCombinedPLRAngl.getRadiansValue());
		obj.addProperty("sigmaCombinedPLRDist", sigmaCombinedPLRDist.getMetresValue());
		
		obj.addProperty("sigmaCombinedPLRZenD", sigmaCombinedPLRZenD.getRadiansValue());
		obj.addProperty("sigmaDCorr", sigmaDCorr.getMetresValue());
		obj.addProperty("sigmaDist", sigmaDist.getMetresValue());
		obj.addProperty("sigmaTargetCentering", sigmaTargetCentering.getMetresValue());
		obj.addProperty("sigmaTargetHt", sigmaTargetHt.getMetresValue());

		obj.addProperty("sigmaZenD", sigmaZenD.getRadiansValue());
		obj.addProperty("targetHt", targetHt.getMetresValue());
		
	}
	
	inline void TInstrumentData::TCAMD::serialize(SerializerObject::SerializationHelper &obj) const
	{
		obj.addProperty("defTarget", defTarget);
		obj.addProperty("ID", ID);
		obj.addProperty("sigmaInstrCentering", sigmaInstrCentering.getMetresValue());
		obj.addProperty("targets", targets);
	}

	inline void TInstrumentData::TCAMD::TTarget::serialize(SerializerObject::SerializationHelper &obj) const
	{
		obj.addProperty("ID", ID);
		obj.addProperty("sigmaCombinedDist", sigmaCombinedDist.getMetresValue());
		obj.addProperty("sigmaCombinedX", sigmaCombinedX.getMetresValue());
		obj.addProperty("sigmaCombinedY", sigmaCombinedY.getMetresValue());
		obj.addProperty("sigmaDist", sigmaDist.getMetresValue());
		obj.addProperty("sigmaTargetCentering", sigmaTargetCentering.getMetresValue());
		obj.addProperty("sigmaX", sigmaX);
		obj.addProperty("sigmaY", sigmaY);
	}

	inline void TInstrumentData::TEDM::serialize(SerializerObject::SerializationHelper &obj) const
	{
		obj.addProperty("defTarget", defTarget);
		obj.addProperty("ID", ID);
		obj.addProperty("instrHeight", instrHeight.getMetresValue());
		obj.addProperty("sigmaInstrCentering", sigmaInstrCentering.getMetresValue());
		obj.addProperty("sigmaInstrHeight", sigmaInstrHeight.getMetresValue());
		obj.addProperty("targets", targets);
	}

	inline void TInstrumentData::TEDM::TTarget::serialize(SerializerObject::SerializationHelper &obj) const
	{
		obj.addProperty("distCorrectionAdjustable", distCorrectionAdjustable);
		obj.addProperty("distCorrectionUnknown", distCorrectionUnknown);
		obj.addProperty("distCorrectionValue", distCorrectionValue.getMetresValue());
		obj.addProperty("ID", ID);
		obj.addProperty("ppmDSpt", ppmDSpt.getMetresValue());
		obj.addProperty("sigmaCombinedDist", sigmaCombinedDist.getMetresValue());
		obj.addProperty("sigmaDCorr", sigmaDCorr.getMetresValue());
		obj.addProperty("sigmaDSpt", sigmaDSpt.getMetresValue());
		obj.addProperty("sigmaTargetCentering", sigmaTargetCentering.getMetresValue());
		obj.addProperty("sigmaTargetHt", sigmaTargetHt.getMetresValue());
		obj.addProperty("targetHt", targetHt.getMetresValue());
	}

	inline void TInstrumentData::TLEVEL::serialize(SerializerObject::SerializationHelper &obj) const
	{
		obj.addProperty("collAngleAdjustable", collAngleAdjustable);
		obj.addProperty("collAngleUnknown", collAngleUnknown);
		obj.addProperty("collAngleValue", collAngleValue.getRadiansValue());
		obj.addProperty("defStaffID", defStaffID);
		obj.addProperty("ID", ID);
		obj.addProperty("targets", targets);
	}

	inline void TInstrumentData::TLEVEL::TTarget::serialize(SerializerObject::SerializationHelper &obj) const
	{
		obj.addProperty("distCorrectionValue", distCorrectionValue.getMetresValue());
		obj.addProperty("ID", ID);
		obj.addProperty("ppmD", ppmD.getMetresValue());
		obj.addProperty("sigmaCombinedDist", sigmaCombinedDist.getMetresValue());
		obj.addProperty("sigmaD", sigmaD.getMetresValue());
		obj.addProperty("sigmaDCorr", sigmaDCorr.getMetresValue());
		obj.addProperty("sigmaStaffHt", sigmaStaffHt.getMetresValue());
		obj.addProperty("staffHt", staffHt.getMetresValue());
	}

	inline void TInstrumentData::TSCALE::serialize(SerializerObject::SerializationHelper &obj) const
	{
		obj.addProperty("distCorrectionValue", distCorrectionValue.getMetresValue());
		obj.addProperty("ID", ID);
		obj.addProperty("ppmD", ppmD.getMetresValue());
		obj.addProperty("sigmaCombinedDist", sigmaCombinedDist.getMetresValue());
		obj.addProperty("sigmaD", sigmaD.getMetresValue());
		obj.addProperty("sigmaDCorr", sigmaDCorr.getMetresValue());
		obj.addProperty("sigmaInstrCentering", sigmaInstrCentering.getMetresValue());
	}

	inline void TInstrumentData::TINCL::serialize(SerializerObject::SerializationHelper &obj) const
	{
		obj.addProperty("angleCorrectionValue", angleCorrectionValue.getRadiansValue());
		obj.addProperty("ID", ID);
		obj.addProperty("refAngleCorrectionValue", refAngleCorrectionValue.getRadiansValue());
		obj.addProperty("refSigmaCorrectionValue", refSigmaCorrectionValue.getRadiansValue());
		obj.addProperty("sigmaAngl", sigmaAngl.getRadiansValue());
		obj.addProperty("sigmaCombinedAngle", sigmaCombinedAngle.getRadiansValue());
		obj.addProperty("sigmaCorrectionValue", sigmaCorrectionValue.getRadiansValue());
	}

	inline void TInstrumentData::THLSR::serialize(SerializerObject::SerializationHelper &obj) const
	{
		obj.addProperty("ID", ID);
		obj.addProperty("sigmaCombinedDist", sigmaCombinedDist.getMetresValue());
		obj.addProperty("sigmaDist", sigmaDist.getMetresValue());
		obj.addProperty("sigmaInstrCentering", sigmaInstrCentering.getMetresValue());
		obj.addProperty("sigmaInstrHeight", sigmaInstrHeight.getMetresValue());
		obj.addProperty("sigmaWS", sigmaWS.getMetresValue());
	}


	inline void TInstrumentData::TWPSR::serialize(SerializerObject::SerializationHelper &obj) const
	{
		obj.addProperty("ID", ID);
		obj.addProperty("sigmaX", sigmaX.getMetresValue());
		obj.addProperty("sigmaZ", sigmaZ.getMetresValue());
		obj.addProperty("sigmaInstrCenteringX", sigmaInstrCenteringX.getMetresValue());
		obj.addProperty("sigmaInstrCenteringZ", sigmaInstrCenteringZ.getMetresValue());
		obj.addProperty("sagWire", sagWire.getMetresValue());
		obj.addProperty("sigmaSagWire", sigmaSagWire.getMetresValue());
		obj.addProperty("sigmaWire", sigmaWire.getMetresValue());
		obj.addProperty("sigmaCombinedX", sigmaCombinedX.getMetresValue());
		obj.addProperty("sigmaCombinedZ", sigmaCombinedZ.getMetresValue());
	}

#endif // USE_SERIALIZER

#endif
