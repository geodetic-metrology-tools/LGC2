#ifndef Round_Of_Measurements
#define Round_Of_Measurements

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

//LGC
#include <MeasDef.h>
#include <LGCAdjustablePlane.h>
#include <LGCAdjustableLine.h>
#include "TLGCObsSummary.h"


/*!
	\ingroup Measurements
	\brief This class represents a round of ECHO (TECHO) measurements, which are measuring a single Plane.
*/
struct TECHOROM {

		/// All ECHO measurements, measuring the 'fmeasuredPlane'
		std::list<TECHO> measECHO;

		//Measured vertical plane
		LGCAdjustablePlane* fMeasuredPlane;

        /// Initialise observation summaries
        void initialiseObsSummaries();

		/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
        const TLGCObsSummary& getECHOObsSummary() const;

		/// Line of the measurement definition
		int  line;

        int romId{ romCounter_++ };

		/// the station attribute is a copy of the parameter to override defaults
		TECHOROM(LGCAdjustablePlane *measPlane) :
			fMeasuredPlane(measPlane),
			line(NO_VALi)
			{}

private:

    static int romCounter_;

    TLGCObsSummary echoSummary_;
};

/*!
\ingroup Measurements
\brief This class represents a round of ECSP (TECSP) measurements, which are measuring a single Plane.
*/
struct TECSPROM {

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

	/// Line of the measurement definition
	int  line;
	/// name of the rom
	string romName;

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

	TECSPROM(string name) :
		romName(name),
		line(NO_VALi),
		p1(nullptr),
		p2(nullptr)
	{}

	/// the station attribute is a copy of the parameter to override defaults
	TECSPROM(string name,const LGCAdjustablePoint& point1, const LGCAdjustablePoint& point2) :
		romName(name),
		p1(&point1),
		p2(&point2),
		line(NO_VALi)
	{}

private:

    static int romCounter_;

    TLGCObsSummary ecspSummary_;
};

/*!
\ingroup Measurements
\brief This class represents a round of ECVE (TECVE) measurements, which are measuring a single Plane.
*/
struct TECVEROM {

	/// All ECHO measurements, measuring the 'fmeasuredPlane'
	std::list<TECVE> measECVE;

	//Measured vertical plane
	LGCAdjustableLine* fMeasuredLine;

    /// Initialise observation summaries
    void initialiseObsSummaries();

	/// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getECVEObsSummary() const;

	/// Line of the measurement definition
	int  line;

    int romId{ romCounter_++ };

	/// the station attribute is a copy of the parameter to override defaults
	TECVEROM(LGCAdjustableLine& measLine) :
		fMeasuredLine(&measLine),
		line(NO_VALi)
	{}

private:

    static int romCounter_;

    TLGCObsSummary ecveSummary_;
};

/*!
	\ingroup Measurements
	\brief This class represents a round of ORIE (TORIE) measurements made from a single position of the instrument.
*/
struct TORIEROM {

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

    private:
        
        static int romCounter_;

        TLGCObsSummary orieSummary_;
};
#endif