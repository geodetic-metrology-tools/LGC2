#ifndef TADJUSTABLE_SCALAR
#define TADJUSTABLE_SCALAR

#include <stdexcept>
#include "TVAdjustableObject.h"
#include "Global.h"

/*! 
	\ingroup AdjustableObjects
	\brief Adds adjustable information to a Scalar object.
*/
class TAdjustableScalar : public TVAdjustableObject {
public:
	/*!@name Constructor */
	//@{
		/*!
			\brief Constructs an TAdjustableScalar based on an existing scalar value, state (fixed or variable) and its name. 
		
			\param[in] scalValue A scalar that will be adjusted. 
			\param[in] isFixed The scalar will be excluded from the adjustment if set to TRUE, i.e. fixed scalar.
			\param[in] name Name of the adjustable scalar.
		*/
		TAdjustableScalar(TReal scalValue, bool isFixed, const std::string& name);

		/// Create an unitialized scalar. 
		static TAdjustableScalar createUninitialized(const std::string& name);
	//@}

	/*!@name Access methods*/
	//@{
		/// Returns the provisional value of the scalar. 
      inline TReal getProvisionalValue() const { return fProvisionalValue; }

		/// Returns the correction value of the scalar. 
      inline TReal getCorrection() const { return fCorrection; }

		// Returns the estimated value of the scalar. 
      inline TReal getEstimatedValue() const { return fEstimatedValue; }
			
		/// Returns the estimated precision of the scalar. 
      inline TReal getEstimatedPrecision() const { return fEstimatedPrecision; }

		/*!
			\brief Returns The number of unknowns of this scalar.
			
			Number of unknowns that are added to the adjustment by this scalar.
			This number varies from zero to 1 unknowns.		
		*/
		inline virtual int getNumUnkn() const { return (int)!isScalarFixed;}

		/// See \ref TVAdjustjustableObject::isFixed
		inline virtual bool isFixed() const { return isScalarFixed;}

		/*! 
			See \ref TVAdjustableObject::getFirstUidx

			\throws Throws a logic_error if the scalar is fixed.
		*/
		inline virtual int getFirstUidx() const{	
			if (!isScalarFixed)
				return uidx;			
			throw std::logic_error("Trying to get unknown index from fixed scalar.");
		}

		/*! 
			See \ref TVAdjustableObject::getLastUidx

			\throws Throws a logic_error if no component of the scalar is variable, i.e. a fixed scalar.
		*/
		inline virtual int getLastUidx() const {
			if (!isScalarFixed)
				return uidx;			
			throw std::logic_error("Trying to get unknown index from fixed scalar.");
		}

		/// Returns the estimated variance.
		inline TReal getVariance() const{ return powq(fEstimatedPrecision, 2);}

		/// Returns the estimated precision.
       inline TReal getEstPrecision() const { return fEstimatedPrecision; }

		/// Returns Name of the scalar.
		inline virtual const std::string& getName() const { return fName;}

		/*! 
			\see TAdjustableObject::isInitialized
		*/
		inline virtual bool isInitialized() const { return !isnan(fProvisionalValue);}

	//@}

	/*!@name Settings */
	//@{

		/*! 
			See \ref TVAdjustableObject::setFirstUidx

			\throws Throws a logic_error if the scalar is fixed.
		*/
		virtual void setFirstUidx(int idx);

		/*! 
			See \ref TVAdjustableObject::setCorrection

		*/
		virtual void setCorrection(int idx, TReal value);

		/// Sets the estimated precision after calculation 
		void	setEstimatedPrecision(int idx, TReal ep);

		/*! 
			\brief Re-initialise the object

			Sets the estimated values to be the provisional values and for the precisions, correections and covariances zeros.
		*/
		void reInitialise();


	//@}

private:

   TReal					fProvisionalValue; /*!< Scalar's provisional value. */
   TReal					fCorrection; /*!< Scalar's correction after calculation.  */
   TReal					fEstimatedValue; /*!< Scalar's estimated value after calculation. */
   TReal					fEstimatedPrecision;  /*!<Scalar's estimated precision after calculation. */

	bool isScalarFixed; /*!< Tells if scalar is excluded from the adjustment, i.e. fixed*/
	int  uidx;	/*!< Position of the scalar in LS input matrix (column)*/

	std::string fName; /*!< Name of the adjustable scalar. */
};
#endif