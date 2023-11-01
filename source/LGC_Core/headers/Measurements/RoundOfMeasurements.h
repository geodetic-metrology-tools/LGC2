/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef Round_Of_Measurements
#define Round_Of_Measurements


//LGC
#include <MeasDef.h>
#include <LGCAdjustablePlane.h>
#include <LGCAdjustableLine.h>
#include <TPositionVector.h>
#include "TLGCObsSummary.h"


/*!
	\ingroup Measurements
	\brief This class represents a round of ECHO (TECHO) measurements, which are measuring a single Plane.
*/
struct TECHOROM : public TStatusObject {

		/// All ECHO measurements, measuring the 'fmeasuredPlane'
		std::list<TECHO> measECHO;

		//Measured vertical plane
		LGCAdjustablePlane* fMeasuredPlane;

        /// Initialise observation summaries
        void initialiseObsSummaries();

		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
        const TLGCObsSummary& getECHOObsSummary() const;
		const TLGCObsSummary& getECHOObsSummary(std::string text) noexcept;

		/// Line of the measurement definition
		int  line;

        int romId{ romCounter_++ };

		/// the station attribute is a copy of the parameter to override defaults
		TECHOROM(LGCAdjustablePlane *measPlane) :
			fMeasuredPlane(measPlane),
			line(NO_VALi)
			{}

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override;
#endif

	private:

    static int romCounter_;

    TLGCObsSummary echoSummary_;
};

/*!
\ingroup Measurements
\brief This class represents a round of ECSP (TECSP) measurements, which are measuring a single Plane.
*/
struct TECSPROM : public TStatusObject {

	/// All ECHO measurements, measuring the 'fmeasuredPlane'
	std::list<TECSP> measECSP;

	//Measured line
	//LGCAdjustableLine* fMeasuredLine;
	const LGCAdjustablePoint* p1;
	const LGCAdjustablePoint* p2;

    /// Initialise observation summaries
    void initialiseObsSummaries();

	/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getECSPObsSummary() const;
	const TLGCObsSummary& getECSPObsSummary(std::string text) noexcept;

	/// Line of the measurement definition
	int  line;
	/// name of the rom
	std::string romName;

    int romId{ romCounter_++ };

	/// the station attribute is a copy of the parameter to override defaults
	/*TECSPROM(LGCAdjustableLine& measLine) :
		fMeasuredLine(&measLine),
		line(NO_VALi),
		p1(nullptr),
		p2(nullptr)
	{}

	/// the station attribute is a copy of the parameter to override defaults
	TECSPROM(LGCAdjustableLine& measLine, const LGCAdjustablePoint& point1, const LGCAdjustablePoint& point2) :
		fMeasuredLine(&measLine),
		p1(&point1),
		p2(&point2),
		line(NO_VALi)
	{}*/

	TECSPROM(std::string name) :
		romName(name),
		line(NO_VALi),
		p1(nullptr),
		p2(nullptr)
	{}

	/// the station attribute is a copy of the parameter to override defaults
	TECSPROM(std::string name,const LGCAdjustablePoint& point1, const LGCAdjustablePoint& point2) :
		romName(name),
		p1(&point1),
		p2(&point2),
		line(NO_VALi)
	{}

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif

private:

    static int romCounter_;

    TLGCObsSummary ecspSummary_;
};

/*!
\ingroup Measurements
\brief This class represents a round of ECVE (TECVE) measurements, which are measuring a single Plane.
*/
struct TECVEROM : public TStatusObject {

	/// All ECHO measurements, measuring the 'fmeasuredPlane'
	std::list<TECVE> measECVE;

	//Measured vertical plane
	LGCAdjustableLine* fMeasuredLine;

    /// Point on the measured line
    LGCAdjustablePoint const * fPtLine;

    /// Initialise observation summaries
    void initialiseObsSummaries();

	/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getECVEObsSummary() const;
	const TLGCObsSummary& getECVEObsSummary(std::string text) noexcept;


	/// Line of the measurement definition
	int  line;

    int romId{ romCounter_++ };

	/// the station attribute is a copy of the parameter to override defaults
    TECVEROM(LGCAdjustablePoint const * ptLine) :
		fMeasuredLine(nullptr),
        fPtLine(ptLine),
		line(NO_VALi)
	{}

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif

private:

    static int romCounter_;

    TLGCObsSummary ecveSummary_;
};

/*!
	\ingroup Measurements
	\brief This class represents a round of ORIE (TORIE) measurements made from a single position of the instrument.
*/
struct TORIEROM : public TStatusObject {

		/// All ORIE measurements
		std::list<TORIE> measORIE;

		/// The position of the instrument
		const LGCAdjustablePoint* instrumentPos;

		/// The instrument that is used on this station
		TInstrumentData::TPOLAR     instrument;

        /// Initialise observation summaries
        void initialiseObsSummaries();

		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
        const TLGCObsSummary& getORIEObsSummary() const;
		const TLGCObsSummary& getORIEObsSummary(std::string text) noexcept;


		TAngle fConstantAngle;

		/// Line of the measurement definition
		int  line;

        int romId{ romCounter_++ };

		/// the station attribute is a copy of the parameter to override defaults
		TORIEROM(const LGCAdjustablePoint& pos, const TInstrumentData::TPOLAR& instrument) :
			instrumentPos(&pos),
			instrument(instrument),
			line(NO_VALi),
			fConstantAngle(0.0, TAngle::EUnits::kGons)
		{
		}

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override;
#endif

    private:
        
        static int romCounter_;

        TLGCObsSummary orieSummary_;
};

/*!
	\ingroup Measurements
	\brief This class represents a round of INCLY (TINCLY) measurements made from a single position of the instrument.
*/
struct TINCLYROM : public TStatusObject {

	/// All INCLY measurements
	std::list<TINCLY> measINCLY;

	/// The instrument that is used on this station
	TInstrumentData::TINCL     instrument;

	/// Line of the measurement definition
	int  line;
	int romId = TINCLYROM::romCounter_++;

	TDataTreeIterator positionInTree;

	/// the station attribute is a copy of the parameter to override defaults
	TINCLYROM(const TInstrumentData::TINCL& instrument, TDataTreeIterator itTree) :
		instrument(instrument),
		line(NO_VALi),
		positionInTree(itTree)
	{
	}

	/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
	const TLGCObsSummary& getINCLYObsSummary() const;
	const TLGCObsSummary& getINCLYObsSummary(std::string text) noexcept;

	/// Initialise observation summaries
	void initialiseObsSummaries();

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif

private:

	static int romCounter_;

	TLGCObsSummary inclySummary_;
};

/*!
	\ingroup Measurements
	\brief This class represents a round of ECWS (TECWS) measurements, which are measuring a single Plane.
*/
struct TECWSROM : public TStatusObject {

	/// All ECWS measurements, measuring the water surface distance
	std::list<TECWS> measECWS;

	/// The instrument that is used on this station
	TInstrumentData::THLSR    instrument;

	/// name of the rom
	std::string romName;

	//Measured water surface height, non-owning pointer
	TAdjustableLength* fMeasuredWSHeight;

	//1-sigma precision of the water surface
	TLength sigmaWS;

	/// Initialise observation summaries
	void initialiseObsSummaries();

	/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
	const TLGCObsSummary& getECWSObsSummary() const;
	const TLGCObsSummary& getECWSObsSummary(std::string text) noexcept;

	/// Line of the measurement definition
	int line;
	int romId = TECWSROM::romCounter_++;


	TECWSROM(const TInstrumentData::THLSR& inst, TLength sigmaWS, TAdjustableLength *MeasuredHeight) :
		instrument(inst),
		fMeasuredWSHeight(MeasuredHeight),
		sigmaWS(sigmaWS),
		romName(""),
		line(NO_VALi)
	{}

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif

private:

	static int romCounter_;

	TLGCObsSummary ecwsSummary_;
};

/*!
	\ingroup Measurements
	\brief This class represents a round of ECWI (TECWI) measurements which are measuring a single Wire.
*/
struct TECWIROM : public TStatusObject
{
	/// All ECWI measurements, measuring the same wire
	std::list<TECWI> measECWI;

	/// The instrument that is used on this station
	TInstrumentData::TWPSR instrument;

	// Lomg term: use the AdjustableLine to handle the wire definition (watchout to vertical case).
	// position of the Reference point (fixed)
	TPositionVector referencePoint;

	// Wire dX, non-owning pointer
	TAdjustableLength *fWireDx;

	// Wire dZ, non-owning pointer
	TAdjustableLength *fWireDz;

	// Wire Bearing, non-owning pointer
	TAdjustableAngle *fWireBearing;

	// Wire Slope, non-owning pointer
	TAdjustableAngle *fWireSlope;

	/// First anchor point, non-owning pointer
	const LGCAdjustablePoint *anchorPtFirst;

	/// Second anchor point, non-owning pointer
	const LGCAdjustablePoint *anchorPtSecond;

	// 1-sigma precision of the wire
	TLength sigmaWire;

	// Adjustable sag, the sag can be fixed or variable, non-owning pointer
	const TAdjustableLength *sagAdjustable;

	// Tells whether the instrument height is fixed or not
	bool sagfix;

	/// name of the rom
	std::string romName;

	/// Initialise observation summaries
	void initialiseObsSummaries();

	/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
	const TECWIObsSummary &getECWIObsSummary() const;
	const TECWIObsSummary &getECWIObsSummary(std::string text) noexcept;

	/// Line of the measurement definition
	int line;
	int romId = TECWIROM::romCounter_++;

	TECWIROM(const TInstrumentData::TWPSR &inst, TLength sigmaWire, const LGCAdjustablePoint &anchor1, const LGCAdjustablePoint &anchor2) :
		instrument(inst),
		referencePoint(TPositionVector(TReal(0), TReal(0), TReal(0), TCoordSysFactory::ECoordSys::k3DCartesian)),
		fWireDx(nullptr),
		fWireDz(nullptr),
		fWireBearing(nullptr),
		fWireSlope(nullptr),
		sagfix(false),
		sagAdjustable(nullptr),
		sigmaWire(sigmaWire),
		anchorPtFirst(&anchor1),
		anchorPtSecond(&anchor2),
		romName(""),
		line(NO_VALi)
	{
	}

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif

private:
	static int romCounter_;

	TECWIObsSummary ecwiSummary_;
};
#endif
