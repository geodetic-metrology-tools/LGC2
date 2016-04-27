#ifndef _MEAS_DEF_H_
#define _MEAS_DEF_H_

#include "TAMeas.h"
#include <bitset>
#include "UEOIndices.h"

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
		TPLR3D(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt);

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
		TANGL(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1>(pos, tgt), fAllFixedV0(TAngle(NO_VALf)) {}

		/// Returns the last LS-matrices equation index of this measurement, ANGL introduces 1 equation.
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}

		/// Allfixed parameter: V0
		TAngle fAllFixedV0;
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
		TZEND(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1>(pos, tgt), fAllFixedHi(NO_VALf) {}

		/// Returns the last LS-matrices equation index of this measurement, ZEND introduces 1 equation.
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}

		/// Allfixed parameter: instrument heigth - Hi
		TLength fAllFixedHi;
};

/*! 
	\ingroup Measurements
	\brief A generic straight line/distance measurement used for different observation types (DIST, DHOR).
*/
class TLINE : public TAScalarMeas<TInstrumentData::TPOLAR::TTarget> {
	public:
		/// See TAScalarMeas
		TLINE(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt);

		/// See TAScalarMeas , is initialized with a single value
		TLINE(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt, TLength v);

		/// Returns the last LS-matrices equation index of this measurement
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}

		/// Allfixed parameter: distance correction - Cs
		TLength fAllFixedCs;
		/// Allfixed parameter: instrument heigth - Hi
		TLength fAllFixedHi;
		
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
		TECTH(const TAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr, TAngle angle):
				TAScalarMeas<TInstrumentData::TSCALE>(stationedPoint, scaleInstr) { obsHorAngle = angle; }
		TECTH(const TAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr,  TAngle obsHorAngle, TLength v);
		//@}

		/// Returns the last LS-matrices equation index of this measurement, 1 equation introduced.
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}

		/// Allfixed parameter: V0
		TAngle fAllFixedV0;
};

/*!
\ingroup Measurements
\brief Offset to a spatial line measurement (ECSP).
*/
class TECSP : public TAScalarMeas<TInstrumentData::TSCALE>
{
public:
	/// Observed horizontal angle defining the reference plane
	TAngle obsHorAngle;
	/// Observed vertical angle defining the reference plane
	TAngle obsVertAngle;

	/*!@name Constructors */
	//@{
	TECSP(const TAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr, TAngle horAngle, TAngle vertAngle);
	TECSP(const TAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr, TAngle horAngle, TAngle vertAngle, TLength v);
	//@}

	/// Returns the last LS-matrices equation index of this measurement, 1 equation introduced.
	inline MatrixIndex getLastEquationIndex() const { return getFirstEquationIndex(); }

	/// Allfixed parameter: V0
	TAngle fAllFixedV0[2];
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
			TDSPT(const TAdjustablePoint& pos, TInstrumentData::TEDM::TTarget tgt) : 
				TAScalarMeas<TInstrumentData::TEDM::TTarget>(pos, tgt), fAllFixedCs(NO_VALf) {}
         TDSPT(const TAdjustablePoint& pos, TInstrumentData::TEDM::TTarget tgt, TLength v) :
			 TAScalarMeas<TInstrumentData::TEDM::TTarget>(pos, tgt, v), fAllFixedCs(NO_VALf) {}
		//@}

		/// Returns the last LS-matrices equation index of this measurement, DSPT introduces 1 equation.*/
		inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}

		/// Allfixed parameter: distance correction
		TLength fAllFixedCs;
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
		TECHO(const TAdjustablePoint& pos, TInstrumentData::TSCALE instr) : 
				TAScalarMeas<TInstrumentData::TSCALE>(pos, instr) {}
        TECHO(const TAdjustablePoint& pos, TInstrumentData::TSCALE instr, TLength v) :
				TAScalarMeas<TInstrumentData::TSCALE>(pos, instr, v) {}
		//@}

		/// Returns the last LS-matrices equation index of this measurement, TECHO introduces 1 equation.*/
		inline MatrixIndex getLastEquationIndex()const {return getFirstEquationIndex();}
};


/*!
\ingroup Measurements
\brief Offset to a vertical plane (ECVE) made by a scale instrument (TInstrumentData::TSCALE).
*/
class TECVE : public TAScalarMeas<TInstrumentData::TSCALE> {
public:
	/*!@name Constructors */
	//@{
	TECVE(const TAdjustablePoint& pos, TInstrumentData::TSCALE instr) :
		TAScalarMeas<TInstrumentData::TSCALE>(pos, instr) {}
	TECVE(const TAdjustablePoint& pos, TInstrumentData::TSCALE instr, TLength v) :
		TAScalarMeas<TInstrumentData::TSCALE>(pos, instr, v) {}
	//@}

	/// Returns the last LS-matrices equation index of this measurement, TECVE introduces 1 equation.*/
	inline MatrixIndex getLastEquationIndex()const { return getFirstEquationIndex(); }
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
		const TAdjustablePoint* station;

		/// Line in the input file where this measurement was defined
		int line;

		/// DB comment after the measurement definition
		std::string eolcomment;

	/*!@name Constructors */
	//@{
      TDVER(const TAdjustablePoint& station, const TAdjustablePoint& target, TLength obsVal);
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
            TDHOR(const TAdjustablePoint& pos, TInstrumentData::TLEVEL::TTarget tgt, TLength v);

				~TDHOR(){}

				/// Returns the last LS-matrices equation index of this measurement, DIST, DHOR introduce 1 equation.
				inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}

				/// Returns the DHOR sigma
				inline TLength getDHORSigma() const {return dhorSigma;}

				/// Stes DHOR sigma
            inline void setDHORSigma(TLength sigma){ dhorSigma = sigma; }

			private:
				/*Optinal horizontal distance sigma which can be assigned to a DHOR measurement inside DLEV.*/
				TLength dhorSigma;
		};

		virtual ~TDLEV(){}

		/*!@name Constructor*/
		//@{
      TDLEV(const TAdjustablePoint& pos, TInstrumentData::TLEVEL::TTarget target, TLength measValue);
		//@}

		/*!@name Access methods*/
		//@{
			/// Returns standard deviation of the observed value
			inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}
		//@}

			/// The optional DHOR measurment 
			shared_ptr<TDLEV::TDHOR> dhor;

			/// Allfixed parameter: collimation angle
			TAngle fAllFixedCollimation;
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
		TORIE(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAScalarMeas(pos, tgt) {}
		//@}

		/*!@name Access methods*/
		//@{
			/// Returns standard deviation of the observed value
			inline MatrixIndex getLastEquationIndex() const {return getFirstEquationIndex();}
		//@}
};

/*!
\ingroup Measurements
\brief  RADI constraints.
*/
class TRADI : public TAMeas<int>
{
public:
	///Pointer to the first point
	const TAdjustablePoint* station;

	/// Line in the input file where this measurement was defined
	int line;

	/// DB comment after the measurement definition
	std::string eolcomment;

	/*!@name Constructors */
	//@{
	TRADI(const TAdjustablePoint& point, TAngle obsVal, TLength sig = TLength(0.0));
	TRADI();
	//@}

	/*!@name Access methods*/
	//@{
	inline MatrixIndex getLastEquationIndex() const { return getFirstEquationIndex(); }

	/// Returns standard deviation of the observed value
	inline TLength getObservedStDev() const { return fSigmaObsVal; }

	/// Returns standard deviation of the observed value
	inline TLength getResidual() const { return fResidual; }

	/// Returns standard deviation of the observed value
	inline TAngle getAngleCnstr() const { return fAngleCnstr; }
	//@}


	/*!@name Settings */
	//@{
	/// Sets standard deviation of the observed value
	inline void setObservedStDev(TLength stDev){ fSigmaObsVal = stDev; }
	/// Returns standard deviation of the observed value
	inline void setResidual(TLength res) { fResidual = res; }
	//@}

private:
	// The angle constraint
	TAngle fAngleCnstr;
	// The residual angle constraint
	TLength fResidual;
	// Standard deviation of the observed value
	TLength fSigmaObsVal;
};
/*!
\ingroup Measurements
\brief   PDOR, which is also an orientation measurement.
*/
class TPdorObs : public TAMeas<int>{
public:
	///Pointer to the fixed point
	const TAdjustablePoint* calaPt;

	///Pointer to the orientation point
	const TAdjustablePoint* orientationPt;

	/// Line in the input file where this measurement was defined
	int line;

	/// DB comment after the measurement definition
	std::string eolcomment;

	/*!@name Constructors */
	//@{
	TPdorObs();
	TPdorObs(const TAdjustablePoint& cala, const TAdjustablePoint& orientation, TAngle gis);
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
	void Initialise(TAdjustablePoint& cala, TAdjustablePoint& ori, TAngle gis, bool hasbearing);

	/// Sets a residual of observed angle
	void setAngleResidual(const TAngle& a) { fbearingResidual = a; }
	//@}
	

private:
	// Standard deviation of the observed value
	TAngle fSigmaObsVal;
	TAngle fbearing;
	TAngle fbearingResidual;
	bool fIsInitialise;
	bool fDefined;
};
#endif