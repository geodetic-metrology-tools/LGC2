#ifndef _TA_ANGLE_MEAS_H_
#define _TA_ANGLE_MEAS_H_

#include "TInstrumentData.h"
#include <array>
#include <TFreeVector.h>
#include "UEOIndices.h"

class TAdjustablePoint;

/// Enum for the case that no value of a certain type is measured in \ref TAScalarMeas.
enum ENoValues {};

/// Enum for the case that only a single value of a certain type is measured in \ref TAScalarMeas.
enum ESingleValue {
	kValue ///< Symbolic access to a single value in a measurement
};
	

/*!
	\ingroup Measurements
	\brief Template based abstract class for measurements. It contains equation & observation indices, Target of the measurement and its position.

	\tparam TTarget Target type of the measurement. The measurement receives a deep copy
					of the observed target. May be set to int and supplied with 0 if no target is used.
*/
template<typename TTarget>
class TAMeas {
	protected:

		/// Index of first EQUATION introduced by this measurement in the LS design matrices 
		MatrixIndex fFirstEquationIndex;
		/// Index of first OBSERVATION introduced by this measurement in the LS design matrices 
		MatrixIndex fFirstObservationIndex;

	public:
		/// A copy of the target with individual properties
		TTarget target;
			
		/// Pointer to the observed point on which the target is positioned
		const TAdjustablePoint* targetPos; 

		/// Line in the input file where this measurement was defined
		int line;

		/// DB comment after the measurement definition
		std::string eolcomment;

		/*!@name Constructors */
		//@{				
			/*!
				\brief Constructs an uninitialized measurement.

				\param targetPos The observed point on which the target is positioned.
				\param tgt The target which is used. A copy of the target is made to store it.
			*/
			TAMeas(const TAdjustablePoint& targetPos, const TTarget& tgt) :
				target(tgt),
				targetPos(&targetPos),
				line(NO_VALi),
				fFirstEquationIndex(NO_VALi),
				fFirstObservationIndex(NO_VALi)
			{};

		//@}

		/*!@name Access methods*/
		//@{
			/// Returns LS-matrices index of the first equation of the model.
			MatrixIndex getFirstEquationIndex ()const{return fFirstEquationIndex;}

			/// Returns LS-matrices observation index of a first observation of this measurement
			MatrixIndex getFirstObservationIndex ()const{return fFirstObservationIndex;}

			/// Get last equation index. This method must be implemented in  the derived classes, depending on the number of equations of the model.
			virtual MatrixIndex getLastEquationIndex() = 0;

		//@}

		/*!@name Setting methods */
		//@{
			/// Sets LS matrices EQUATION index of the first equation in the measurement model
			void setFirstEquationIndex(MatrixIndex firstEquationIndex){fFirstEquationIndex = firstEquationIndex;}

			/// Sets LS matrices OBSERVATION index of the first observation of this measurement
			void setFirstObservationIndex(MatrixIndex firstObservationIndex){fFirstObservationIndex = firstObservationIndex;}
		//@}
};
	
/*!
	\ingroup Measurements

	\brief Base class for SCALAR measurement values. A single measurement may consist
	of an arbitrary number of values that can either be real numbers (distances) or angles.

	\tparam TTarget Target type of the measurement. The measurement receives a deep copy
			of the observed target. May be set to int and supplied with 0 if no target is used.

	\tparam TEnumDistance The type-safe range of indices to access the array of real valued distance measurement values. 
			The default parameter \ref ESingleValue allows to pass the value 0 to access a single measurement.

	\tparam numDistances The number of real valued distances in this measurement. Must match the number of entries in TEnumDistance.

	\tparam TEnumAngle The type-safe range of indices to access the array of angle measurement values. 
			The default parameter \ref ENoValues disables the access to angle measurements

	\tparam numAngles The number of angles in this measurement. Must match the number of entries in TEnumAngle.
*/
template<typename TTarget, 
		 typename TEnumDistance = ESingleValue, int numDistances = 1, 
		 typename TEnumAngle = ENoValues, int numAngles = 0>
class TAScalarMeas : public TAMeas<TTarget> 
{
	protected:
		/// Typed container of real valued (distance) measurements.
		std::array<TReal, numDistances> distances; 
		/// Typed container of angle valued measurements.
		std::array<TAngle, numAngles> angles;
			
		/// Typed container of real valued measurement residuals. Stored in meters [m].
		std::array<TReal, numDistances> distancesResiduals;
		/// Typed container of angle valued measurements residuals.
		std::array<TAngle, numAngles> anglesResiduals; 

	public:				
		/*!
			\brief Constructs an uninitialized measurement

			\param targetPos The observed point on which the Target (tgt) is positioned.
			\param tgt The observed target. A copy of the target is made to store it.
		*/
		TAScalarMeas(const TAdjustablePoint& targetPos, const TTarget& tgt) :
			TAMeas<TTarget>(targetPos, tgt)
		{
			for (int i = 0; i < numDistances; i++)
				distances[i] = NO_VALf;
		}

		/*!
			\brief Constructs an measurement that is initialized with a single real value

			\param targetPos The observed point
			\param tgt The observed target. A copy of the target is made to store it.
			\param value The measured value.
		*/
		TAScalarMeas(const TAdjustablePoint& targetPos, TTarget tgt, TReal value) :
			TAMeas<TTarget>(targetPos, tgt)
		{
			static_assert(numDistances==1, "This works only for single distance values.");
			distances[0] = value;
		}

		/// The virtual base destructor does nothing.
		virtual ~TAScalarMeas() {}
		/*!@name Access methods*/
		//@{
			/// Returns a distance value for a given ID
			TReal getDistance(TEnumDistance id=kValue) const {
				return distances[id];
			}

			/// Returns an angle value for a given ID
			const TAngle& getAngle(TEnumAngle id=kValue) const {
				return angles[id];
			}

			/// Returns a residual of observed distances
			TReal getDistanceResidual(TEnumDistance id=kValue) const {
				return distancesResiduals[id];
			}

			/// Returns residual of the observed angle
			const TAngle& getAngleResidual(TEnumAngle id=kValue) const {
				return anglesResiduals[id];
			}
		//@}

		/*!@name Setting methods */
		//@{
			/// Sets a distance value
			void setDistance(TReal v, TEnumDistance id=kValue) {
				distances[id] = v;
			}


			/// Sets an angle value
			void setAngle(const TAngle& a, TEnumAngle id=kValue) {
				angles[id] = a;
			}

			/// Sets a residual of observed distance
			void setDistanceResidual(TReal v, TEnumDistance id=kValue) {
				distancesResiduals[id] = v;
			}

			/// Sets a residual of observed angle
			void setAngleResidual(const TAngle& a, TEnumAngle id=kValue) {
				anglesResiduals[id] = a;
			}
		//@}
};

/*!
	Base class for a vector measurements.

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
			\brief Constructs an uninitialized measurement

			\param targetPos The observed point
			\param tgt The observed target. A copy of the target is made to store it.
		*/
		TAVectorMeas(const TAdjustablePoint& targetPos, const TTarget& tgt) :
			TAMeas<TTarget>(targetPos, tgt),
			vector(NO_VALf, NO_VALf, NO_VALf,TCoordSysFactory::ECoordSys::k3DCartesian),
			XcompResidual(NO_VALf),
			YcompResidual(NO_VALf)
		{
		}

		/*!
			\brief Constructs an measurement that is initialized with a FreeVector

			\param targetPos The observed point
			\param tgt The observed target. A copy of the target is made to store it.
			\param value The measured vector
		*/
		TAVectorMeas(const TAdjustablePoint& targetPos, TTarget tgt, const TFreeVector& value) :
			TAMeas<TTarget>(targetPos, tgt),
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