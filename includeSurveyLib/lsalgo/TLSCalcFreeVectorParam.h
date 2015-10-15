// TLSCalcFreeVectorParam.h


#ifndef SU_FREE_VECTOR_PARA
#define SU_FREE_VECTOR_PARA

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
//using namespace std;

#include "TSpatialPoint.h"
#include "TSpatialPosition.h"
#include "TFreeVector.h"
#include "UEOIndices.h"
#include "TALSCalcParameter.h"

class TLSCalcFreeVectorParam : public TALSCalcParameter
{
public:
	/*!@name Constructors / destructor */
	//@{
		/*! Default constructor */
		TLSCalcFreeVectorParam();
		/*! Constructor
		\param spobs provisional value for the spatial point */
		TLSCalcFreeVectorParam(TFreeVector pos,struct LSParaStatus status,string name);
		/*! Copy constructor */
		TLSCalcFreeVectorParam(const TLSCalcFreeVectorParam& source);
		/*! Destructor */
		virtual ~TLSCalcFreeVectorParam();
		int size();
	
	//@}


	/*!@name Access methods */
	//@{
		/*! Returns the provisional value of the position vector */
		TFreeVector	getProvisionalValue() const;
		/*! Returns the correction value */
		TFreeVector		getCorrection() const;
		/*! Returns the estimated value for the position */
		TFreeVector	getEstimatedValue() const;
		/*! Returns the estimated precision for the position */
		TFreeVector		getEstimatedPrecision() const;
		/*! Returns the covariance element for the position */
		TFreeVector		getEstimatedCovariance() const;
		/*! Returns the status of the X-position for calculation */
		TALSCalcParameter::ELSStatus	getXStatus() const;
		/*! Returns the status of the Y-position for calculation */
		TALSCalcParameter::ELSStatus	getYStatus() const;
		/*! Returns the status of the Z-position for calculation */
		TALSCalcParameter::ELSStatus	getZStatus() const;
		/*! Returns the unknown index of the X-position  */
		MatrixIndex			getXIndex() const;
		/*! Returns the unknown index of the Y-position  */
		MatrixIndex			getYIndex() const;
		/*! Returns the unknown index of the Z-position  */
		MatrixIndex			getZIndex() const;
		/*!Retutns the status (Cala, Vxy....)*/
		TSpatialStatus::ESpatialStatus	getGlobalStatus() const;
	//@}

	/*!@name Settings */
	//@{
		void	resetValues();
		/*! Sets the provisional value of the position vector */
		void	setProvisionalValue(TFreeVector pv);
		/*! Sets the estimated value of the position vector */
		void	setEstimatedValueVector(TFreeVector pv);
		/*! Sets the correction value */
		void	setCorrection(TFreeVector corr);
		/*! Sets the estimated precision after calculation */
		void	setEstimatedPrecision(TFreeVector ep);
		/*! Sets the covariance after calculation */
		void	setEstimatedCovariance(TFreeVector cov);
		/*! Sets the status of the vector */
		void	setStatus(struct LSParaStatus stat);
		/*! Sets the position vector parameter's unknown index */
		UEOIndices		setUIndex(UEOIndices ui);
		/*! re-initialises the parameters alterred during or after
			a least squares calculation */
		void	reInitialise();
	//@}

	
protected:

	/*! Sets the estimated value (provisional + correction) */
	void	setEstimatedValue();


private:

	/*! Copy assignment operator */
	TLSCalcFreeVectorParam&	operator=(const TLSCalcFreeVectorParam& right);

	TFreeVector				fProvisionalValue; /*!< position's provisional value */
	TFreeVector				fCorrection; /*!< position's correction after calculation  */
	TFreeVector				fEstimatedValue; /*!< position's estimated value after calculation */
	TFreeVector				fEstimatedPrecision; /*!< position's estimated precision after calculation */
	TFreeVector				fCovariance; 
	

	struct LSParaStatus		fStatus; /*!< position parameter's status for calculation */
	MatrixIndex				fFreeVectorIndices[3]; /*!< position parameter's unknown indices for calculation */
};

/*!@name Constants definition */
//@{
/*! List of ls calc position vector observations  */
typedef list< TLSCalcFreeVectorParam >		LSFreeVecContainer;
/*! Iterator of the list of ls calc position vector observations */
typedef LSFreeVecContainer::iterator			LSFreeVecIter;
/*! Constant iterator of the list of ls calc position vector observations */
typedef LSFreeVecContainer::const_iterator	LSFreeVecConstIter;
//@}


#endif //SU_FREE_VECTOR_PARA
