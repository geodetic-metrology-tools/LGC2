//TALSCalcAngleObservation.h

/*! 
	Abstract Class for angle observations used in the least square algorithms. It defines the
	the basic information concerning the observation (observed value, sigma...).

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				angle observations, position and angle parameters)

	Copyright 2003 CERN EST/SU. All rights reserved.

*/

#ifndef SU_ABSTRACT_LSCALC_ANGLE_OBS
#define SU_ABSTRACT_LSCALC_ANGLE_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TALSCalcObservation.h"
#include "TAngle.h"
#include "TDouble.h"

//! Abstract Class for angle observations used in the least square algorithms
class TALSCalcAngleObservation : public TALSCalcObservation {

public:

	/*!@name Constructor / Destructor
	//@
		! Default constructor */
		TALSCalcAngleObservation();
		/*! Cosntructor
		\param obsAngle observed value 
		\param sAPriori sigma a priori on the observation */
		TALSCalcAngleObservation(TAngle obsAngle, TAngle sAPriori);
		/*! Copy constructor */
		TALSCalcAngleObservation(const TALSCalcAngleObservation& source);
		/*! Destructor */
		virtual ~TALSCalcAngleObservation();
	//@}



	/*!@name Access methods : Facade*/
	//@{
		/*! Returns the observed angle */
		virtual TAngle		getObsAngle() const {return fObsAngle;}

		/*! Returns the sigma a priori */
		virtual TAngle		getSigmaAPriori() const {return fSigmaAPriori;}

		/*! Returns the compensated angle after calculation */
		virtual TAngle		getEstimatedAngle() const {return fEstimatedAngle;}

		/*! Returns the compensated angle after calculation */
		virtual TAngle		getEstAngle() const {return fEstimatedAngle;}

		/*! Returns the sigma on the compensated distance */
		virtual TAngle		getSigmaAPosteriori() const {return fSigmaAPosteriori;}

		/*! Returns the residue after calculation */
		virtual TAngle		getResidue() const {return fResidue;}	

		/*! Returns the variance on residue after calculation */
		virtual TDouble		getResVariance() const {return fResVariance;}

		/*! Returns the residue / sigma a priori */
		virtual TDouble		getResDivSig() const;

		/*! Returns the nabla value as a TAngle */
		virtual TAngle		getNabla() const;

		/*! Returns the g value as a TAngle */
		virtual TAngle		getG() const;

	//@}


	/*!@name Access methods : Facade Default function*/
	//@{
		/*! Returns the estimated value of the angle constant as a TLenght */
	//	virtual TAngle		getAngConstValue() const {return TAngle(LITERAL(0.0));}

		/*!Return the corrected observation (obsvervation + constant)*/
		virtual TAngle		getCorrectedObsAngle() const {return TAngle(LITERAL(0.0));}

		/*!Return the stationned point Name*/
		virtual string		getStPointName() const {return string("");}

		/*!Return the target point Name*/
		virtual string		getTgPointName() const {return string("");}

		/*! Returns the geode identifiant*/
		virtual int			getId() const {return int(0);}

		/*! Returns the geode comment */
		virtual string		getComment() const {return string("");}
	//@}



	/*!@name Settings */
	//@{
		/*! Sets the residual of the computed measurement */
		virtual void	setResidual(TAngle res);

		/*! Sets a simulated value of the observed angle */
		virtual void	setSimulatedObsAngle(TAngle sObsAng);

		/*! Sets the sigma a posteriori 
		\param alpha level of significance for reliability calculation
		\param beta risk of having false values for reliability calculation */
		virtual void	setSigmaAPosteriori(TReal spost, TReal alpha, TReal beta, TReal s0);
	//@}


protected:

		/*! sets the sigma a priori */
		virtual void	setSigmaAPriori(const TAngle& s) { fSigmaAPriori = s; }

	/*!@name Protected setting method */
	//@{
		/*! Sets the variance of the residue after calculation (sigma a priori - sigma a posteriori) */
		virtual void	setResVariance(TReal s0);
		/*! Sets the estimated angle (observed + residual) */
		virtual void	setEstimatedAngle();
	//@}

private:

	TAngle		fObsAngle;	/*!< observed angle in calculation */
	TAngle		fSigmaAPriori;	/*!< sigma on observed angle in calculation */
	TAngle		fEstimatedAngle;	/*!< compensated angle after calculation */
	TAngle		fSigmaAPosteriori;	/*!< sigma on the compensated angle */
	TDouble		fResVariance; /*!< variance on the residue after calculation */ 
	TAngle		fResidue;	/*!< residue after calculation */

};

#endif //SU_ABSTRACT_LSCALC_ANGLE_OBS
