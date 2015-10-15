//TLSCalcHorAngleObservation.h

/*! 
	Abstract Base Class for any observation used in the least square algorithms. It defines the
	different error detection parameters.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				angle observations, position and angle parameters)

	Copyright 2003 CERN EST/SU. All rights reserved.

*/

#ifndef SU_ABSTRACT_LSCALC_OBS
#define SU_ABSTRACT_LSCALC_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
using namespace std;
 
#include "UEOIndices.h"

//!Abstract Class for any observation used in the least square algorithms
class TALSCalcObservation {

public:
	/*!@name Constructor / Destructor */
	//@{
		/*! Default constructor */
		TALSCalcObservation();
		/*! Copy constructor */
		TALSCalcObservation(const TALSCalcObservation& source);
		/*! Destructor */
		virtual ~TALSCalcObservation();
	//@}

	/*!@name Access methods : facade*/
	//@{
		/*! Returns the Z value */
		virtual TReal	getZ() const {return fZ;}
		/*! Returns the W value */
		virtual TReal	getW() const {return fW;}
		/*! Returns the T value */
		virtual TReal	getT() const {return fT;}
		/*! Returns the Delty value */
		virtual TReal	getDelty() const {return fDelty;}
		/*! Returns the geode identifiant*/
		virtual int		getId() const {return 0;}
		/*! Returns the geode comment */
		virtual string	getComment() const {return "";}
	//@}

	/*!@name Access methods for ls calc observations */
	//@{
		/*! Returns the equations index */
		virtual MatrixIndex		getEqnIndex() const;
		/*! Returns the observations index */
		virtual MatrixIndex		getObsIndex() const;
		/*! Indicates if the error detection parameters can be determined or not */
		virtual bool			paramsCanBeDetermined() const {return fAreDetermined;}
		/*! Indicates if the w param is to compute: false if simulation */
		virtual bool			wToCompute() const {return fWToCompute;}
		/*! Indicates if G has to be computed */
		virtual bool			gToCompute() const {return fGToCompute;}
		/*! Indicates if the parameters for statistics have been rightly chosen */
		virtual bool			deltaComputed() const {return fDeltaComputed;}
		/*! Returns the error during computation */
		virtual string	getError() const {return fError;}
		/*! return true if the simulated value has been written*/
		virtual bool  isSimValueWritten() {return fSimValueIsWritten;}
		/*! return true if the simulated value has been written*/
		virtual bool  isSimValueWritten() const {return fSimValueIsWritten;}
	//@}

	/*! sets wToCompute to false */
	virtual void	wIsNotToCompute();

	/*! Computes the different error detection parameters */
	virtual void	computeErrorDetectionParam(TReal alpha, TReal beta, TReal res, TReal varRes, TReal sAPriori, TReal s0);
	
	/*!Sets observations and equations indices
	\param ueoi a UEOIndices object holding the lowest free unknown, equation and observation indices */
	virtual UEOIndices	setEOIndices(UEOIndices ueoi);

	/*! turn the boolean indicating if the simulated value has been written to true */
	virtual void  simValueWritten() {fSimValueIsWritten = true; return;}


protected :

		/*! Returns the G value as a TReal*/
		TReal	getGValue() const {return fGValue;}
		/*! Returns the Nabla value as a TReal*/
		TReal	getNablaValue() const {return fNablaValue;}



private:

	/*!@name Error detection related attributes */
	//@{
	TReal		fZ; /*!< local reliability statistic */
	TReal		fW; /*!< Gross-error detection statistic */
	TReal		fT; /*!< Level of ease a gross error of size NABLA can be detected */
	TReal		fDelty; /*!< Maximum effect of an undetected gross-error */
	TReal		fNablaValue; /*!< Value for greatest undetected error */
	TReal		fGValue; /*!< Value for estimation of the gross-error made with the statistic wi */
	bool		fAreDetermined; /*!< indicates if the error detection params can be computed */
	bool		fWToCompute; /*!< Indicates if the w param is to compute: false if simulation */
	bool		fGToCompute; /*!< indicates if G needs to be computed */
	bool		fDeltaComputed; /*!< indicates that the parameters for statistic tests have been rightly chosen */
	string		fError;
	bool		fSimValueIsWritten; /*< indicates if the simulated value has already been written in .sim file */
	//@}

	UEOIndices	fIndices; /*!< Unknown, Equation, observation indices of the observation for calculation */
};

#endif //SU_ABSTRACT_LSCALC_OBS