#ifndef _TA_ANGLE_MEAS_H_
#define _TA_ANGLE_MEAS_H_

#include <TInstrumentData.h>
#include <array>
#include <TFreeVector.h>

class TAdjustablePoint;

/// Enum for the case that no value of a certain type is measured in \ref TAScalarMeas.
enum ENoValues {};
/// Enum for the case that only a single value of a certain type is measured in \ref TAScalarMeas .
enum ESingleValue {
	kValue ///< Symbolic access to a single value in a measurement
};
	

/*!
	Interface for measurement values. It contains equation & observations index and the desired target.

	\tparam TTarget Target type of the measurement. The measurement receives a deep copy
					of the observed target. May be set to int and supplied with 0 if no target is used.
*/
template<typename TTarget>
class TAMeas {
	
	protected:

		/// Index of first equation introduced by this measurement in LS design matrices 
		MatrixIndex fFirstEquationIndex;
		/// Index of first equation introduced by this measurement in LS design matrices 
		MatrixIndex fFirstObservationIndex;

	public:

		/// A copy of the target with individual properties
		typename TTarget target;
			
		/// Pointer to the observed point
		const TAdjustablePoint* targetPos; 

		/// Line in the input file where this measurement was defined
		int line;

		/// DB comment after the measurement definition
		std::string eolcomment;
				
		/*!
			Constructs an uninitialized measurement

			\param targetPos The observed point
			\param tgt The observed target. A copy of the target is made to store it.
		*/
		TAMeas(const TAdjustablePoint& targetPos, const typename TTarget& tgt) :
			target(tgt),
			targetPos(&targetPos),
			line(NO_VALi),
			fFirstEquationIndex(NO_VALi),
			fFirstObservationIndex(NO_VALi)
		{};

		/// Sets LS matrices equation index of a first equation of this measurement
		void setFirstEquationIndex(MatrixIndex firstEquationIndex){fFirstEquationIndex = firstEquationIndex;}

		/// Returns LS-matrices equation index of a first equation of this measurement
		MatrixIndex getFirstEquationIndex ()const{return fFirstEquationIndex;}

		/// Returns LS-matrices observation index of a first observation of this measurement
		MatrixIndex getFirstObservationIndex ()const{return fFirstObservationIndex;}

		/// Sets LS matrices equation index of a first equation of this measurement
		void setFirstObservationIndex(MatrixIndex firstObservationIndex){fFirstObservationIndex = firstObservationIndex;}

		/// Get last index must be implemented in derived classes
		virtual MatrixIndex getLastEquationIndex() = 0;
};
	
/*!
	Base class for scalar measurement values. A single measurement may consist
	of an arbitrary number of values that can either be real numbers or angles.

	\tparam TTarget Target type of the measurement. The measurement receives a deep copy
			of the observed target. May be set to int and supplied with 0 if no target is used.

	\tparam TEnumScalar The type-safe range of indices to access the array of real valued measurement values. 
			The default parameter \ref ESingleValue allows to pass the value 0 to access a single measurement.

	\tparam numScalars The number of real values in this measurement. Must match the number of entries in TEnumScalar.

	\tparam TEnumAngle The type-safe range of indices to access the array of angle measurement values. 
			The default parameter \ref ENoValues disables the access to angle measurements

	\tparam numAngles The number of angles in this measurement. Must match the number of entries in TEnumAngle.
*/
template<typename TTarget, 
		 typename TEnumScalar = ESingleValue, int numScalars = 1, 
		 typename TEnumAngle = ENoValues, int numAngles = 0>
class TAScalarMeas : public TAMeas<TTarget> 
{
	protected:
		/// Typed container of real valued measurements
		std::array<TReal, numScalars> scalars;
		/// Typed container of angle valued measurements
		std::array<LGC::TAngle, numAngles> angles;
			
		/// Typed container of real valued measurements residuals, stored in meters [m]
		std::array<TReal, numScalars> scalarsResiduals;
		/// Typed container of angle valued measurements residuals
		std::array<LGC::TAngle, numAngles> anglesResiduals;

	public:
					
		/*!
			Constructs an uninitialized measurement

			\param targetPos The observed point
			\param tgt The observed target. A copy of the target is made to store it.
		*/
		TAScalarMeas(const TAdjustablePoint& targetPos, const typename TTarget& tgt) :
			TAMeas(targetPos, tgt)
		{
			for (int i = 0; i < numScalars; i++)
				scalars[i] = NO_VALf;
		}

		/*!
			Constructs an measurement that is initialized with a single real value

			\param targetPos The observed point
			\param tgt The observed target. A copy of the target is made to store it.
			\param value The measured value
		*/
		TAScalarMeas(const TAdjustablePoint& targetPos, typename TTarget tgt, TReal value) :
			TAMeas(targetPos, tgt)
		{
			static_assert(numScalars==1, "This works only for single scalar values.");
			scalars[0] = value;
		}

		/// The virtual base destructor does nothing.
		virtual ~TAScalarMeas() {}

		/// Sets a scalar value
		void setScalar(TReal v, TEnumScalar id=kValue) {
			scalars[id] = v;
		}

		/// Returns a scalar value for a given ID
		TReal getScalar(TEnumScalar id=kValue) const {
			return scalars[id];
		}

		/// Sets an angle value
		void setAngle(const LGC::TAngle& a, TEnumAngle id=kValue) {
			angles[id] = a;
		}

		/// Returns an angle value for a given ID
		const LGC::TAngle& getAngle(TEnumAngle id=kValue) const {
			return angles[id];
		}

		/// Sets a residual of observed scalar
		void setScalarResidual(TReal v, TEnumScalar id=kValue) {
			scalarsResiduals[id] = v;
		}

		/// Returns a residual of observed scalar
		TReal getScalarResidual(TEnumScalar id=kValue) const {
			return scalarsResiduals[id];
		}

		/// Sets a residual of observed angle
		void setAngleResidual(const LGC::TAngle& a, TEnumAngle id=kValue) {
			anglesResiduals[id] = a;
		}

		/// Returns a residual of observed angle
		const LGC::TAngle& getAngleResidual(TEnumAngle id=kValue) const {
			return anglesResiduals[id];
		}
};

/*!
	Base class for measurement values. A single measurement consists in a vector

	\tparam TTarget Target type of the measurement. The measurement receives a deep copy
			of the observed target. May be set to int and supplied with 0 if no target is used.
*/
template <typename TTarget>
class TAVectorMeas : public TAMeas<TTarget>
{
		
	protected:

		/// FreeVector containing measurment value
		TFreeVector vector;
		/// X-component residual
		TReal XcompResidual;
		/// Y-component residual
		TReal YcompResidual;
	public:


		/*!
			Constructs an uninitialized measurement

			\param targetPos The observed point
			\param tgt The observed target. A copy of the target is made to store it.
		*/
		TAVectorMeas(const TAdjustablePoint& targetPos, const typename TTarget& tgt) :
			TAMeas(targetPos, tgt),
			vector(NO_VALf, NO_VALf, NO_VALf,TCoordSysFactory::ECoordSys::k3DCartesian),
			XcompResidual(NO_VALf),
			YcompResidual(NO_VALf)
		{
		}

		/*!
			Constructs an measurement that is initialized with a FreeVector

			\param targetPos The observed point
			\param tgt The observed target. A copy of the target is made to store it.
			\param value The measured vector
		*/
		TAVectorMeas(const TAdjustablePoint& targetPos, typename TTarget tgt, const TFreeVector& value) :
			TAMeas(targetPos, tgt),
			vector(value),
			XcompResidual(NO_VALf),
			YcompResidual(NO_VALf)
		{
		}

		/// The virtual base destructor does nothing.
		virtual ~TAVectorMeas() {}

		/// Sets a vector
		void setVectorMeasurement(const TFreeVector& vec) {
			vector = vec;
		}

		/// Returns a vector value
		const TFreeVector& getVectorValue() const {
			return vector;
		}

		/// Sets a residual of an X component of observed vector
		void setXVectorComponentResidual(TReal res) {
			XcompResidual = res;
		}

		/// Sets a residual of an Y component of observed vector
		void setYVectorComponentResidual(TReal res) {
			YcompResidual = res;
		}

		/// Returns a residual of an X component of observed vector
		const TReal getXCompVectorResidual() const {
			return XcompResidual;
		}

		/// Returns a residual of an Y component of observed vector
		const TReal getYCompVectorResidual() const {
			return YcompResidual;
		}
};

#endif