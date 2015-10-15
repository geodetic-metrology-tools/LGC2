#ifndef TADJUSTABLE_ANGLE
#define TADJUSTABLE_ANGLE

#include "TVAdjustableObject.h"
#include "TLGCAngle.h"


class TAdjustableAngle: public TVAdjustableObject{
public:

	/*!@name Constructors */
	//@{
		/*!
			\brief Constructs an TAdjustableAngle based on an existing TAngle value, state (fixed or variable) and its name. 
		
			\param[in] scalValue A reference to an angle that will be adjusted. The angle will be copied 
							so the pointed-to object is no longer needed after construction.
			\param[in] isFixed The angle will be excluded from the adjustment if set to TRUE, i.e. fixed angle.
			\param[in] name Name of the adjustable angle.
		*/
		TAdjustableAngle(const LGC::TAngle& angleValue, bool isFixed, const std::string& name);
	//@}
		/// Create an unitialized point. 
		static TAdjustableAngle createUninitialized(const std::string& name);

	/*!@name Access methods*/
	//@{
		/*! Returns the provisional value of the angle. */
		inline LGC::TAngle 	getProvisionalValue() const {return fProvisionalValue;}

		/*! Returns the correction value. */
		inline LGC::TAngle		getCorrection() const {	return fCorrection;}

		/*! Returns the estimated value of the angle. */
		inline LGC::TAngle		getEstimatedValue() const {	return fEstimatedValue;}
			
		/*! Returns the estimated precision of the angle. */
		inline LGC::TAngle		getEstimatedPrecision() const {	return fEstimatedPrecision;}


		/*!
			Number of unknowns that are added to the adjustment by this angle.
			This number varies from zero to 1 unknowns.

			\returns The number of unknowns for this angle.
		*/
		inline virtual int getNumUkn() const { return (int)!isAngleFixed;}

		/// See \ref TVAdjustjustableObject::isFixed
		inline virtual bool isFixed() const { return isAngleFixed;}

		/*! 
			See \ref TVAdjustableObject::getFirstUidx

			\throws Throws a logic_error if the angle is fixed.
		*/
		inline virtual int getFirstUidx() const {	
			if (!isAngleFixed)
				return uidx;			
			throw std::logic_error("Trying to get unknown index from fixed angle.");
		}

		/*! 
			See \ref TVAdjustableObject::getLastUidx

			\throws Throws a logic_error if no component of the angle is variable, i.e. a fixed angle.
		*/
		inline virtual int getLastUidx() const {
			if (!isAngleFixed)
				return uidx;			
			throw std::logic_error("Trying to get unknown index from fixed angle.");
		}

		/*!
			\returns Name of the angle.
		*/
		inline virtual const std::string& getName() const { return fName;}

		/*!
			Angle is in any case initialized.
		*/
		inline virtual bool isInitialized() const{ return true;}

	//@}

	/*!@name Settings */
	//@{

		/*! 
			See \ref TVAdjustableObject::setFirstUidx

			\throws Throws a logic_error if the angle is fixed.
		*/
		virtual void setFirstUidx(int idx);

		/*! 
			See \ref TVAdjustableObject::setCorrection. Radians value expected.
		*/
		virtual void setCorrection(int idx, TReal value);

		/*! Sets the estimated precision after calculation in radians.*/
		void	setEstimatedPrecision(int idx, TReal ep);

	//@}

private:
	LGC::TAngle					fProvisionalValue; /*!< Angle's provisional value. */
	LGC::TAngle					fCorrection; /*!< Angle's correction after calculation.  */
	LGC::TAngle					fEstimatedValue; /*!< Angle's estimated value after calculation. */
	LGC::TAngle					fEstimatedPrecision;  /*!< Angle's estimated precision after calculation. */

	bool isAngleFixed; /*!< Tells if the angle is excluded from the adjustment (TRUE means that angle is fixed)*/
	int  uidx;	/*!< INdex of the angle in LS input matrix (unknown) */

	std::string fName; /*!< Name of the adjustable angle. */
};

#endif //TADJUSTABLE_ANGLE