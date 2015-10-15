
// TLSCalcOrientationParam.h


#ifndef SU_ORIENT_PARA
#define SU_ORIENT_PARA

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
//using namespace std;

#include "TRotationMatrix.h"
#include "UEOIndices.h"
#include "TALSCalcParameter.h"

class TLSCalcOrientationParam : public TALSCalcParameter
{

public:

	/*!@name Constructors / destructor */
	//@{
	/*! Default constructor */
		TLSCalcOrientationParam();
		/*! Constructor
		\param provAng provisional value for the orientation
		\param ems orientation status for calculation */
		explicit TLSCalcOrientationParam(const struct Angles& provAng,struct LSParaStatus ems, std::string name);
		/*! Copy constructor */
		TLSCalcOrientationParam(const TLSCalcOrientationParam& source);
		/*! Destructor */
		virtual ~TLSCalcOrientationParam();
	//@}

	/*!@name Access methods */
	//@{
	/*! Returns the provisional value of the position vector */
		struct Angles	getProvisionalValue() const;
		/*! Returns the correction value */
		struct Angles	getCorrection() const;
		/*! Returns the estimated value for the position */
		struct Angles	getEstimatedValue() const;
		/*! Returns the estimated precision for the position */
		struct Angles	getEstimatedPrecision() const;
		/*! Returns the status of the omega-position for calculation */
		TALSCalcParameter::ELSStatus	getOmegaStatus() const;
		/*! Returns the status of the phi-position for calculation */
		TALSCalcParameter::ELSStatus	getPhiStatus() const;
		/*! Returns the status of the kappa-position for calculation */
		TALSCalcParameter::ELSStatus	getKappaStatus() const;
		/*! Returns the status of the position for calculation */
		struct LSParaStatus	getStatus() const;
		/*! Returns the omega angle's unknown index  */
		MatrixIndex			getOmegaIndex() const;
		/*! Returns the phi angle's unknown index  */
		MatrixIndex			getPhiIndex() const;
		/*! Returns the kappa angle's unknown index  */
		MatrixIndex			getKappaIndex() const;
	//@}

	/*!@name Settings */
	//@{
		/*! Sets the angle's provisional value  */
		void	setProvisionalValue(TAngle pvo,TAngle pvp,TAngle pvk);
		/*! Sets the correction to the angle's provisional value */
		void	setCorrection(TAngle ocorr,TAngle pcorr,TAngle kcorr);
		/*! Sets the estimated precision after calculation */
		void	setEstimatedPrecision(TAngle epo,TAngle epp,TAngle epk);
		/*! Sets the angle's fixed/variable/1D variable status */
		void	setStatus(struct LSParaStatus lss);
		/*! Reset the values to zero */
		void resetValues();
		/*! Sets the unknown index */
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
	TLSCalcOrientationParam&	operator=(const TLSCalcOrientationParam& right);

	struct Angles		fProvisionalValue; /*!< orientation's provisional value */
	struct Angles		fCorrection; /*!< orientation's correction after calculation  */
	struct Angles		fEstimatedValue; /*!< orientation's estimated value after calculation */
	struct Angles		fEstimatedPrecision; /*!< orientation's estimated precision after calculation */
	
	struct LSParaStatus		fStatus; /*!< orientation parameter's status for calculation */
	MatrixIndex				fOrientIndex[3]; /*!< orientation parameter's unknown indices for calculation */
};

/*!@name Constants definition */
//@{
/*! List of ls calc orientation observations  */
typedef std::list< TLSCalcOrientationParam >		LSOrientContainer;
/*! Iterator of the list of ls calc orientation observations */
typedef LSOrientContainer::iterator			LSOrientIter;
/*! Constant iterator of the list of ls calc orientation observations */
typedef LSOrientContainer::const_iterator	LSOrientConstIter;
//@}


#endif //SU_ORIENT_PARA
