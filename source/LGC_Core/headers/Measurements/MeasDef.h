/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _MEAS_DEF_H_
#define _MEAS_DEF_H_

//STL
#include <bitset>
//SURVEYLIB
#include <UEOIndices.h>
#include <TPositionVector.h>
//LGC
#include <TAMeas.h>


//--------------------------  TSTN measurement--------------------------------------------


/// Two angle values of a PLR3D measurement
enum EPLR3DAngles {
	kANGL,  ///< horizontal direction
	kZEND  ///< zenith distance
};

/*! 
	\ingroup Measurements

	\brief PLR3D observation made by a total station (TSTSN). It is a composition of ANGL, ZEND and DIST observations.

	Note that the observation indices go in order:

	fFirstObservationIndex (First) = THETA (ANGL)

	fFirstObservationIndex + 1 (Second) = PHI (ZEND)

	fFirstObservationIndex + 2 (Third) =  s (DIST)
*/
class TPLR3D : public TAScalarMeas<TInstrumentData::TPOLAR::TTarget, 
								   ESingleValue, 1, 
								   EPLR3DAngles, 2> {
	public:
		/// See TAScalarMeas
		TPLR3D(const LGCAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt);

		/// Returns the last LS-matrices equation index of this measurement (PLR3D introduces 3 equations) 
		inline MatrixIndex getLastEquationIndex() const { return getFirstEquationIndex() + 2;}			

		/// Returns the last observation index of this measurement (PLR3D introduces 3 observations, order is defined to be: theta, phi, s-distance). 
		inline MatrixIndex getLastObservationIndex() const {return fFirstObservationIndex + 2;}
			
		/// Allfixed parameter: instrument heigth - Hi
		TLength fAllFixedHi;
		/// Allfixed parameter: distance correction - Cs
		TLength fAllFixedCs;
		/// Allfixed parameter: V0
		TAngle fAllFixedV0[2];
		/// Allfixed parameter: Rx
		TAngle fAllFixedRx[2];
		/// Allfixed parameter: Ry
		TAngle fAllFixedRy[2];


#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const;
#endif
		
};
		
/*! 
	\ingroup Measurements
	\brief Horizontal angle observation (ANGL) made by a total station (TTSTN).
*/
class TANGL : public TAScalarMeas<TInstrumentData::TPOLAR::TTarget, 
								  ENoValues, 0,
								  ESingleValue, 1> {
	public:
		/// See TAScalarMeas
		TANGL(const LGCAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1>(pos, tgt), fAllFixedV0(TAngle(NO_VALf)) {}

		/// Returns the last LS-matrices equation index of this measurement, ANGL introduces 1 equation.
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}

		/// Allfixed parameter: V0
		TAngle fAllFixedV0;

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const;
#endif
};

/*! 
	\ingroup Measurements
	\brief Zenith distance observation (ZEND) made by a total station (TTSTN).
*/
class TZEND : public TAScalarMeas<TInstrumentData::TPOLAR::TTarget, 
			                ENoValues, 0,
							ESingleValue, 1> {
	public:
		/// See TAScalarMeas
		TZEND(const LGCAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1>(pos, tgt), fAllFixedHi(NO_VALf) {}

		/// Returns the last LS-matrices equation index of this measurement, ZEND introduces 1 equation.
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}

		/// Allfixed parameter: instrument heigth - Hi
		TLength fAllFixedHi;

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const;
#endif
};

/*! 
	\ingroup Measurements
	\brief A generic straight line/distance measurement used for different observation types (DIST, DHOR).
*/
class TLINE : public TAScalarMeas<TInstrumentData::TPOLAR::TTarget> {
	public:
		/// See TAScalarMeas
		TLINE(const LGCAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt);

		/// See TAScalarMeas , is initialized with a single value
		TLINE(const LGCAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt, TLength v);

		/// Returns the last LS-matrices equation index of this measurement
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}

		/// Allfixed parameter: distance correction - Cs
		TLength fAllFixedCs;
		/// Allfixed parameter: instrument heigth - Hi
		TLength fAllFixedHi;

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const;
#endif
};

/*! 
	\ingroup Measurements
	\brief Offset to a theodolit plane measurement (ECTH).
*/
class TECTH : public TAScalarMeas<TInstrumentData::TSCALE>
{
	public:
		/// Observed horizontal angle defining the reference plane
		TAngle obsHorAngle;	

		/*!@name Constructors */
		//@{
		TECTH(const LGCAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr, TAngle angle):
				TAScalarMeas<TInstrumentData::TSCALE>(stationedPoint, scaleInstr) { obsHorAngle = angle; }
		TECTH(const LGCAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr,  TAngle obsHorAngle, TLength v);
		//@}

		/// Returns the last LS-matrices equation index of this measurement, 1 equation introduced.
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}

		/// Allfixed parameter: V0
		TAngle fAllFixedV0;

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const;
#endif
};
/*!
\ingroup Measurements
\brief Offset to a theodolit plane measurement (ECTH).
*/
class TECDIR : public TAScalarMeas<TInstrumentData::TSCALE>
{
public:
	/// Observed horizontal angle defining the reference plane
	TAngle obsHorAngle;
	/// Observed vertical angle defining the reference plane
	TAngle obsVertAngle;

	/*!@name Constructors */
	//@{
	TECDIR(const LGCAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr, TAngle HorAngle, TAngle VerAngle);
	TECDIR(const LGCAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr, TAngle obsHorAngle, TAngle obsVerAngle, TLength v);
	//@}

	/// Returns the last LS-matrices equation index of this measurement, 1 equation introduced.
	inline MatrixIndex getLastEquationIndex() const { return getFirstEquationIndex(); }

	/// Allfixed parameter: V0
	TAngle fAllFixedV0[2];

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const;
#endif
};

/*!
\ingroup Measurements
\brief Offset to a spatial line measurement (ECSP).
*/
class TECSP : public TAScalarMeas<TInstrumentData::TSCALE>
{
public:
	/*!@name Constructors */
	//@{
	TECSP(const LGCAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr);
	TECSP(const LGCAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr, TLength v);
	//@}

	/// Returns the last LS-matrices equation index of this measurement, 1 equation introduced.
	inline MatrixIndex getLastEquationIndex() const { return getFirstEquationIndex(); }

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override { 
		TAScalarMeas::serialize(obj); 
	}
#endif
};

//--------------------------  EDM measurement--------------------------------------------
/*! 
	\ingroup Measurements
	\brief Spatial distance measurement (DSPT), made by an electronic distance metes instrument (TInstrumentData::TEDM).
*/
class TDSPT : public TAScalarMeas<TInstrumentData::TEDM::TTarget> {
	public:
		/*!@name Constructors */
		//@{
			TDSPT(const LGCAdjustablePoint& pos, TInstrumentData::TEDM::TTarget tgt) : 
				TAScalarMeas<TInstrumentData::TEDM::TTarget>(pos, tgt), fAllFixedCs(NO_VALf) {}
         TDSPT(const LGCAdjustablePoint& pos, TInstrumentData::TEDM::TTarget tgt, TLength v) :
			 TAScalarMeas<TInstrumentData::TEDM::TTarget>(pos, tgt, v), fAllFixedCs(NO_VALf) {}
		//@}

		/// Returns the last LS-matrices equation index of this measurement, DSPT introduces 1 equation.*/
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}

		/// Allfixed parameter: distance correction
		TLength fAllFixedCs;

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const;
#endif
};

//--------------------------  SCALE measurement--------------------------------------------
/*! 
	\ingroup Measurements
	\brief Offset to a vertical plane (ECHO) made by a scale instrument (TInstrumentData::TSCALE).
*/
class TECHO : public TAScalarMeas<TInstrumentData::TSCALE> {
	public:
		/*!@name Constructors */
		//@{
		TECHO(const LGCAdjustablePoint& pos, TInstrumentData::TSCALE instr) : 
				TAScalarMeas<TInstrumentData::TSCALE>(pos, instr) {}
        TECHO(const LGCAdjustablePoint& pos, TInstrumentData::TSCALE instr, TLength v) :
				TAScalarMeas<TInstrumentData::TSCALE>(pos, instr, v) {}
		//@}

		/// Returns the last LS-matrices equation index of this measurement, TECHO introduces 1 equation.*/
		inline MatrixIndex getLastEquationIndex()const {return getFirstEquationIndex();}

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(ObjectSerializer &obj) const override { 
			TAScalarMeas::serialize(obj); 
		}
#endif
};


/*!
\ingroup Measurements
\brief Offset to a vertical plane (ECVE) made by a scale instrument (TInstrumentData::TSCALE).
*/
class TECVE : public TAScalarMeas<TInstrumentData::TSCALE> {
public:
	/*!@name Constructors */
	//@{
	TECVE(const LGCAdjustablePoint& pos, TInstrumentData::TSCALE instr) :
		TAScalarMeas<TInstrumentData::TSCALE>(pos, instr) {}
	TECVE(const LGCAdjustablePoint& pos, TInstrumentData::TSCALE instr, TLength v) :
		TAScalarMeas<TInstrumentData::TSCALE>(pos, instr, v) {}
	//@}

	/// Returns the last LS-matrices equation index of this measurement, TECVE introduces 1 equation.*/
	inline MatrixIndex getLastEquationIndex()const { return getFirstEquationIndex(); }

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override { 
		TAScalarMeas::serialize(obj); 
	}
#endif
};

//--------------------------  Levelling measurement--------------------------------------------
/*! 
	\ingroup Measurements
	- DVER has no Target supplied with 0
	\brief  Vertical distance -- Geodetic Reference System.
*/
class TDVER  : public TAScalarMeas<int>{
	public:
		///Pointer to the first point
		const LGCAdjustablePoint* station;

	/*!@name Constructors */
	//@{
      TDVER(const LGCAdjustablePoint& station, const LGCAdjustablePoint& target, TLength obsVal);
	//@}

	/*!@name Access methods*/
	//@{
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}

		/// Returns the observed value.
		inline TLength getDistanceCorrection() const {return fDistanceCorrection;}
		
		/// Returns standard deviation of the observed value
		inline TLength getObservedStDev() const { return fSigmaObsVal;}
	//@}


	/*!@name Settings */
	//@{
		/// Returns the observed value.*/
      inline void setDistanceCorrection(TLength distCorr) { fDistanceCorrection = distCorr; }

		/// Sets standard deviation of the observed value
      inline void setObservedStDev(TLength stDev){ fSigmaObsVal = stDev; }
	//@}

#if USE_SERIALIZER
	  // Inherited via Serializable
	  virtual void serialize(ObjectSerializer &obj) const;
#endif

	private:
		// The distance correction
		TLength fDistanceCorrection;
		// Standard deviation of the observed value
		TLength fSigmaObsVal;
};

/*! 
	\ingroup Measurements
	\brief  Vertical distance measurement.
*/
class TDLEV : public TAScalarMeas<TInstrumentData::TLEVEL::TTarget> {
	public:
		///Nested class of TDLEV for an optional DHOR measurement
		class TDHOR : public TAScalarMeas<TInstrumentData::TLEVEL::TTarget> { 
			public:

				/// See TAScalarMeas , is initialized with a single value
            TDHOR(const LGCAdjustablePoint& pos, TInstrumentData::TLEVEL::TTarget tgt, TLength v);

				~TDHOR(){}

				/// Returns the last LS-matrices equation index of this measurement, DIST, DHOR introduce 1 equation.
				inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}

				/// Returns the DHOR sigma
				inline TLength getDHORSigma() const {return dhorSigma;}

				/// Stes DHOR sigma
            inline void setDHORSigma(TLength sigma){ dhorSigma = sigma; }

			#if USE_SERIALIZER
						// Inherited via Serializable
						virtual void serialize(ObjectSerializer &obj) const;
			#endif

			private:
				/*Optinal horizontal distance sigma which can be assigned to a DHOR measurement inside DLEV.*/
				TLength dhorSigma;
		};

		virtual ~TDLEV(){}

		/*!@name Constructor*/
		//@{
      TDLEV(const LGCAdjustablePoint& pos, TInstrumentData::TLEVEL::TTarget target, TLength measValue);
		//@}

		/*!@name Access methods*/
		//@{
			/// Returns standard deviation of the observed value
			inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}
		//@}

			/// The optional DHOR measurment 
			std::shared_ptr<TDLEV::TDHOR> dhor;

			/// Allfixed parameter: collimation angle
			TAngle fAllFixedCollimation;

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(ObjectSerializer &obj) const;
#endif
};

//--------------------------  Orientation measurement--------------------------------------------
/*! 
	\ingroup Measurements
	\brief  Gyro-Theodolite Azimuth (ORIE), also used for PDOR, which is also an ORIE measurement.
*/
class TORIE : public TAScalarMeas<TInstrumentData::TPOLAR::TTarget, 
			                         ENoValues, 0,
							             ESingleValue, 1> 
{
	public:
		/*!@name Constructor*/
		//@{
		TORIE(const LGCAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAScalarMeas(pos, tgt) {}
		//@}

		/*!@name Access methods*/
		//@{
			/// Returns standard deviation of the observed value
			inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}
		//@}

#if USE_SERIALIZER
			// Inherited via Serializable
			virtual void serialize(ObjectSerializer &obj) const override { 
				TAScalarMeas::serialize(obj); 
			}
#endif
};

/*!
\ingroup Measurements
\brief  RADI constraints.
*/
class TRADI : public TAMeas<int>
{
public:
	///Pointer to the first point
	const LGCAdjustablePoint* station;

	/*!@name Constructors */
	//@{
	TRADI(const LGCAdjustablePoint& point, TAngle obsVal, TLength sig = TLength(0.0));
	TRADI();
	//@}

	/*!@name Access methods*/
	//@{
	inline MatrixIndex getLastEquationIndex() const { return getFirstEquationIndex(); }

	/// Return the standard deviation of the observed value
	inline TLength getObservedStDev() const { return fSigmaObsVal; }

	/// Return the constant value
	inline TAngle getConstAngle() const { return fConstAngleVal; }

	/// Return the residual angle constraint
	inline TLength getResidual() const { return fResidual; }

	/// Return the angle constraint value
	inline TAngle getAngleCnstr() const { return fAngleCnstr; }
	//@}


	/*!@name Settings */
	//@{
	/// Set the standard deviation of the observed value
	inline void setObservedStDev(TLength stDev){ fSigmaObsVal = stDev; }
	/// Set the constant angle
	inline void setConstAngle(TAngle constAngle) { fConstAngleVal = constAngle; }
	/// Set the residual angle constraint
	inline void setResidual(TLength res) { fResidual = res; }
    /// Set the angle constraint value
    inline void setAngleCnstr(TAngle angl) { fAngleCnstr = angl; }
	//@}

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const;
#endif

private:
	// The angle constraint
	TAngle fAngleCnstr;
	// The residual angle constraint
	TLength fResidual;
	// Standard deviation of the observed value
	TLength fSigmaObsVal;
	// Constant angle
	TAngle fConstAngleVal{0};
};
/*!
\ingroup Measurements
\brief   PDOR, which is also an orientation measurement.
*/
class TPdorObs : public TAMeas<int>{
public:
	///Pointer to the fixed point
	const LGCAdjustablePoint* calaPt;

	///Pointer to the orientation point
	const LGCAdjustablePoint* orientationPt;

	/*!@name Constructors */
	//@{
	TPdorObs();
	TPdorObs(const LGCAdjustablePoint& cala, const LGCAdjustablePoint& orientation, TAngle gis);
	//@}

	/*!@name Access methods*/
	//@{
	inline MatrixIndex getLastEquationIndex() const { return getFirstEquationIndex(); }

	/// Return if the pdor measurement is initialise or not
	const bool isInitialised()const { return fIsInitialise; }

	/// Return if the pdor measurement is initialise or not
	const bool isBearingDefined()const { return fDefined; }

	/// Return if the pdor measurement is initialise or not
	const TAngle getBearing()const { return fbearing; }

	/// Return if the pdor measurement is initialise or not
	const TAngle& getSigma()const { return fSigmaObsVal; }

	/// Returns residual of the observed angle
	const TAngle& getAngleResidual()const { return fbearingResidual; }

	//@}


	/*!@name Settings */
	//@{

	/// Initialise the measurement
	void Initialise(LGCAdjustablePoint& cala, LGCAdjustablePoint& ori, TAngle gis, bool hasbearing);

	/// Sets a residual of observed angle
	void setAngleResidual(const TAngle& a) { fbearingResidual = a; }
	//@}
	
#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const;
#endif

private:
	// Standard deviation of the observed value
	TAngle fSigmaObsVal;
	TAngle fbearing;
	TAngle fbearingResidual;
	bool fIsInitialise;
	bool fDefined;
};

/*!
\ingroup Measurements
\brief  RADI constraints.
*/
class TOBSXYZ : public TAMeas<int>
{
public:
	///Pointer to the point
	const LGCAdjustablePoint* station;
	//Position of the point in the subframe (= observation)
	TPositionVector obsValue;
	TDataTreeIterator positionInTree;

	/*!@name Constructors */
	//@{
	TOBSXYZ(const LGCAdjustablePoint& point, TPositionVector pos, TLength sigX, TLength sigY, TLength sigZ, TDataTreeIterator itTree);
	TOBSXYZ();
	//@}

	/*!@name Access methods*/
	//@{
	inline MatrixIndex getLastEquationIndex() const { return getFirstEquationIndex()+2; }

	/// Returns standard deviation of the observed value
	inline TLength getXObservedStDev() const { return fXSigmaObsVal; }
	inline TLength getYObservedStDev() const { return fYSigmaObsVal; }
	inline TLength getZObservedStDev() const { return fZSigmaObsVal; }

	/// Returns standard deviation of the observed value
	inline TLength getXResidual() const { return fXResidual; }
	inline TLength getYResidual() const { return fYResidual; }
	inline TLength getZResidual() const { return fZResidual; }
	//@}


	/*!@name Settings */
	//@{
	/// Sets standard deviation of the observed value
	inline void setXObservedStDev(TLength stDev){ fXSigmaObsVal = stDev; }
	inline void setYObservedStDev(TLength stDev){ fYSigmaObsVal = stDev; }
	inline void setZObservedStDev(TLength stDev){ fZSigmaObsVal = stDev; }
	/// Returns standard deviation of the observed value
	inline void setXResidual(TLength res) { fXResidual = res; }
	inline void setYResidual(TLength res) { fYResidual = res; }
	inline void setZResidual(TLength res) { fZResidual = res; }
	//@}

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const;
#endif

private:
	// The residual angle constraint
	TLength fXResidual;
	TLength fYResidual;
	TLength fZResidual;
	// Standard deviation of the observed value
	TLength fXSigmaObsVal;
	TLength fYSigmaObsVal;
	TLength fZSigmaObsVal;
};

//--------------------------  INCL measurement--------------------------------------------
/*!
	\ingroup Measurements
	\brief Angle measurement with respect to local vertical (INCLY) made by an inclinometer instrument (TInstrumentData::TINCL).
*/
class TINCLY : public TAScalarMeas<TInstrumentData::TINCL, ENoValues, 0,
	ESingleValue, 1>
{
public:


	/*!@name Constructors */
	//@{

	TINCLY(const LGCAdjustablePoint& pos, TInstrumentData::TINCL tgt) :
		TAScalarMeas<TInstrumentData::TINCL, ENoValues, 0, ESingleValue, 1>(pos, tgt){}
	//@}

	///Destructor
	~TINCLY() override = default;

	/// Returns the last LS-matrices equation index of this measurement, 1 equation introduced.
	MatrixIndex getLastEquationIndex() const { return getFirstEquationIndex(); }

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override { 
		TAScalarMeas::serialize(obj);
	}
#endif
};

//--------------------------  HLSR measurement--------------------------------------------


class TECWS : public TAScalarMeas<TInstrumentData::THLSR> {
public:
	/*!@name Constructors */
	//@{
	TECWS(const LGCAdjustablePoint& station, TInstrumentData::THLSR instr, TLength obsVal) :
		TAScalarMeas<TInstrumentData::THLSR>(station, instr, obsVal) {}

	///Destructor
	~TECWS() override = default;
	//@}

	/// Returns the last LS-matrices equation index of this measurement, 1 equation introduced.
	inline MatrixIndex getLastEquationIndex() const { return getFirstEquationIndex(); }

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const;
#endif
};


//--------------------------  WPSR measurement--------------------------------------------

/// Two distance values of a ECWI measurement
enum EECWIDistances
{
	kX, ///< Distance along the X-Axis in the station's frame
	kZ ///< Distance along the Z-Axis in the station's frame
};

/*!
	\ingroup Measurements
	\brief 2 Distance measurements made by a Wire Positionning Sensor instrument (TInstrumentData::TWPSR).
*/
class TECWI : public TAScalarMeas<TInstrumentData::TWPSR, EECWIDistances, 2, ENoValues, 0>
{
public:
	/*!@name Constructors */
	//@{
	TECWI(const LGCAdjustablePoint &station, TInstrumentData::TWPSR instr) : TAScalarMeas<TInstrumentData::TWPSR, EECWIDistances, 2, ENoValues, 0>(station, instr){};

	/// Destructor
	~TECWI() override = default;
	//@}

	/// Returns the last LS-matrices equation index of this measurement (ECWI introduces 2 equations)
	inline MatrixIndex getLastEquationIndex() const { return getFirstEquationIndex() + 1; }

	/// Returns the last observation index of this measurement (ECWI introduces 2 observations, order is defined to be: X, Z).
	inline MatrixIndex getLastObservationIndex() const { return fFirstObservationIndex + 1; }

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const;
#endif

};

#endif
