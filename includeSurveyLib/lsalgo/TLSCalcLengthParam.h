// TLSCalcLengthParam.h

#ifndef SU_LENGTH_PARA
#define SU_LENGTH_PARA

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
//using namespace std;

#include "TLength.h"
#include "UEOIndices.h"
#include "TALSCalcParameter.h"

class TLSCalcLengthParam : public TALSCalcParameter
{
public:
	/*!@name Constructors / Destructor */
	//@{
		/*! Default Constructor */
		TLSCalcLengthParam();
		/*! Constructor
		\param lobs observed value of the length parameter
		\param ecs status of the observed length (kFixed / kVariable) */
		TLSCalcLengthParam(TLength lobs,TALSCalcParameter::ELSStatus els, std::string name);
		/*! Copy constructor */
		TLSCalcLengthParam(const TLSCalcLengthParam& source);
		/*! Destructor */
		virtual ~TLSCalcLengthParam();
	//@}

	/*!@name Access methods */
	//@{
		/*! Returns the provisional value of the parameter */
		TLength		getProvisionalValue() const;
		/*! Returns the correction to be brought on the provisional value */
		TLength		getCorrection() const;
		/*! Returns the estimated value of the parameter after correction */
		TLength		getEstimatedValue() const;
		/*! Returns the estimated precision of the parameter after calculation */
		TLength		getEstimatedPrecision() const;
		/*! Returns the LSCalc status of the parameter for calculation */
		TALSCalcParameter::ELSStatus	getStatus() const;
		/*! Returns the index of the parameter's unknown index */
		MatrixIndex	getIndex() const;
	//@}

	/*!@name Settings */
	//@{
		/*! Sets the provisional value */
		void		setProvisionalValue(TLength pv);
		/*! Sets the correction's value */
		void		setCorrection(TLength corr);
		/*! Sets the estimated precision */
		void		setEstimatedPrecision(TLength ep);
		/*! Sets the status od the parameter */
		void		setStatus(TALSCalcParameter::ELSStatus lsStat);
		/*! Sets the unknown index for calculation */
		UEOIndices		setUIndex(UEOIndices ui);
		/*! re-initialises the parameters alterred during or after
			a least squares calculation */
		void	reInitialise();
	//@}

	
protected:

	/*! Sets the estimated value (provisional + correction) */
	void		setEstimatedValue();

private:

	/*! Copy assignment operator */
	TLSCalcLengthParam& operator=(const TLSCalcLengthParam& right);

	TLength		fProvisionalValue; /*!< length's provisional value */
	TLength		fCorrection; /*!< length's correction after calculation  */
	TLength		fEstimatedValue; /*!< length's estimated value after calculation */
	TLength		fEstimatedPrecision; /*!< length's estimated precision */
	
	MatrixIndex	fIndex; /*!< length parameter's unknown index */
	TALSCalcParameter::ELSStatus	fStatus; /*!< length parameter's status for calculation */
};

/*!@name Constants definition */
//@{
/*! List of ls calc length observations  */
typedef std::list< TLSCalcLengthParam >			LSLengthContainer;
/*! Iterator of the list of ls calc length observations */
typedef LSLengthContainer::iterator			LSLengthIter;
/*! Constant iterator of the list of ls calc length observations */
typedef LSLengthContainer::const_iterator	LSLengthConstIter;
//@}

#endif //SU_LENGTH_PARA
