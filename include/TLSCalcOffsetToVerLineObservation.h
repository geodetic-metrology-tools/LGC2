//TLSCalcOffsetToVerLineObservation.h
/*! 
	Class for a offset (ECVE) observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				distance observations, position and length parameters)

	Copyright 2003 CERN EST/SU. All rights reserved.
*/


#ifndef LSCALC_OFFSET_ECVE_OBS
#define LSCALC_OFFSET_ECVE_OBS


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

#include "TOffsetToVerLineObservation.h"
#include "TLSCalcPosVectorParam.h" 
#include "TLSCalcLengthParam.h"

#include "TALSCalcLengthObservation.h"

/*!\ingroup lgc++
	@{*/

//! Class for a offset to vertical line observation to be used in the least square algorithms
class TLSCalcOffsetToVerLineObservation : public TALSCalcLengthObservation
{
public:
	/*!@name Constructors / Destructors */
	//@{
		/*! Default constructor */
		TLSCalcOffsetToVerLineObservation();
		/*! Constructor
		\param sdObs iterator of the spatial dist observations container  
		\param stPoint iterator for the stationned point
		\param lineTargetPoint iterator for the point used to define the target line*/
		TLSCalcOffsetToVerLineObservation(OffsetToVerLineObsConstIter obs,
			LSPosVecIter stPoint,LSPosVecIter lineTargetPoint,
			LSLengthIter cte);
		/*! Copy constructor */
		TLSCalcOffsetToVerLineObservation(const TLSCalcOffsetToVerLineObservation& source);
		/*! Destructor */
		virtual ~TLSCalcOffsetToVerLineObservation();
	//@}

	/*! Overloaded equality operator */
	bool	operator==(const TLSCalcOffsetToVerLineObservation& right);

	/*!@name Access methods : iterator*/
	//@{
		/*! Returns a iterator to the corresponding facade spatial dist observation */
		OffsetToVerLineObsConstIter	getOffsetToVerLineObs() const { return fOffsetToVerLineObs;}

		/*! Returns a pointer to the stationned point */
		LSPosVecIter		getStPoint() const {return fStPoint;} 

		/*! Returns a pointer to the first target point used to defined the target line */
		LSPosVecIter		getFirstTgPoint() const {return fTgPoint;} 

		/*!Returns an iterator of the distance cste */
		LSLengthConstIter	getDistConst() const {return fDistConst;}
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

		/*! Returns  the stationned point name */
		string		getStPointName() const {return fStPoint->getName();} 

		/*! Returns the first target point name */
		string		getFirstTgPointName() const {return fTgPoint->getName();} 


		/*! Returns the geode identifiant*/
		int		getId() const {return fOffsetToVerLineObs->getId();}

		/*! Returns the geode comment */
		string	getComment() const {return fOffsetToVerLineObs->getComment();}


		TPositionVector	getStEstCoordinate() const {return fStPoint->getEstimatedValue();}

		TPositionVector	getFirstTgEstCoordinate() const {return fTgPoint->getEstimatedValue();}

		MatrixIndex		getStXIndex() const {return fStPoint->getXIndex();}

		MatrixIndex		getStYIndex() const {return fStPoint->getYIndex();}

		MatrixIndex		getStZIndex() const {return fStPoint->getZIndex();}

		MatrixIndex		getFirstTgXIndex() const {return fTgPoint->getXIndex();}

		MatrixIndex		getFirstTgYIndex() const {return fTgPoint->getYIndex();}

		MatrixIndex		getFirstTgZIndex() const {return fTgPoint->getZIndex();}

		TALSCalcParameter::ELSStatus  getStXStatus() const { return fStPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getStYStatus() const { return fStPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getStZStatus() const { return fStPoint->getZStatus();}

		TALSCalcParameter::ELSStatus  getFirstTgXStatus() const { return fTgPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getFirstTgYStatus() const { return fTgPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getFirstTgZStatus() const { return fTgPoint->getZStatus();}

		int	getObservationID() const { return fOffsetToVerLineObs->getObservationID(); }

	//@}

private:

	/*!@name Private copy methods */
	//@{
		/*! Copy assignment operator */
		TLSCalcOffsetToVerLineObservation& operator=(const TLSCalcOffsetToVerLineObservation& right);
	//@}

	/*!@name Observation attribute for calculation */
	//@{
		OffsetToVerLineObsConstIter fOffsetToVerLineObs; /*!< iterator of the corresponding offset observation */
	//@}

	/*!@name Variables attributes for calculation */
	//@{
		LSPosVecIter		fTgPoint; /*!< iterator for the first target point used to define the target line*/
		LSPosVecIter		fStPoint; /*!< iterator for the stationned point */
		LSLengthIter		fDistConst; /*! Iterator for the distance constant */
	//@}
};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc spatial dist observations  */
typedef list< TLSCalcOffsetToVerLineObservation >		LSOffsetToVerLineContainer;
/*! Iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToVerLineContainer::iterator			LSOffsetToVerLineIter;
/*! Constant iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToVerLineContainer::const_iterator		LSOffsetToVerLineConstIter;
//@}

#endif //LSCALC_OFFSET_ECVE_OBS



