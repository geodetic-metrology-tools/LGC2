//TALSCalcLengthObservation.h

/*! 
	Abstract Class for length observations used in the least square algorithms. It defines the
	the basic informations concerning the observation (observed value, sigma...).

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				angle observations, position and angle parameters)

	Copyright 2003 CERN EST/SU. All rights reserved.

*/

#ifndef SU_ABSTRACT_LSCALC_LENGTH_OBS
#define SU_ABSTRACT_LSCALC_LENGTH_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TALSCalcObservation.h"
#include "TLength.h"
#include "TDouble.h"

//! Abstract Class for angle observations used in the least square algorithms
class TALSCalcLengthObservation : public TALSCalcObservation
{

public:

	/*!@name Constructor / Destructor
	//@
		! Default constructor */
		TALSCalcLengthObservation();

		/*! Cosntructor
		\param obsDist observed value 
		\param sAPriori sigma a priori on the observation */
		TALSCalcLengthObservation(TLength obsDist, TLength sAPriori);

		/*! Copy constructor */
		TALSCalcLengthObservation(const TALSCalcLengthObservation& source);

		/*! Destructor */
		virtual ~TALSCalcLengthObservation();
	//@}



	/*!@name Access methods */
	//@{
		/*! Returns the estimated value of the dist constant as a TLenght */
		virtual TLength				getDistConstValue() const {return TLength(LITERAL(0.0));}

		/*! Returns the sigma a priori */
		virtual TLength				getSigmaAPriori() const {return fSigmaAPriori;}

		/*! Returns the compensated distance after calculation */
		virtual TLength				getEstimatedDist() const {return fEstimatedDist;}

		/*! Returns the sigma on the compensated distance */
		virtual TLength				getSigmaAPosteriori() const {return fSigmaAPosteriori;}

		/*! Returns the residue after calculation */
		virtual TLength				getResidue() const {return fResidue;}
		
		/*! Returns the residual variance */
		virtual TDouble				getResVariance() const {return fResVariance;}
	//@}



	/*!@name Access methods : Facade */
	//@{
		/*! Returns the residue / sigma a priori */
		TDouble		getResDivSig() const;

		/*! Returns the observed distance */
		TLength		getObsDist() const {return fObsDist;}

		/*! Returns the compensated distance after calculation */
		TLength		getEstDist() const {return fEstimatedDist;}

		/*! Returns the nabla value as a TLength */
		virtual TLength	getNabla() const;

		/*! Returns the g value as a TLength */
		virtual TLength getG() const;

	//@}

	/*!@name Access methods : Facade Default function*/
	//@{
		/*!Return the corrected observation (obsvervation + constant)*/
		virtual TLength		getCorrectedObsDist() const {return TLength(LITERAL(0.0));}

		/*! Returns the geode identifiant*/
		virtual int			getId() const {return int(0);}

		/*! Returns the geode comment */
		virtual string		getComment() const {return string("");}
	//@}


	/*!@name Settings */
	//@{
		/*! Sets the sigma a posteriori 
		\param alpha level of significance for reliability calculation
		\param beta risk of having false values for reliability calculation */
		virtual void	setSigmaAPosteriori(TReal  spost, TReal alpha, TReal beta, TReal s0);

		/*! Sets the residual of the computed measurement */
		virtual void	setResidual(TLength res);

		/*! Sets a simulated value of the observed distance */
		virtual void	setSimulatedObsDist(TLength sObsDist);
	//@}



		
protected:

		/*! sets the sigma a priori */
		virtual void	setSigmaAPriori(const TLength& s) { fSigmaAPriori = s; }

	/*!@name Protected setting method */
	//@{
		/*! Sets the residual variance (s2 a priori - s2 a posteriori) 
		@param s0 s0 a posteriori to be applied */
		virtual void		setResVariance(TReal s0);

		/*! Sets the estimated distance (observed + residual) */
		virtual void		setEstimatedDist();
	//@}

private:

	TLength		fObsDist;	/*!< observed distance in calculation */
	TLength		fSigmaAPriori;	/*!< sigma on observed distance in calculation */
	TLength		fEstimatedDist;	/*!< compensated distance after calculation */
	TLength		fSigmaAPosteriori;	/*!< sigma on the compensated distance */
	TLength		fResidue;	/*!< residue after calculation */
	TDouble		fResVariance; /*!< variance on the residue after calculation */

};

#endif //SU_ABSTRACT_LSCALC_LENGTH_OBS
