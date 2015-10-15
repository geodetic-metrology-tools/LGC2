//TLSCalcOffsetToVerticalLineObservation.h
/*! 
	Class for a offset (ECVE, ECSP, SCHO) observations, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				distance observations, position and length parameters)

	Copyright 2003 CERN EST/SU. All rights reserved.
*/


#ifndef LSCALC_OFFSET_LINE_OBS
#define LSCALC_OFFSET_LINE_OBS


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)
#endif // _MSC_VER >= 1000

#ifdef _DEBUG
#define TLSCalcOffsetToVerLineObservation TLSCECVEOBS
#endif

#include <list>
#include <string>
using namespace std;

#include "TOffsetToLineOrPlaneMeasurement.h"
#include "lsalgo/TLSCalcPosVectorParam.h" 
#include "lsalgo/TLSCalcFreeVectorParam.h" 
#include "lsalgo/TLSCalcLengthParam.h"

#include "TALSCalcLengthObservation.h"

/*!\ingroup lgc++
	@{*/

//! Class for a offset to vertical line observation to be used in the least square algorithms
class TLSCalcOffsetToVerticalLineObservation : public TALSCalcLengthObservation
{
public:
	/*!@name Constructors / Destructors */
	//@{
		TLSCalcOffsetToVerticalLineObservation(const TOffsetToLineOrPlaneMeasurement* obs,
			LSPosVecIter target,
			LSPosVecIter origin,
			LSPosVecIter pointOnLine,
			LSFreeVecIter unit,
			LSLengthIter cte);

		~TLSCalcOffsetToVerticalLineObservation()
		{
		}


	/*!@name Access methods : iterator*/
	//@{
		/*! Returns a iterator to the corresponding facade spatial dist observation */
		const TOffsetToLineOrPlaneMeasurement*	getOffsetToLineObs() const { return fOffsetToLineObs;}
		
		/*! Returns a pointer to the measured point */
		LSPosVecIter		getTg() const {return fTgPoint;}

		/*!Returns an iterator of the distance cste */
		LSLengthConstIter	getDistConst() const {return fDistConst;}

		LSPosVecIter getPointOnLine() const { return pointOnLine; }
		LSFreeVecIter getUnitVector() const { return fUnitVector; }
		LSPosVecIter getOrigin() const { return origin; }
	//@}



	/*!@name Access methods : facade */
	//@{
		/*! Returns the estimated value of the dist constant as a TLenght */
		virtual TLength		getDistConstValue() const {return getDistConst()->getEstimatedValue();}

		/*! Returns the estimated value of the dist constant as a TLength */
		TLength		getEstDistConst() const {return getDistConst()->getEstimatedValue();}

		/*! Returns the provisional value of the distance constant as a TLength */
		TLength		getProvDistConst() const {return getDistConst()->getProvisionalValue();}

		/*!Return the corrected observation (obsvervation + constant)*/
		TLength		getCorrectedObsDist() const {return getObsDist() + getProvDistConst();}

		/*! Returns the compensated angle after calculation  add to constante*/
		TLength		getCorrectedEstDist() const {return getEstDist()+ getEstDistConst();}

		/*! Returns  the measured point name */
		string		getTgName() const {return fTgPoint->getName();} 



		/*! Returns the geode identifiant*/
		int		getId() const {return fOffsetToLineObs->getId();}

		/*! Returns the geode comment */
		string	getComment() const {return fOffsetToLineObs->getComment();}


		TPositionVector	getTgEstCoordinate() const {return fTgPoint->getEstimatedValue();}

		MatrixIndex		getTgXIndex() const {return fTgPoint->getXIndex();}

		MatrixIndex		getTgYIndex() const {return fTgPoint->getYIndex();}

		MatrixIndex		getTgZIndex() const {return fTgPoint->getZIndex();}

		TALSCalcParameter::ELSStatus  getTgXStatus() const { return fTgPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getTgYStatus() const { return fTgPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getTgZStatus() const { return fTgPoint->getZStatus();}

		int	getObservationID() const { return fOffsetToLineObs->getObservationID(); }

	//@}
bool	operator==(const TLSCalcOffsetToVerticalLineObservation& right);

private:

	void calculateSigma();

	/*!@name Observation attribute for calculation */
	//@{
		const TOffsetToLineOrPlaneMeasurement* fOffsetToLineObs; /*!< iterator of the corresponding offset observation */
		LSPosVecIter pointOnLine;
		LSFreeVecIter fUnitVector;
		LSPosVecIter origin;
		//const TOffsetToLineOrPlaneMeasurement* fOriginPointMeasurement; /*!< iterator for the origin point used to define the target line*/
		
	//@}

	/*!@name Variables attributes for calculation */
	//@{
		LSPosVecIter		fTgPoint; /*!< iterator for the target point used */
		LSLengthIter		fDistConst; /*! Iterator for the distance constant */
	//@}
};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc spatial dist observations  */
typedef list< TLSCalcOffsetToVerticalLineObservation >			LSOffsetToVerLineContainer;
/*! Iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToVerLineContainer::iterator			LSOffsetToVerLineIter;
/*! Constant iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToVerLineContainer::const_iterator		LSOffsetToVerLineConstIter;
//@}

#endif //LSCALC_OFFSET_LINE_OBS



