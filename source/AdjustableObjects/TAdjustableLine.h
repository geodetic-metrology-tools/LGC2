#ifndef TADJUSTABLE_LINE_N
#define TADJUSTABLE_LINE_N

#include <bitset>

#include "TVAdjustableObject.h"
#include "TAdjustablePoint.h"
#include "TPositionVector.h"
#include "TFreeVector.h"

///////////////////////////
/// IMPLEMENTATION OF THIS CLASS IS NOT FINISHED, NEEDS REVIEW BEFORE IT IS USED!!!!!!!!!!
///////////////////////////

/*! 
	\ingroup AdjustableObjects
	\brief Adds adjustable information to a line object defined through a point (TPositionVector) on a line and a line vector (TFreeVector).
*/
class TAdjustableLine : public TVAdjustableObject
{
public:
	/*!@name Constructors*/
	//@{

		/*!
			\brief Constructs an TAdjustableLine based on known point on a line and a line vector.
			
			\param[in] pointOnALine Known (e.g. measured) point on a line.
			\param[in] lineVect Line vector, i.e. vector parallel with the line.
			\param[in] lineVectorFixedState Lock state of the line vector.
			\param[in] name Name of the adjustable line.
		*/
	   //In this case point on a line is fixed, therefore we can ommit pointLockState and set it inside to fixed
	TAdjustableLine(const TAdjustablePoint* pointOnALine, const TFreeVector& lineVect, const std::bitset<3>& lineVectorFixedState, const std::string& name);

		/*!
			\brief Constructs an TAdjustableLine based on a reference point, point on a line and line vector.
			
			\param[in] pointOnALine Known (e.g. measured) point of line.
			\param[in] referencePoint Reference point for the Point on a line.
			\param[in] lineVect Line vector, i.e. vector parallel with the line.
			\param[in] lineVectorFixedState Lock state of the line vector.
			\param[in] name Name of the adjustable line.
		*/
		TAdjustableLine(const TAdjustablePoint* pointOnALine, const TPositionVector& referencePoint, const TFreeVector& lineVect, const std::bitset<3>& lineVectorFixedState, const std::string& name);
	
		
		/// Create an unitialized line. 
		static TAdjustableLine createUninitialized(const std::string& name);

		/// initialize the line. 
		void initialize(const TAdjustablePoint* pointOnALine, const TFreeVector& lineVect, const std::bitset<3>& lineVectorFixedState);
		/// initialize the line.
		void initialize(const TAdjustablePoint* pointOnALine, const TPositionVector& referencePoint, const TFreeVector& lineVect, const std::bitset<3>& lineVectorFixedState);
      
		//@}

	   /*!@name Access methods*/
	   //@{
		
		/// Returns a constant reference on the provisional value of the line's vector 
		const TFreeVector&	getLineVectorProvisionalValue() const {return fLineVectorProvisionalValue;}

		/// Returns a constant reference on the correction value of the line's vector 
		const TFreeVector&	getLineVectorCorrection() const { return fLineVectorCorrection;}

		/// Returns a constant reference on the estimated value of the line's vector 
		const TFreeVector&	getLineVectorEstimatedValue() const { return fLineVectorEstimatedValue;}

		/// Returns a constant reference on the estimated precision of the line's vector 
		const TFreeVector&	getLineVectorEstimatedPrecision() const { return fLineVectorEstimatedPrecision;}

		/// Returns a constant reference on the covariances of the line's vector 
		const TFreeVector&	getLineVectorCovariances() const {return fLineVectorCovariance;}

		/// Returns a constant pointer on the line point
		const TAdjustablePoint* getLinePoint() const { return fLinePoint; }
		
		/// Returns a constant reference on the reference point 
		const TPositionVector& getReferencePoint() const { return fReferencePointPosition; }
		
      virtual bool isInitialized() const { return fInit; }

		/*!
			\brief Returns The number of unknowns for this line.

			Calculates the number of unknowns that are added to the adjustment by this line.
			This number varies from zero to six unknowns (3 for point + 3 for line vector).
		*/
		virtual int getNumUnkn() const;

		/// See \ref TVAdjustableObject::isFixed
		virtual bool isFixed() const { return(fixedStateLineVector.all());}

		/*! 
			See \ref TVAdjustableObject::getFirstUidx

			\throws Throws a logic_error if no component of the line is variable, i.e. a fixed line.
		*/
		virtual int getFirstUidx() const;

		/*! 
			See \ref TVAdjustableObject::getLastUidx

			\throws Throws a logic_error if no component of the line is variable, i.e. a fixed line.
		*/
		virtual int getLastUidx() const;

		/// Returns Name of the transformation.
		virtual const std::string& getName() const { return fName;}
		
	//@}


	/*!@name Settings.*/
	//@{

		/*! 
			See \ref TVAdjustableObject::setFirstUidx

			\throws Throws a logic_error if no component of the line is variable.
		*/
		virtual void setFirstUidx(int idx);
		
		/*! 
			See \ref TVAdjustableObject::setCorrection
		*/
      virtual void setCorrection(int idx, TReal value);

		/// Sets the estimated precision after calculation to a line's point
      void setLineVectorEstimatedPrecision(int idx, TReal value);

		/// Sets the covariance after calculation to a line's vector
		void setLineVectEstimatedPointCovariance(TFreeVector cov) {fLineVectorCovariance = cov; return; }

		void reInitialise();
	//@}


private:

	TPositionVector fReferencePointPosition; /*!< position of the Reference point (fixed)*/
	const TAdjustablePoint* fLinePoint;	//Line point is only being adjusted if there is associated Reference Point Position, otherwise it is fixed. 

	//Definition of line vector
	TFreeVector				fLineVectorProvisionalValue;
	TFreeVector				fLineVectorCorrection;
	TFreeVector				fLineVectorEstimatedValue; 
	TFreeVector				fLineVectorEstimatedPrecision; 
	TFreeVector				fLineVectorCovariance; 

	std::bitset<3> fixedStateLineVector; /*!< Tells which element of the line vector will be adjusted.*/

	int uidx_lineVector[3]; /*!< Position of the line vector's elements in the LS input matrix (column).*/

	std::string fName; /*!< Name of the adjustable line */

	void setDefaults();
	bool fInit;
};
#endif //TADJUSTABLE_LINE_N