#ifndef TADJUSTABLE_ANGLE
#define TADJUSTABLE_ANGLE

#include "TLGCAngle.h"
#include "TVAdjustableObject.h"

/*! 
	\ingroup AdjustableObjects
	\brief Class providing adjustable related information for a LGC::TAngle class.
*/
class TAdjustableAngle: public TVAdjustableObject{
public:

	/*!@name Constructors */
	//@{
		/*!
			\brief Constructs an TAdjustableAngle based on an existing LGC::TAngle value.
		
			\param[in] angleValue A reference to an angle that will be adjusted. The angle will be copied 
								  so the pointed-to object is no longer needed after construction.
			\param[in] isFixed The angle will be excluded from the adjustment if set to TRUE, i.e. fixed angle.
			\param[in] name Name of the adjustable angle.
		*/
		TAdjustableAngle(const LGC::TAngle& angleValue, bool isFixed, const std::string& name);
	//@}
		/// Create an unitialized angle, the angle is by default fixed and has no NO_VALf inside.
		static TAdjustableAngle createUninitialized(const std::string& name);

	/*!@name Access methods*/
	//@{
		/// Returns a constant reference on the provisional value of the angle. 
		inline const LGC::TAngle& 	getProvisionalValue() const {return fProvisionalValue;}

		/// Returns a constant reference on the correction value. 
		inline const LGC::TAngle&	getCorrection() const {	return fCorrection;}

		/// Returns a constant reference on the estimated value of the angle. 
		inline const LGC::TAngle&	getEstimatedValue() const {	return fEstimatedValue;}
			
		/// Returns a constant reference on the estimated precision of the angle. 
		inline const LGC::TAngle&	getEstimatedPrecision() const {	return fEstimatedPrecision;}


		/*!
			\brief Returns The number of unknowns for this angle.

			Number of unknowns that are added to the adjustment by this angle.
			This number varies from zero to 1 unknowns.		
		*/
		inline virtual int getNumUnkn() const { return (int)!ifFixed;}

		/// See \ref TVAdjustjustableObject::isFixed
		inline virtual bool isFixed() const { return ifFixed;}

		/*! 
			See \ref TVAdjustableObject::getFirstUidx

			\throws Throws a logic_error if the angle is fixed.
		*/
		inline virtual int getFirstUidx() const {	
			if (!ifFixed)
				return uidx;			
			throw std::logic_error("Trying to get unknown index from fixed angle.");
		}

		/*! 
			See \ref TVAdjustableObject::getLastUidx

			\throws Throws a logic_error if no component of the angle is variable, i.e. a fixed angle.
		*/
		inline virtual int getLastUidx() const {
			if (!ifFixed)
				return uidx;			
			throw std::logic_error("Trying to get unknown index from fixed angle.");
		}

		/// Returns the name of the angle.
		inline virtual const std::string& getName() const { return fName;}

		/// Returns TRUE since the angle is always initialized.
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

		/*! 
			\brief Re-initialise the object

			Sets the estimated values to be the provisional values and for the precisions, correections and covariances zeros. Typically used for SIMULATION.
		*/
		void reInitialise();

	//@}

private:
	LGC::TAngle					fProvisionalValue; /*!< Angle provisional value. */
	LGC::TAngle					fCorrection; /*!< Angle correction after calculation.  */
	LGC::TAngle					fEstimatedValue; /*!< Angle estimated value after calculation. */
	LGC::TAngle					fEstimatedPrecision;  /*!< Angle estimated precision after calculation. */

	bool ifFixed; /*!< Tells if the angle is excluded from the adjustment, TRUE = fixed*/
	int  uidx;	/*!< Index of the angle in LS input matrix (unknown) */

	std::string fName; /*!< Name of the adjustable angle. */
};

#endif //TADJUSTABLE_ANGLE