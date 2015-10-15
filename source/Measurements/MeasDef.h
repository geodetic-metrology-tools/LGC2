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
class TPLR3D : public TAMeas<TInstrumentData::TPOLAR::TTarget, 
			                ESingleValue, 1, 
			                EPLR3DAngles,  2> {
	public:
		/// See TAMeas
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
class TDIR3D : public TAMeas<TInstrumentData::TPOLAR::TTarget, 
			                ESingleValue, 0, 
			                EPLR3DAngles,  2> {
	public:
		/// See TAMeas
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
class TANGL : public TAMeas<TInstrumentData::TPOLAR::TTarget, 
			                ENoValues, 0,
							ESingleValue, 1> {
	public:
		/// See TAMeas
		TANGL(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAMeas(pos, tgt) {}

		/*!Returns the last LS-matrices equation index of this measurement, ANGL introduces 1 equation.*/
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
};

/// A horizonthal angle measurement
class TZEND : public TAMeas<TInstrumentData::TPOLAR::TTarget, 
			                ENoValues, 0,
							ESingleValue, 1> {
	public:
		/// See TAMeas
		TZEND(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt) : 
			TAMeas(pos, tgt) {}

		/*!Returns the last LS-matrices equation index of this measurement, ZEND introduces 1 equation.*/
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
};

///  A generic straight line/distance measurement
class TLINE : public TAMeas<TInstrumentData::TPOLAR::TTarget> {
	public:
		/// See TAMeas
		TLINE(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt);

		/// See TAmeas , is initialized with a single value
		TLINE(const TAdjustablePoint& pos, TInstrumentData::TPOLAR::TTarget tgt, TReal v);

		/*!Returns the last LS-matrices equation index of this measurement, DIST, DHOR introduce 1 equation.*/
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}
};

//This class is used for DHOR measurement defined in DLEV, because a different target type is used, for TSTN's DHOR measurements TLINE generic class is used
class TDHOR : public TAMeas<TInstrumentData::TLEVEL::TTarget> {
	public:

		/// See TAmeas , is initialized with a single value
		TDHOR(const TAdjustablePoint& pos, TInstrumentData::TLEVEL::TTarget tgt, TReal v);

		/*!Returns the last LS-matrices equation index of this measurement, DIST, DHOR introduce 1 equation.*/
		inline MatrixIndex getLastEquationIndex(){return getFirstEquationIndex();}

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
			///See TAmeas , is initialized with a single value
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
class TDSPT : public TAMeas<TInstrumentData::TEDM::TTarget> {
	public:
		TDSPT(const TAdjustablePoint& pos, TInstrumentData::TEDM::TTarget tgt) : 
			TAMeas(pos, tgt) {}
		TDSPT(const TAdjustablePoint& pos, TInstrumentData::TEDM::TTarget tgt, TReal v) : 
			TAMeas(pos, tgt, v) {}

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
		const TAdjustablePoint* pointA;

		///Ponter to the second point
		const TAdjustablePoint* pointB;

		/// Line in the input file where this measurement was defined
		int line;

		/// DB comment after the measurement definition
		std::string eolcomment;

	/*!@name Constructors */
	//@{
		TDVER(const TAdjustablePoint& pointA, const TAdjustablePoint& pointB, TReal obsVal);
	//@}

	/*!@name Access methods*/
	//@{
		/*!Returns the first LS-matrices equation index of this measurement.*/
		inline MatrixIndex getFirstEquationIndex() const {return fFirstEquationIndex;}

		/*!Returns the first LS-matrices equation index of this measurement.*/
		inline MatrixIndex getFirstObservationIndex() const {return fFirstObservationIndex;}

		/*!Returns the last LS-matrices equation index of this measurement, DVER introduces 1 equation.*/
		inline MatrixIndex getLastEquationIndex() const {return fFirstEquationIndex;}

		/// Returns standard deviation of the observed value
		inline TReal getObservedStDev() const { return fSigmaObsVal;}
	//@}


	/*!@name Settings */
	//@{
		/*!Set first LS-matrices equation index of this measurement.*/
		inline void setFirstEquationIndex(int uidx){fFirstEquationIndex = uidx;}

		/*!Set first LS-matrices equation index of this measurement.*/
		inline void setFirstObservationIndex(int uidx){fFirstObservationIndex = uidx;}

		/// Sets standard deviation of the observed value
		inline void setObservedStDev(TReal stDev){ fSigmaObsVal = stDev;}
	//@}

	private:
		// The observed vertical distance
		TReal fObservedValue;
		// Standard deviation of the observed value
		TReal fSigmaObsVal;
		// Index of the first equation introduced by this measurement
		MatrixIndex fFirstEquationIndex;
		// Index of the first observation introduced by this measurement
		MatrixIndex fFirstObservationIndex;
};

class TDLEV {
	public:
		TDLEV(const TAdjustablePoint& targetPos, TInstrumentData::TLEVEL::TTarget target, TReal measValue);
		/// The levelling staff, which is a target
		TInstrumentData::TLEVEL::TTarget     target;

		/// Stationed point of the levelling staff (target)
		const TAdjustablePoint*  targetPos; 

		/// Line in the input file where this measurement was defined
		int line;

		/*!@name Access methods*/
		//@{
			/*!Returns the first LS-matrices equation index of this measurement.*/
			inline MatrixIndex getEquationIndex() const {return fEquationIndex;}

			/*!Returns the first LS-matrices equation index of this measurement.*/
			inline MatrixIndex getObservationIndex() const {return fObservationIndex;}

			/// Returns standard deviation of the observed value
			inline TReal getHorizontalDistanceSigma() const{ return fSigmaHorDistVal;}

			inline TReal getMeasuredVerticalDistance() const{ return fMeasuredValue;}

			inline TReal getMeasuredHorizontalDistance() const{ return fHorDistance;}

			inline TReal getVertDistResidual() const{ return fVertDstObsResidual;}
		//@}

		/*!@name Settings */
		//@{
			/*!Set LS-matrices equation index of this measurement.*/
			inline void setEquationIndex(int uidx){fEquationIndex = uidx;}

			/*!Set LS-matrices observation index of this measurement.*/
			inline void setObservationIndex(int uidx){fObservationIndex = uidx;}

			void setHorizontalDistance(TReal dist) {fHorDistance = dist;}

			void setHorizontalDistanceSigma(TReal sigma) {fSigmaHorDistVal = sigma;}

			void setVertDistResidual(TReal residual) {fVertDstObsResidual = residual;}
		//@}	

	private:
		// Observed vertical distance
		TReal fMeasuredValue;
		// Horizontal distance value - between the reference point (instrument) and the stationed point (scale)
		TReal fHorDistance;
		// Standard deviation of the observed horizontal distance value
		TReal fSigmaHorDistVal;

		//Residual of the observed vertical distance
		TReal fVertDstObsResidual;

		// Equation index in LS design matrices of this measurement
		MatrixIndex fEquationIndex;
		// Observation index in LS design matrices of this measurement
		MatrixIndex fObservationIndex;
};
///////////////////////////////////////////////////////////////////////////
// FRAME and POINT measurements.  THESE MEASUREMENTS ARE NOT NEEDED NOW, SINCE THE STANDARD DEVIATION ARE APPLIED IN WEIGHT MARTRIX. LEFT IF NEEDED IN A FUTURE
#if 0
///////////////////////////////////////////////////////////////////////////
class TFrameMeas {
	public:

	/// Pointer to the observed frame
	const TAdjustableHelmertTransformation* fFrameMeas; 
	
	/// Line in the input file where this frame measurement was introduced
	int line;

	/*!@name Constructors */
	//@{
		TFrameMeas(const TAdjustableHelmertTransformation& observedFrame);
	//@}

	/*!@name Access methods*/
	//@{
		/// Allowed values are 0(TX), 1(TY) and 2(TZ)
		inline TReal getTranslationSigma(int axis) const {
			assert3D(axis);
			return (!fFrameMeas->fixedTranslations[axis]) ? fSigmaTranslations[axis] : throw std::logic_error("Trying to access standard deviation of a translation which is not assigned.");
		}

		/// Allowed values are 0(RX), 1(RY) and 2(RZ)
		inline TReal getRotationSigma(int axis) const {
			assert3D(axis);
			return (!fFrameMeas->fixedRotations[axis]) ? fSigmaRotations[axis] : throw std::logic_error("Trying to access standard deviation of a rotation which is not assigned.");
		}

		///Return the scale's standard deviation
		inline TReal getScaleSigma() const {
			return (!fFrameMeas->fixedScale[0]) ? fScaleSigma : throw std::logic_error("Trying to access standard deviation of a scale which is not assigned.");
		}

		/*!Returns the first LS-matrices equation index of this measurement.*/
		inline MatrixIndex getFirstEquationIndex() const {
			 return getNumOfEquations()>0 ? fFirstEquationIndex : throw std::logic_error("This frame measurement does not introduce any equation.");
		}

		/*!Returns the last LS-matrices equation index of this measurement, FRAME measurement introduces up to 7 equations.*/
		inline MatrixIndex getLastEquationIndex() const {
			int nEq = getNumOfEquations();
		    return getNumOfEquations()>0 ? fFirstEquationIndex + nEq - 1 : throw std::logic_error("This frame measurement does not introduce any equation.");
		}

		///Returns the number of equations introduced in this measurement
		inline int getNumOfEquations() const {
			return fSigmaTranslationsState.count() + fSigmaRotationsState.count() + fScaleSigmaState.count();
		}

		///Returns the number of observations introduced in this measurement, it is equal to the number of equations
		inline int getNumOfObservations() const { return getNumOfEquations();}
	//@}


	/*!@name Settings */
	//@{
		/// Sets the standard deviation of a rotation around X(0), Y(1) or Z(2) axis
		void setRotationSigma(int axis, TReal stDev);

		/// Sets the standard deviation of a translation around X(0), Y(1) or Z(2) axis
		void setTranslationSigma(int axis, TReal stDev);

		/// Sets the standard deviation of a scale factor
		void setScaleSigma(TReal stDev);

		/*! Sets the first observation index of this measurement, FRAME measurement introduces up to 7 equations. */
		inline void setFirstEquationIndex(MatrixIndex firstEquationIndex){fFirstEquationIndex = firstEquationIndex;}

	//@}

	private:
		TReal fSigmaTranslations[3];
		TReal fSigmaRotations[3];
		TReal fScaleSigma;

		//Say whether particular part has assigned standard deviation or not
		std::bitset<3> fSigmaTranslationsState;
		std::bitset<3> fSigmaRotationsState;
		std::bitset<1> fScaleSigmaState;

		/// Index of first equation introduced by this measurement in LS design matrices 
		MatrixIndex fFirstEquationIndex;
};

class TPointMeas {
	public:

	/// Pointer to the observed point
	const TAdjustablePoint* fPointMeas; 

	/*!@name Constructors */
	//@{
		TPointMeas(const TAdjustablePoint& measuredPoint, TReal stDevX, TReal stDevY, TReal stDevZ);
	//@}

	/*!@name Access methods*/
	//@{
		/// Allowed values are 0(X), 1(Y) and 2(Z)
		inline TReal getStandardDeviation(int idx) const {
			assert3D(idx);
			return fStandardDeviations[idx];
		}

		/*! Returns the first observation index of this point measurement. */
		inline MatrixIndex getFirstEquationIndex() const { return fFirstEquationIndex;}

		/*!Returns the last LS-matrices equation index of this measurement, POINT measurement introduces 3 equations. */
		inline MatrixIndex getLastEquationIndex() const { return fFirstEquationIndex + 2;}
	//@}


	/*!@name Settings */
	//@{
		/*! Sets the first observation index of this measurement, POINT measurement introduces 3 equations. */
		inline void setFirstEquationIndex(MatrixIndex firstEquationIndex){fFirstEquationIndex = firstEquationIndex;}
	//@}

	private:
		TReal fStandardDeviations[3];

		/// Index of first equation introduced by this measurement in LS design matrices 
		MatrixIndex fFirstEquationIndex;
};
#endif

#endif