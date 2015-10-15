// TLSCalcScalarParam.h
//Class to store the scale parameter of the calculation

#ifndef SU_SCALAR_PARA
#define SU_SCALAR_PARA

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
//using namespace std;

#include "TSpatialPoint.h"
#include "TSpatialPosition.h"
#include "TFreeVector.h"
#include "TScalar.h"
#include "UEOIndices.h"
#include "TALSCalcParameter.h"

class TLSCalcScalarParam : public TALSCalcParameter
{
public:
	/*!@name Constructors / destructor */
	//@{
		/*! Default constructor */
		TLSCalcScalarParam();
		/*! Constructor
		/param stores provisional value for the scale */
		TLSCalcScalarParam(TScalar pos, ELSStatus status, string name);
		/*! Copy constructor */
		TLSCalcScalarParam(const TLSCalcScalarParam& source);
		/*! Destructor */
		virtual ~TLSCalcScalarParam();
	//@}


	/*!@name Access methods */
	//@{
		/*! Returns the provisional value of the scale */
		TScalar	getProvisionalValue() const;
		/*! Returns the correction value */
		TScalar		getCorrection() const;
		/*! Returns the estimated value for the scale */
		TScalar	getEstimatedValue() const;
		/*! Returns the estimated precision for the scale */
		TScalar		getEstimatedPrecision() const;
		/*! Returns the covariance element for the scale */
		TScalar		getEstimatedCovariance() const;
		/*! Returns the status of the scale for calculation */
		TALSCalcParameter::ELSStatus	getStatus() const;
		/*!Returns the status (Cala, Vxy....)*/
		TSpatialStatus::ESpatialStatus	getGlobalStatus() const;
		void resetValues();
	//@}

	/*!@name Settings */
	//@{
		/*! Sets the provisional value of the scale */
		void	setProvisionalValue(TScalar pv);
		/*! Sets the estimated value of the scale */
		void	setEstimatedValue(TScalar ev);
		/*! Sets the correction value */
		void	setCorrection(TScalar corr);
		/*! Sets the estimated precision after calculation */
		void	setEstimatedPrecision(TScalar ep);
		/*! Sets the covariance after calculation */
		void	setEstimatedCovariance(TScalar cov);
		/*! Sets the status of the scale */
		void	setStatus(ELSStatus stat);
		
		/*! Sets the position vector parameter's unknown index */
		
		UEOIndices		setUIndex(UEOIndices ui);
		MatrixIndex		getScalarIndex();

		/*! re-initialises the parameters alterred during or after
			a least squares calculation */
		void	reInitialise();
	//@}

	
protected:

	/*! Sets the estimated value (provisional + correction) */
	void	setEstimatedValue();


private:

	/*! Copy assignment operator */
	TLSCalcScalarParam&	operator=(const TLSCalcScalarParam& right);

	TScalar				fProvisionalValue; /*!< scale's provisional value */
	TScalar				fCorrection; /*!< scale's correction after calculation  */
	TScalar				fEstimatedValue; /*!< scale's estimated value after calculation */
	TScalar				fEstimatedPrecision; /*!< scale's estimated precision after calculation */
	TScalar				fCovariance; 
	

	TALSCalcParameter::ELSStatus		fStatus; /*!< scale parameter's status for calculation */
	MatrixIndex			fScalarIndices[1]; /*!< scalar parameter's unknown indices for calculation */
};

/*!@name Constants definition */
//@{
/*! List of ls calc position vector observations  */
typedef list< TLSCalcScalarParam >		LSScalarContainer;
/*! Iterator of the list of ls calc position vector observations */
typedef LSScalarContainer::iterator			LSScalarIter;
/*! Constant iterator of the list of ls calc position vector observations */
typedef LSScalarContainer::const_iterator	LSScalarConstIter;
//@}


#endif //SU_POS_VECTOR_PARA
