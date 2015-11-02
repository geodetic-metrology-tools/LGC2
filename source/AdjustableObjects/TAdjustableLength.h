#ifndef TADJUSTABLE_LENGTH
#define TADJUSTABLE_LENGTH

#include "TLength.h"
#include "TVAdjustableObject.h"

/*! 
	\ingroup AdjustableObjects
	\brief Class providing adjustable related information for a TLength class.
*/
class TAdjustableLength: public TVAdjustableObject{
public:

	/*!@name Constructors */
	//@{
		/*!
			\brief Constructs an TAdjustableLength based on an existing TLength value.
		
			\param[in] lengthValue A reference to an length that will be adjusted. The length will be copied 
								  so the pointed-to object is no longer needed after construction.
			\param[in] isFixed The length will be excluded from the adjustment if set to TRUE, i.e. fixed length.
			\param[in] name Name of the adjustable length.
		*/
		TAdjustableLength(const TLength& lengthValue, bool isFixed, const std::string& name);
	//@}
		/// Create an unitialized length, the length is by default fixed and has no NO_VALf inside.
		static TAdjustableLength createUninitialized(const std::string& name);

	/*!@name Access methods*/
	//@{
		/// Returns a constant reference on the provisional value of the length. 
		inline const TLength& 	getProvisionalValue() const {return fProvisionalValue;}

		/// Returns a constant reference on the correction value. 
		inline const TLength&	getCorrection() const {	return fCorrection;}

		/// Returns a constant reference on the estimated value of the length. 
		inline const TLength&	getEstimatedValue() const { return fEstimatedValue;}
			
		/// Returns a constant reference on the estimated precision of the length. 
		inline const TLength&	getEstimatedPrecision() const { return fEstimatedPrecision;}


		/*!
			\brief Returns The number of unknowns for this length.

			Number of unknowns that are added to the adjustment by this length.
			This number varies from zero to 1 unknowns.		
		*/
		inline virtual int getNumUnkn() const { return (int)!ifFixed;}

		/// See \ref TVAdjustjustableObject::isFixed
		inline virtual bool isFixed() const { return ifFixed;}

		/*! 
			See \ref TVAdjustableObject::getFirstUidx

			\throws Throws a logic_error if the length is fixed.
		*/
		inline virtual int getFirstUidx() const {	
			if (!ifFixed)
				return uidx;			
			throw std::logic_error("Trying to get unknown index from fixed length.");
		}

		/*! 
			See \ref TVAdjustableObject::getLastUidx

			\throws Throws a logic_error if no component of the length is variable, i.e. a fixed length.
		*/
		inline virtual int getLastUidx() const {
			if (!ifFixed)
				return uidx;			
			throw std::logic_error("Trying to get unknown index from fixed length.");
		}

		/// Returns the name of the length.
		inline virtual const std::string& getName() const { return fName;}

		/// Returns TRUE since the length is always initialized.
		inline virtual bool isInitialized() const{ return true;}

	//@}

	/*!@name Settings */
	//@{

		/*! 
			See \ref TVAdjustableObject::setFirstUidx

			\throws Throws a logic_error if the length is fixed.
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
	TLength					fProvisionalValue; /*!< length provisional value. */
	TLength					fCorrection; /*!< length correction after calculation.  */
	TLength					fEstimatedValue; /*!< length estimated value after calculation. */
	TLength					fEstimatedPrecision;  /*!< length estimated precision after calculation. */

	bool ifFixed; /*!< Tells if the length is excluded from the adjustment, TRUE = fixed*/
	int  uidx;	/*!< Index of the length in LS input matrix (unknown) */

	std::string fName; /*!< Name of the adjustable length. */
};

#endif //TADJUSTABLE_length