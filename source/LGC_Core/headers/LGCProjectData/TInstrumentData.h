/*
� Copyright CERN 2000-2024. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _INSTRUMENTDATA_H_
#define _INSTRUMENTDATA_H_

// STL
#include <map>

#include <Global.h>
// SURVEYLIB
#include <TAdjustableAngle.h>
#include <TAdjustableLength.h>

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
				int line = 0,
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
				line(line),
				sigmaCombinedDist(sigmaCombinedDist),
				sigmaCombinedAngle(sigmaCombinedAngle),
				sigmaCombinedPLRAngl(sigmaCombinedPLRAngl),
				sigmaCombinedPLRZenD(sigmaCombinedPLRZenD),
				sigmaCombinedPLRDist(sigmaCombinedPLRDist) {};

			std::string ID;
			TAngle sigmaAngl; // [rad]
			TAngle sigmaZenD; // [rad]
			TLength sigmaDist; // [m]
			TLength ppmDist; // [m]
			bool distCorrectionUnknown;
			TLength distCorrectionValue; // [m]
			TLength sigmaDCorr; // [m]
			TLength sigmaTargetCentering; // [m]
			TLength targetHt; // [m]
			TLength sigmaTargetHt; // [m]
			TAdjustableLength *distCorrectionAdjustable; // Adjustable object for "distCorrectionValue"
			int line;
			TLength sigmaCombinedDist; // [m]
			TAngle sigmaCombinedAngle; // [rad]
			TAngle sigmaCombinedPLRAngl; // [rad]
			TAngle sigmaCombinedPLRZenD; // [rad]
			TLength sigmaCombinedPLRDist; // [m]

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(ObjectSerializer &obj) const override;
#endif // USE_SERIALIZER
		};

		TPOLAR() = default;
		TPOLAR(const std::string &instr_id, const std::string &defTgt, const TLength &instrH, const TLength &sigmaInstrH, const TLength &sigmaInstrCent, const TAngle &cstAngle, const int &line) :
			ID(instr_id), defTarget(defTgt), instrHeight(instrH), sigmaInstrHeight(sigmaInstrH), sigmaInstrCentering(sigmaInstrCent), constAngle(cstAngle), line(line)
		{
		}

		TPOLAR(const TPOLAR &instr)
		{
			*this = instr; // Use the assignment operator to have the targets deep copied
		}
		TPOLAR &operator=(const TPOLAR &other)
		{
			if (this == &other)
				return *this;

			ID = other.ID;
			defTarget = other.defTarget;
			instrHeight = other.instrHeight;
			sigmaInstrHeight = other.sigmaInstrHeight;
			sigmaInstrCentering = other.sigmaInstrCentering;
			constAngle = other.constAngle;
			line = other.line;
			targets = other.targets;

			for (auto &tgt : targets)
				// Replace the target in the memory
				tgt.second.reset(new TTarget(*tgt.second));

			return *this;
		}

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override;
#endif // USE_SERIALIZER

		std::string ID;
		std::string defTarget;
		TLength instrHeight; // [m]
		TLength sigmaInstrHeight; // [m]
		TLength sigmaInstrCentering; // [m]
		TAngle constAngle; // [rad]
		int line;
		std::map<std::string, std::shared_ptr<TTarget>> targets; /// allows the lookup of targets for this instrument based on the target ID.
	};

	// Definition of camera parameters
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
				int line = 0,
				TLength sigmaCombinedX = TLength(),
				TLength sigmaCombinedY = TLength(),
				TLength sigmaCombinedDist = TLength()) :
				ID(ID),
				sigmaX(sigmaX),
				sigmaY(sigmaY),
				sigmaDist(sigmaDist),
				sigmaTargetCentering(sigmaTargetCentering),
				line(line),
				sigmaCombinedX(sigmaCombinedX),
				sigmaCombinedY(sigmaCombinedY),
				sigmaCombinedDist(sigmaCombinedDist) {};

			std::string ID;
			TReal sigmaX; // [] unitless
			TReal sigmaY; // [] unitless
			TLength sigmaDist; // [m]
			TLength sigmaTargetCentering; // [m]
			int line;
			TLength sigmaCombinedX; // [m]
			TLength sigmaCombinedY; // [m]
			TLength sigmaCombinedDist; // [m]

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(ObjectSerializer &obj) const override;
#endif // USE_SERIALIZER
		};

		TCAMD() = default;
		TCAMD(const std::string &instr_id, const std::string &defTgt, const TLength &sigmaInstrCent, const int &line) :
			ID(instr_id), defTarget(defTgt), sigmaInstrCentering(sigmaInstrCent), line(line)
		{
		}

		TCAMD(const TCAMD &instr)
		{
			*this = instr; // Use the assignment operator to have the targets deep copied
		}

		TCAMD &operator=(const TCAMD &other)
		{
			if (this == &other)
				return *this;

			ID = other.ID;
			defTarget = other.defTarget;
			sigmaInstrCentering = other.sigmaInstrCentering;
			line - other.line;
			targets = other.targets;

			for (auto &tgt : targets)
				// Replace the target in the memory
				tgt.second.reset(new TTarget(*tgt.second));

			return *this;
		}

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override;
#endif // USE_SERIALIZER

		std::string ID;
		std::string defTarget;
		TLength sigmaInstrCentering; // [m]
		int line;
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
				int line = 0,
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
				line(line),
				sigmaCombinedDist(sigmaCombinedDist) {};

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
			int line;
			TLength sigmaCombinedDist; // [m]

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(ObjectSerializer &obj) const override;
#endif // USE_SERIALIZER
		};

		TEDM() = default;
		TEDM(const std::string &instr_id, const std::string &defTgt, const TLength &instrH, const TLength &sigmaInstrH, const TLength &sigmaInstrCent, const int &line) :
			ID(instr_id), defTarget(defTgt), instrHeight(instrH), sigmaInstrHeight(sigmaInstrH), sigmaInstrCentering(sigmaInstrCent), line(line)
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
			line = other.line;
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
		int line;
		std::map<std::string, std::shared_ptr<TTarget>> targets; /// allows the lookup of targets for this instrument based on the target ID.

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override;
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
				TLength sigmaDHor = TLength(),
				TLength ppmDHor = TLength(),
				TLength dhorCorrectionValue = TLength(),
				int line = 0,
				TLength sigmaCombinedDist = TLength(),
				TLength sigmaCombinedDHor = TLength()) :
				ID(ID),
				sigmaD(sigmaD),
				ppmD(ppmD),
				distCorrectionValue(distCorrectionValue),
				sigmaDCorr(sigmaDCorr),
				staffHt(staffHt),
				sigmaStaffHt(sigmaStaffHt),
				sigmaDHor(sigmaDHor),
				ppmDHor(ppmDHor),
				dhorCorrectionValue(dhorCorrectionValue),
				line(line),
				sigmaCombinedDist(sigmaCombinedDist),
				sigmaCombinedDHor(sigmaCombinedDHor) {};

			std::string ID;
			TLength sigmaD; // [m]
			TLength ppmD; // [m]
			TLength distCorrectionValue; // [m]
			TLength sigmaDCorr; // [m]
			TLength staffHt; // [m] i.e vertical offset of the staff = staff height
			TLength sigmaStaffHt; // [m] standard deviation of the vertical offset of the staff
			TLength sigmaDHor; // [m] Standard deviation of a horizontal distance
			TLength ppmDHor; // ppm of a horizontal distance
			TLength dhorCorrectionValue; // [m] correction to apply on a horizontal distance
			TLength sigmaCombinedDist;
			TLength sigmaCombinedDHor;
			int line;

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(ObjectSerializer &obj) const override;
#endif // USE_SERIALIZER
		};

		TLEVEL() = default;
		TLEVEL(const std::string &instr_id,
			const std::string &defStaff,
			const TLength &instrH,
			const TLength &sigmaInstrH,
			const bool &collAnglUnknown,
			const TAngle &collAnglVal,
			TAdjustableAngle *collAnglAdj,
			const int line) :
			ID(instr_id),
			defStaffID(defStaff),
			instrHeight(instrH),
			sigmaInstrHeight(sigmaInstrH),
			collAngleUnknown(collAnglUnknown),
			collAngleValue(collAnglVal),
			collAngleAdjustable(collAnglAdj),
			line(line)
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
			instrHeight = other.instrHeight;
			sigmaInstrHeight = other.sigmaInstrHeight;
			collAngleUnknown = other.collAngleUnknown;
			collAngleValue = other.collAngleValue;
			collAngleAdjustable = other.collAngleAdjustable;
			line = other.line;
			targets = other.targets;

			for (auto &tgt : targets)
				// Replace the target in the memory
				tgt.second.reset(new TTarget(*tgt.second));

			return *this;
		}

		std::string ID;
		std::string defStaffID;
		TLength instrHeight; // [m]
		TLength sigmaInstrHeight; // [m]
		bool collAngleUnknown;
		TAngle collAngleValue; // [rad]
		TAdjustableAngle *collAngleAdjustable;
		int line;
		std::map<std::string, std::shared_ptr<TTarget>> targets; /// allows the lookup of targets for this instrument based on the target ID.

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override;
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
			int line = 0,
			TLength sigmaCombinedDist = TLength()) :
			ID(ID),
			sigmaD(sigmaD),
			ppmD(ppmD),
			distCorrectionValue(distCorrectionValue),
			sigmaDCorr(sigmaDCorr),
			sigmaInstrCentering(sigmaInstrCentering),
			line(line),
			sigmaCombinedDist(sigmaCombinedDist) {};

		std::string ID;
		TLength sigmaD; // [m]
		TLength ppmD; // [m]
		TLength distCorrectionValue; // [m]
		TLength sigmaDCorr; // [m]
		TLength sigmaInstrCentering; // [m]
		int line;
		TLength sigmaCombinedDist; // [m]

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override;
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
			TAngle sigmaPpm = TAngle(),
			TAngle angleCorrectionValue = TAngle(),
			TAngle sigmaCorrectionValue = TAngle(),
			TAngle refAngleCorrectionValue = TAngle(),
			TAngle refSigmaCorrectionValue = TAngle(),
			int line = 0,
			TAngle sigmaCombinedAngle = TAngle()) :
			ID(ID),
			sigmaAngl(sigmaAngl),
			sigmaPpm(sigmaPpm),
			angleCorrectionValue(angleCorrectionValue),
			sigmaCorrectionValue(sigmaCorrectionValue),
			refAngleCorrectionValue(refAngleCorrectionValue),
			refSigmaCorrectionValue(refSigmaCorrectionValue),
			line(line),
			sigmaCombinedAngle(sigmaCombinedAngle) {};

		std::string ID;
		TAngle sigmaAngl; // [rad]
		TAngle sigmaPpm; // [rad]
		TAngle angleCorrectionValue; // [rad]
		TAngle sigmaCorrectionValue; // [rad]
		TAngle refAngleCorrectionValue; // [rad]
		TAngle refSigmaCorrectionValue; // [rad]
		int line;
		TAngle sigmaCombinedAngle; // [rad]

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override;
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
			int line = 0,
			TLength sigmaWS = TLength(),
			TLength sigmaCombinedDist = TLength()) :
			ID(ID), sigmaDist(sigmaDist), sigmaInstrHeight(sigmaInstrHeight), sigmaInstrCentering(sigmaInstrCentering), line(line), sigmaWS(sigmaWS), sigmaCombinedDist(sigmaCombinedDist) {};

		std::string ID;
		TLength sigmaDist;
		TLength sigmaInstrHeight;
		TLength sigmaInstrCentering;
		int line;
		TLength sigmaWS;
		TLength sigmaCombinedDist;

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override;
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
			TLength sigmaInstrCenteringX = TLength(),
			TLength sigmaInstrCenteringZ = TLength(),
			TLength sagWire = TLength(),
			TLength sigmaSagWire = TLength(),
			TLength sigmaWire = TLength(),
			TLength sigmaCombinedX = TLength(),
			TLength sigmaCombinedZ = TLength(),
			int line = 0) :
			ID(ID),
			sigmaX(sigmaX),
			sigmaZ(sigmaZ),
			sigmaInstrCenteringX(sigmaInstrCenteringX),
			sigmaInstrCenteringZ(sigmaInstrCenteringZ),
			sagWire(sagWire),
			sigmaSagWire(sigmaSagWire),
			sigmaWire(sigmaWire),
			sigmaCombinedX(sigmaCombinedX),
			sigmaCombinedZ(sigmaCombinedZ),
			line(line) {};

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
		int line;

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override;
#endif // USE_SERIALIZER
	};

	/// All available polar instruments, accessible by their ID. See \ref getDevice for failsave lookup.
	std::map<std::string, std::shared_ptr<TPOLAR>> fPOLAR;
	/// All available camera instruments, accessible by their ID. See \ref getDevice for failsave lookup.
	std::map<std::string, std::shared_ptr<TCAMD>> fCAMD;
	/// All available distance meters, accessible by their ID. See \ref getDevice for failsave lookup.
	std::map<std::string, std::shared_ptr<TEDM>> fEDM;
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
	const T &getDevice(const std::map<std::string, std::shared_ptr<T>> &m, const std::string &id) const
	{
		auto it(m.find(id));
		if (it == m.end())
			throw std::runtime_error("Could not find device " + id + ".");

		return *it->second;
	}

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif // USE_SERIALIZER
};

#if USE_SERIALIZER
inline void TInstrumentData::serialize(ObjectSerializer &obj) const
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

inline void TInstrumentData::TPOLAR::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("constAngle", constAngle.getRadiansValue());
	obj.addProperty("defTarget", defTarget);
	obj.addProperty("ID", ID);
	obj.addProperty("instrHeight", instrHeight.getMetresValue());
	obj.addProperty("sigmaInstrCentering", sigmaInstrCentering.getMetresValue());
	obj.addProperty("sigmaInstrHeight", sigmaInstrHeight.getMetresValue());
	obj.addProperty("line", line);
	obj.addProperty("targets", targets);
}

inline void TInstrumentData::TPOLAR::TTarget::serialize(ObjectSerializer &obj) const
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
	obj.addProperty("line", line);
}

inline void TInstrumentData::TCAMD::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("defTarget", defTarget);
	obj.addProperty("ID", ID);
	obj.addProperty("sigmaInstrCentering", sigmaInstrCentering.getMetresValue());
	obj.addProperty("line", line);
	obj.addProperty("targets", targets);
}

inline void TInstrumentData::TCAMD::TTarget::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("ID", ID);
	obj.addProperty("sigmaCombinedDist", sigmaCombinedDist.getMetresValue());
	obj.addProperty("sigmaCombinedX", sigmaCombinedX.getMetresValue());
	obj.addProperty("sigmaCombinedY", sigmaCombinedY.getMetresValue());
	obj.addProperty("sigmaDist", sigmaDist.getMetresValue());
	obj.addProperty("sigmaTargetCentering", sigmaTargetCentering.getMetresValue());
	obj.addProperty("sigmaX", sigmaX);
	obj.addProperty("sigmaY", sigmaY);
	obj.addProperty("line", line);
}

inline void TInstrumentData::TEDM::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("defTarget", defTarget);
	obj.addProperty("ID", ID);
	obj.addProperty("instrHeight", instrHeight.getMetresValue());
	obj.addProperty("sigmaInstrCentering", sigmaInstrCentering.getMetresValue());
	obj.addProperty("sigmaInstrHeight", sigmaInstrHeight.getMetresValue());
	obj.addProperty("line", line);
	obj.addProperty("targets", targets);
}

inline void TInstrumentData::TEDM::TTarget::serialize(ObjectSerializer &obj) const
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
	obj.addProperty("line", line);
}

inline void TInstrumentData::TLEVEL::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("collAngleAdjustable", collAngleAdjustable);
	obj.addProperty("collAngleUnknown", collAngleUnknown);
	obj.addProperty("collAngleValue", collAngleValue.getRadiansValue());
	obj.addProperty("instrHeight", instrHeight.getMetresValue());
	obj.addProperty("sigmaInstrHeight", sigmaInstrHeight.getMetresValue());
	obj.addProperty("defStaffID", defStaffID);
	obj.addProperty("ID", ID);
	obj.addProperty("line", line);
	obj.addProperty("targets", targets);
}

inline void TInstrumentData::TLEVEL::TTarget::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("distCorrectionValue", distCorrectionValue.getMetresValue());
	obj.addProperty("ID", ID);
	obj.addProperty("ppmD", ppmD.getMetresValue());
	obj.addProperty("sigmaCombinedDist", sigmaCombinedDist.getMetresValue());
	obj.addProperty("sigmaD", sigmaD.getMetresValue());
	obj.addProperty("sigmaDCorr", sigmaDCorr.getMetresValue());
	obj.addProperty("sigmaStaffHt", sigmaStaffHt.getMetresValue());
	obj.addProperty("staffHt", staffHt.getMetresValue());
	obj.addProperty("sigmaDHor", sigmaDHor.getMetresValue());
	obj.addProperty("ppmDHor", ppmDHor.getMetresValue());
	obj.addProperty("dhorCorrectionValue", dhorCorrectionValue.getMetresValue());
	obj.addProperty("sigmaCombinedDHor", sigmaCombinedDHor.getMetresValue());
	obj.addProperty("line", line);
}

inline void TInstrumentData::TSCALE::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("distCorrectionValue", distCorrectionValue.getMetresValue());
	obj.addProperty("ID", ID);
	obj.addProperty("ppmD", ppmD.getMetresValue());
	obj.addProperty("sigmaCombinedDist", sigmaCombinedDist.getMetresValue());
	obj.addProperty("sigmaD", sigmaD.getMetresValue());
	obj.addProperty("sigmaDCorr", sigmaDCorr.getMetresValue());
	obj.addProperty("sigmaInstrCentering", sigmaInstrCentering.getMetresValue());
	obj.addProperty("line", line);
}

inline void TInstrumentData::TINCL::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("angleCorrectionValue", angleCorrectionValue.getRadiansValue());
	obj.addProperty("ID", ID);
	obj.addProperty("refAngleCorrectionValue", refAngleCorrectionValue.getRadiansValue());
	obj.addProperty("refSigmaCorrectionValue", refSigmaCorrectionValue.getRadiansValue());
	obj.addProperty("sigmaAngl", sigmaAngl.getRadiansValue());
	obj.addProperty("sigmaPpm", sigmaPpm.getRadiansValue());
	obj.addProperty("sigmaCombinedAngle", sigmaCombinedAngle.getRadiansValue());
	obj.addProperty("sigmaCorrectionValue", sigmaCorrectionValue.getRadiansValue());
	obj.addProperty("line", line);
}

inline void TInstrumentData::THLSR::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("ID", ID);
	obj.addProperty("sigmaCombinedDist", sigmaCombinedDist.getMetresValue());
	obj.addProperty("sigmaDist", sigmaDist.getMetresValue());
	obj.addProperty("sigmaInstrCentering", sigmaInstrCentering.getMetresValue());
	obj.addProperty("sigmaInstrHeight", sigmaInstrHeight.getMetresValue());
	obj.addProperty("sigmaWS", sigmaWS.getMetresValue());
	obj.addProperty("line", line);
}

inline void TInstrumentData::TWPSR::serialize(ObjectSerializer &obj) const
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
	obj.addProperty("line", line);
}

#endif // USE_SERIALIZER

#endif
