#ifndef _MEAS_DEF_H_
#define _MEAS_DEF_H_

#include "TAMeas.h"
#include <bitset>

#include "AdjustableObjects\TAdjustablePlane.h"
#include "AdjustableObjects\TAdjustableAngle.h"
#include "AdjustableObjects\TAdjustableHelmertTransformation.h"


///////////////////////////////////////////////////////////////////////////
// TTSTN/TCAM 
///////////////////////////////////////////////////////////////////////////
/// Two angle values of a PLR3D and DIR3D measurements
enum EPLR3DAngles {
	kANGL,  ///< horizontal direction
	kZEND  ///< zenith distance
};
/*! A PLR3D measurement
	Note that the observation indices go in order:
	fFirstObservationIndex (First) = THETA (ANGL)
	fFirstObservationIndex + 1 (Second) = PHI (ZEND)
	fFirstObservationIndex + 2 (Third) = DISTANCE s (DIST)
*/
class TPLR3D : public TAScalarMeas<TInstrumentData::TPOLAR::TTarget, 
			                ESingleValue, 1, 
			                EPLR3DAngles,  2> {
	public:
		/// See TAScalarMeas
		TPLR3D(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt);

		/*!Returns the last LS-matrices equation index of this measurement (PLR3D introduces 3 equations) */
		inline MatrixIndex getLastEquationIndex()  { return getFirstEquationIndex() + 2;}			

		/*! Returns the last observation index of this measurement (PLR3D introduces 3 observations, order is defined to be: theta, phi, s-distance). */
		inline MatrixIndex getLastObservationIndex()  {return fFirstObservationIndex + 2;}
				
	private:
		//An LS-matrices observatin index of a first observation in this measurement.
		MatrixIndex fFirstObservationIndex;
};

/// A DIR3D measurement
class TDIR3D : public TAScalarMeas<TInstrumentData::TPOLAR::TTarget, 
			                ESingleValue, 0, 
			                EPLR3DAngles,  2> {
	public:
		/// See TAScalarMeas
		TDIR3D(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt);

		/*!Returns the last LS-matrices equation index of this measurement (DIR3D introduces 3 equations) */
		inline MatrixIndex getLastEquationIndex() {return getFirstEquationIndex() + 2;}

		/*! Returns the last observation index of this measurement (DIR3D introduces 2 observations, order is defined to be: theta, phi). */
		inline MatrixIndex getLastObservationIndex() {return fFirstObservationIndex + 1;}

	private:
		//An LS-matrices observatin index of a first observation in this measurement.
		MatrixIndex fFirstObservationIndex;
};

		
/// A horizonthal angle measurement
class TANGL : public TAScalarMeas<TInstrumentData::TPOLAR::TTarget, 
			                ENoValues, 0,
							ESingleValue, 1> {
	public:
		/// See TAScalarMeas
		TANGL(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAScalarMeas(pos, tgt) {}

		/*!Returns the last LS-matrices equation index of this measurement, ANGL introduces 1 equation.*/
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
};

/// A horizonthal angle measurement
class TZEND : public TAScalarMeas<TInstrumentData::TPOLAR::TTarget, 
			                ENoValues, 0,
							ESingleValue, 1> {
	public:
		/// See TAScalarMeas
		TZEND(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAScalarMeas(pos, tgt) {}

		/*!Returns the last LS-matrices equation index of this measurement, ZEND introduces 1 equation.*/
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
};

///  A generic straight line/distance measurement
class TLINE : public TAScalarMeas<TInstrumentData::TPOLAR::TTarget> {
	public:
		/// See TAScalarMeas
		TLINE(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt);

		/// See TAScalarMeas , is initialized with a single value
		TLINE(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt, TReal v);

		/*!Returns the last LS-matrices equation index of this measurement, DIST, DHOR introduce 1 equation.*/
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
};

//This class is used for DHOR measurement defined in DLEV, because a different target type is used, for TSTN's DHOR measurements TLINE generic class is used
class TDHOR : public TAScalarMeas<TInstrumentData::TLEVEL::TTarget> {
	public:

		/// See TAScalarMeas , is initialized with a single value
		TDHOR(const TAdjustablePoint& pos, TInstrumentData::TLEVEL::TTarget tgt, TReal v);

		/*!Returns the last LS-matrices equation index of this measurement, DIST, DHOR introduce 1 equation.*/
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}

		inline TReal getDHORSigma() const{return dhorSigma;}

		inline void setDHORSigma(TReal sigma){dhorSigma = sigma;}

	private:
		/*Optinal horizontal distance sigma which can be assigned to a DHOR measurement inside DLEV.*/
		TReal dhorSigma;


};

///  Offset to a Theodolite plane
class TECTH 
{
	public:
		/// A copy of SCALE instrument with individual properties
		TInstrumentData::TSCALE scaleInstr;

		/// Line in the input file where this measurement was defined
		int line;

		/// DB comment after the measurement definition
		std::string eolcomment;

		//Observed horizontal angle defining the reference plane
		LGC::TAngle obsHorAngle;	

		//Point on which the SCALE instrument is positioned 
		const TAdjustablePoint* stationedPoint;

		/*!@name Constructors */
		//@{
			///See TAScalarMeas , is initialized with a single value
			TECTH(const TAdjustablePoint& stationedPoint, TInstrumentData::TSCALE scaleInstr,  LGC::TAngle obsHorAngle, TReal v);
		//@}

		/*!@name Access methods*/
		//@{

			/*!Returns the last LS-matrices equation index of this measurement, 1 equation introduced.*/
			inline MatrixIndex getLastEquationIndex() const {return fFirstEquationIndex;}

			/*!Returns the first LS-matrices equation index of this measurement.*/
			inline MatrixIndex getFirstEquationIndex() const {return fFirstEquationIndex;}

			/*!Returns the first LS-matrices observation index of this measurement.*/
			inline MatrixIndex getFirstObservationIndex() const {return fFirstEquationIndex;}

			///Returns measured offset value
			inline TReal getMeasuredOffsetValue() const{return measuredOffset;}

			///Returns residual of the measured offset value
			inline TReal getMeasuredValueResidual() const{return residualOffset;}
		//@}

		/*!@name Settings */
		//@{
			/*!Returns the first LS-matrices equation index of this measurement.*/
			inline void setFirstEquationIndex(MatrixIndex fEqIdx){fFirstEquationIndex = fEqIdx;}

			/*!Returns the first LS-matrices equation index of this measurement.*/
			inline void setFirstObservationIndex(MatrixIndex fObsIdx){fFirstEquationIndex = fObsIdx;}

			///Sets the residual of the measured offset value
			inline void setMeasuredValueResidual(TReal resVal){residualOffset = resVal;}
		//@}

	private:
		/// Index of the first equation introduced by this measurement in LS design matrices 
		MatrixIndex fFirstEquationIndex;

		MatrixIndex fFirstObservationIndex;

		///Measured offset value
		TReal measuredOffset;

		///Residual of the measured value
		TReal residualOffset;		
};
///////////////////////////////////////////////////////////////////////////
// TEDM 
///////////////////////////////////////////////////////////////////////////
// Specialisation of the abstract measurement: distance
class TDSPT : public TAScalarMeas<TInstrumentData::TEDM::TTarget> {
	public:
		TDSPT(const TAdjustablePoint& pos, TInstrumentData::TEDM::TTarget tgt) : 
			TAScalarMeas(pos, tgt) {}
		TDSPT(const TAdjustablePoint& pos, TInstrumentData::TEDM::TTarget tgt, TReal v) : 
			TAScalarMeas(pos, tgt, v) {}

		/*!Returns the last LS-matrices equation index of this measurement, DSPT introduces 1 equation.*/
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
};

///////////////////////////////////////////////////////////////////////////
// TSCALE measurements
///////////////////////////////////////////////////////////////////////////
// Specialisation of the abstract measurement: distance
class TECHO : public TAScalarMeas<TInstrumentData::TSCALE> {
	public:
		TECHO(const TAdjustablePoint& pos, TInstrumentData::TSCALE instr) : 
			TAScalarMeas(pos, instr) {}
		TECHO(const TAdjustablePoint& pos, TInstrumentData::TSCALE instr, TReal v) : 
			TAScalarMeas(pos, instr, v) {}

		/*!Returns the last LS-matrices equation index of this measurement, DSPT introduces 1 equation.*/
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
};

///////////////////////////////////////////////////////////////////////////
// Levelling measurements
///////////////////////////////////////////////////////////////////////////

// Vertical distance -- Geodetic Reference System
class TDVER {
	public:
		///Pointer to the first point
		const TAdjustablePoint* station;

		///Ponter to the second point
		const TAdjustablePoint* target;

		/// Line in the input file where this measurement was defined
		int line;

		/// DB comment after the measurement definition
		std::string eolcomment;

	/*!@name Constructors */
	//@{
		TDVER(const TAdjustablePoint& station, const TAdjustablePoint& target, TReal obsVal);
	//@}

	/*!@name Access methods*/
	//@{
		/*!Returns the first LS-matrices equation index of this measurement.*/
		inline MatrixIndex getFirstEquationIndex() const {return fFirstEquationIndex;}

		/*!Returns the first LS-matrices equation index of this measurement.*/
		inline MatrixIndex getFirstObservationIndex() const {return fFirstObservationIndex;}

		/*!Returns the last LS-matrices equation index of this measurement, DVER introduces 1 equation.*/
		inline MatrixIndex getLastEquationIndex() const {return fFirstEquationIndex;}

		/*!Returns the observed value.*/
		inline TReal getObservedValue() const {return fObservedValue;}

		/*!Returns the observed value.*/
		inline TReal getDistanceCorrection() const {return fDistanceCorrection;}

		/// Returns standard deviation of the observed value
		inline TReal getObservedStDev() const { return fSigmaObsVal;}
	//@}


	/*!@name Settings */
	//@{
		/*!Set first LS-matrices equation index of this measurement.*/
		inline void setFirstEquationIndex(int uidx){fFirstEquationIndex = uidx;}

		/*!Set first LS-matrices equation index of this measurement.*/
		inline void setFirstObservationIndex(int uidx){fFirstObservationIndex = uidx;}

		/*!Returns the observed value.*/
		inline void setDistanceCorrection(TReal distCorr) {fDistanceCorrection = distCorr;}

		/// Sets standard deviation of the observed value
		inline void setObservedStDev(TReal stDev){ fSigmaObsVal = stDev;}
	//@}

	private:
		// The observed vertical distance
		TReal fObservedValue;

		// The distance correction
		TReal fDistanceCorrection;


		// Standard deviation of the observed value
		TReal fSigmaObsVal;
		// Index of the first equation introduced by this measurement
		MatrixIndex fFirstEquationIndex;
		// Index of the first observation introduced by this measurement
		MatrixIndex fFirstObservationIndex;
};

class TDLEV : public TAScalarMeas<TInstrumentData::TLEVEL::TTarget> {
	public:
		TDLEV(const TAdjustablePoint& targetPos, TInstrumentData::TLEVEL::TTarget target, TReal measValue);

		/*!@name Access methods*/
		//@{

			/// Returns standard deviation of the observed value
			inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
		//@}

	private:
};

#endif