//TLSCalcRadialOffsetCnstrObservation.h
/*! 
	Class for a offset (RADI) observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				distance observations, position and length parameters)

	Copyright 2003 CERN EST/SU. All rights reserved.
*/


#ifndef LSCALC_OFFSET_RADI_OBS
#define LSCALC_OFFSET_RADI_OBS


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)
#endif // _MSC_VER >= 1000

#ifdef _DEBUG
#define TLSCalcRadialOffsetCnstrObservation TLSCRADIOBS
#endif

#include <list>
#include <string>
using namespace std;

#include "TRadialOffsetCnstrObservation.h"
#include "TLSCalcPosVectorParam.h" 
#include "THorAngleObservation.h"
#include "TLSCalcOrientationParam.h"
#include "TLSCalcLengthParam.h" 

#include "TALSCalcObservation.h"

/*!\ingroup lgc++
	@{*/

//! Class for a offset to vertical Plane observation to be used in the least square algorithms
class TLSCalcRadialOffsetCnstrObservation : public TALSCalcObservation
{
public:
	/*!@name Constructors / Destructors */
	//@{
		/*! Default constructor */
		TLSCalcRadialOffsetCnstrObservation();

		/*! Constructor*/
		TLSCalcRadialOffsetCnstrObservation(RadOffCnstrObsConstIter obs, LSPosVecIter point);

		/*! Copy constructor */
		TLSCalcRadialOffsetCnstrObservation(const TLSCalcRadialOffsetCnstrObservation& source);

		/*! Destructor */
		virtual ~TLSCalcRadialOffsetCnstrObservation();
	//@}

	/*! Overloaded equality operator */
	bool	operator==(const TLSCalcRadialOffsetCnstrObservation& right);

	/*!@name Access methods : iterator*/
	//@{
		/*! Returns a iterator to the corresponding facade spatial dist observation */
		RadOffCnstrObsConstIter	getRadialOffsetCnstrObs() const { return fRadialOffsetCnstrObs;}

		/*! Returns a pointer to the point */
		LSPosVecIter			getPoint() const {return fPoint;}
	//@}


	/*!@name Access methods : facade */
	//@{
		/*! Returns the numerical value of residue */
		virtual TReal getResValue() const { return getResidue().getMetresValue(); }

		/*! Returns the numerical value of residual variance */
		virtual TReal getResVarValue() const { return getResVariance().getValue(); }

		/*! Returns the numerical value of sigma a posteriori */
		virtual TReal getSAPostValue() const { return getSigmaAPosteriori().getMetresValue(); }

		/*! Returns the nabla value as a TLength */
		TLength			getNabla() const;

		/*! Returns the g value as a TLength */
		TLength			getG() const;

		/*!Returne the point's name*/
		string			getPointName() const {return getPoint()->getName();}

		/*! Returns the residue / sigma a priori */
		TDouble			getResDivSig() const;

		/*! Returns the sigma a priori */
		TLength			getSigmaAPriori() const {return fSigmaAPriori;}

		/*! Returns the sigma on the compensated offset */
		TLength			getSigmaAPosteriori() const  {return fSigmaAPosteriori;}

		/*! residue after calculation */
		TLength			getResidue() const {return fResidue;}
		
		/*! Returns the variance on residue after calculation */
		TDouble			getResVariance() const {return fResVariance;}
		
		/*! Returns a pointer to the angle used to defined the bearing */
		TAngle			getBearing() const {return fRadialOffsetCnstrObs->getRadialOffsetCnstrBearing();}

		/*! Returns the geode identifiant*/
		int				getId() const {return fRadialOffsetCnstrObs->getId();}

		/*! Returns the geode comment */
		string			getComment() const {return fRadialOffsetCnstrObs->getComment();}

		TPositionVector	getPtEstCoordinate() const {return getPoint()->getEstimatedValue();}

		TPositionVector	getPtProvCoordinate() const {return getPoint()->getProvisionalValue();}

		MatrixIndex	getPtXIndex() const { return getPoint()->getXIndex();}

		MatrixIndex	getPtYIndex() const { return getPoint()->getYIndex();}

		MatrixIndex	getPtZIndex() const { return getPoint()->getZIndex();}

		TALSCalcParameter::ELSStatus  getPtXStatus() const { return getPoint()->getXStatus();}

		TALSCalcParameter::ELSStatus  getPtYStatus() const { return getPoint()->getYStatus();}

		TALSCalcParameter::ELSStatus  getPtZStatus() const { return getPoint()->getZStatus();}

		int	getObservationID() const { return fRadialOffsetCnstrObs->getObservationID(); }


	//@}


	/*!@name Setting methods */
	//@{
		/*! Sets the sigma a posteriori */
		void				setSigmaAPosteriori(TReal spost, TReal alpha, TReal beta, TReal s0);
		/*! Sets the residual of the computed measurement */
		void				setResidual(TLength res);
	//@}

protected:
	/*!@name Protected setting method */
	//@{
		/*! Sets the variance of the residue after calculation (sigma a priori - sigma a posteriori) */
		void		setResVariance(TReal s0);
	//@}

private:

	/*!@name Private copy methods */
	//@{
		/*! Copy assignment operator */
		TLSCalcRadialOffsetCnstrObservation& operator=(const TLSCalcRadialOffsetCnstrObservation& right);
	//@}

	/*!@name Observation attributes for calculation */
	//@{
		TLength						fSigmaAPriori;	/*!< sigma on observed offset in calculation */
		TLength						fSigmaAPosteriori;	/*!< sigma on the offset distance */
		TLength						fResidue;	/*!< residue after calculation */
		TDouble						fResVariance; /*!< variance on the residue after calculation */ 
		RadOffCnstrObsConstIter		fRadialOffsetCnstrObs; /*!< iterator of the corresponding offset observation */
	//@}

	/*!@name Variables attributes for calculation */
	//@{
		LSPosVecIter		fPoint; /*!< iterator for the first target point used to define the target Plane*/
	//@}
};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc spatial dist observations  */
typedef list< TLSCalcRadialOffsetCnstrObservation >		LSRadOffCnstrContainer;
/*! Iterator of the list of LSCalc spatial dist observations */
typedef LSRadOffCnstrContainer::iterator				LSRadOffCnstrIter;
/*! Constant iterator of the list of LSCalc spatial dist observations */
typedef LSRadOffCnstrContainer::const_iterator			LSRadOffCnstrConstIter;
//@}

#endif //LSCALC_OFFSET_RADI_OBS



