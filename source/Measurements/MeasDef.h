#ifndef _MEAS_DEF_H_
#define _MEAS_DEF_H_

#include "TAMeas.h"
#include <bitset>

#include "TAdjustablePlane.h"
#include "TAdjustableAngle.h"
#include "TAdjustableHelmertTransformation.h"
#include "UEOIndices.h"

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
		inline MatrixIndex getLastEquationIndex()  { return getFirstEquationIndex() + 2;}			

		/// Returns the last observation index of this measurement (PLR3D introduces 3 observations, order is defined to be: theta, phi, s-distance). 
		inline MatrixIndex getLastObservationIndex()  {return fFirstObservationIndex + 2;}
			
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
			TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1>(pos, tgt) {}

		/// Returns the last LS-matrices equation index of this measurement, ANGL introduces 1 equation.
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
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
			TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1>(pos, tgt) {}

		/// Returns the last LS-matrices equation index of this measurement, ZEND introduces 1 equation.
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
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
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
};

//THIS IS NOT YET FINISHED, IT SHOULD INHERIT FROM THET TAScalarMeas, can be supplied with 0 as DVER or with the scale instrument!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/*! 
	\ingroup Measurements
	\brief Offset to a theodolit plane measurement (ECTH).
*/
class TECTH /*: public TAScalarMeas< ... >*/
{
	public:
		/// A copy of SCALE instrument with individual properties
		TInstrumentData::TSCALE scaleInstr;

		/// Line in the input file where this measurement was defined
		int line;

		/// DB comment after the measurement definition
		std::string eolcomment;

		/// Observed horizontal angle defining the reference plane
		TAngle obsHorAngle;	

		/// Point on which the SCALE instrument is positioned 
		const TAdjustablePoint* stationedPoint;

		/*!@name Constructors */
		//@{
			///See TAScalarMeas , is initialized with a single value
			TECTH(const TAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr,  TAngle obsHorAngle, TReal v);
		//@}

		/*!@name Access methods*/
		//@{

			/// Returns the last LS-matrices equation index of this measurement, 1 equation introduced.
			inline MatrixIndex getLastEquationIndex() const {return fFirstEquationIndex;}

			/// Returns the first LS-matrices equation index of this measurement.
			inline MatrixIndex getFirstEquationIndex() const {return fFirstEquationIndex;}

			/// Returns the first LS-matrices observation index of this measurement.
			inline MatrixIndex getFirstObservationIndex() const {return fFirstEquationIndex;}

			///Returns measured offset value
			inline TReal getMeasuredOffsetValue() const{return measuredOffset;}

			///Returns residual of the measured offset value
			inline TReal getMeasuredValueResidual() const{return residualOffset;}
		//@}

		/*!@name Settings */
		//@{
			/// Returns the first LS-matrices equation index of this measurement.
			inline void setFirstEquationIndex(MatrixIndex fEqIdx){fFirstEquationIndex = fEqIdx;}

			/// Returns the first LS-matrices equation index of this measurement.
			inline void setFirstObservationIndex(MatrixIndex fObsIdx){fFirstEquationIndex = fObsIdx;}

			///Sets the residual of the measured offset value
         inline void setMeasuredValueResidual(TLength resVal){ residualOffset = resVal; }
		//@}

	private:
		/// Index of the first equation introduced by this measurement in LS design matrices 
		MatrixIndex fFirstEquationIndex;

		/// Index of the first observation introduced by this measurement in LS design matrices 
		MatrixIndex fFirstObservationIndex;

		///Measured offset value
		TLength measuredOffset;

		///Residual of the measured value
      TLength residualOffset;
};
///////////////////////////////////////////////////////////////////////////
// TEDM 
///////////////////////////////////////////////////////////////////////////
/*! 
	\ingroup Measurements
	\brief Spatial distance measurement (DSPT), made by an electronic distance metes instrument (TInstrumentData::TEDM).
*/
class TDSPT : public TAScalarMeas<TInstrumentData::TEDM::TTarget> {
	public:
		/*!@name Constructors */
		//@{
			TDSPT(const TAdjustablePoint& pos, TInstrumentData::TEDM::TTarget tgt) : 
				TAScalarMeas<TInstrumentData::TEDM::TTarget>(pos, tgt) {}
         TDSPT(const TAdjustablePoint& pos, TInstrumentData::TEDM::TTarget tgt, TLength v) :
				TAScalarMeas<TInstrumentData::TEDM::TTarget>(pos, tgt, v) {}
		//@}

		/// Returns the last LS-matrices equation index of this measurement, DSPT introduces 1 equation.*/
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
};

///////////////////////////////////////////////////////////////////////////
// TSCALE measurements
///////////////////////////////////////////////////////////////////////////
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
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
};

///////////////////////////////////////////////////////////////////////////
// Levelling measurements
///////////////////////////////////////////////////////////////////////////
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
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}

		/// Returns the observed value.
		inline TReal getDistanceCorrection() const {return fDistanceCorrection;}
		
		/// Returns standard deviation of the observed value
		inline TReal getObservedStDev() const { return fSigmaObsVal;}
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
				inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}

				/// Returns the DHOR sigma
				inline TReal getDHORSigma() const{return dhorSigma;}

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
			inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
		//@}

			/// The optional DHOR measurment 
			shared_ptr<TDLEV::TDHOR> dhor;
};


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
			inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
		//@}
};

#endif