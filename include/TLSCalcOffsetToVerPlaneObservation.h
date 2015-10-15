//TLSCalcOffsetToVerPlaneObservation.h
/*! 
	Class for a offset (ECHO) observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				distance observations, position and length parameters)

	Copyright 2003 CERN EST/SU. All rights reserved.
*/


#ifndef LSCALC_OFFSET_ECHO_OBS
#define LSCALC_OFFSET_ECHO_OBS


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)
#endif // _MSC_VER >= 1000

#ifdef _DEBUG
#define TLSCalcOffsetToVerPlaneObservation TLSCECHOOBS
#endif

#include <list>
#include <string>
using namespace std;

#include "TOffsetToVerPlaneObservation.h"
#include "TLSCalcPosVectorParam.h" 
#include "TLSCalcLengthParam.h"

#include "TALSCalcLengthObservation.h"

/*!\ingroup lgc++
	@{*/

//! Class for a offset to vertical Plane observation to be used in the least square algorithms
class TLSCalcOffsetToVerPlaneObservation : public TALSCalcLengthObservation
{
public:
	/*!@name Constructors / Destructors */
	//@{
		/*! Default constructor */
		TLSCalcOffsetToVerPlaneObservation();

		/*! Constructor
		\param sdObs iterator of the spatial dist observations container  
		\param stPoint iterator for the stationned point
		\param firstPlaneTargetPoint iterator for the first point used to define the target Plane
		\param secondPlaneTargetPoint iterator for the second point used to define the target Plane*/
		TLSCalcOffsetToVerPlaneObservation(OffsetToVerPlaneObsConstIter obs,
			LSPosVecIter stPoint,
			LSPosVecIter firstPlaneTargetPoint, LSPosVecIter secondPlaneTargetPoint,
			LSLengthIter cte);

		/*! Copy constructor */
		TLSCalcOffsetToVerPlaneObservation(const TLSCalcOffsetToVerPlaneObservation& source);

		/*! Destructor */
		virtual ~TLSCalcOffsetToVerPlaneObservation();
	//@}

	/*! Overloaded equality operator */
	bool	operator==(const TLSCalcOffsetToVerPlaneObservation& right);

	/*!@name Access methods : iterator*/
	//@{
		/*! Returns a iterator to the corresponding facade spatial dist observation */
		OffsetToVerPlaneObsConstIter	getOffsetToVerPlaneObs() const { return fOffsetToVerPlaneObs; }
	
		/*! Returns a pointer to the stationned point */
		LSPosVecIter		getStPoint() const {return fStPoint;}
		
		/*! Returns a pointer to the first target point used to defined the target Plane */
		LSPosVecIter		getFirstTgPoint() const {return fFirstTgPoint;}
		
		/*! Returns a pointer to the second target point used to defined the target Plane */
		LSPosVecIter		getSecondTgPoint() const {return fSecondTgPoint;}
		
		/*!Returns an iterator of the distance cste */
		LSLengthConstIter	getDistConst() const {	return fDistConst;}
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
		string		getFirstTgPointName() const {return fFirstTgPoint->getName();} 

		/*! Returns the first target point name */
		string		getSecondTgPointName() const {return fSecondTgPoint->getName();} 


		/*! Returns the geode identifiant*/
		int		getId() const {return fOffsetToVerPlaneObs->getId();}

		/*! Returns the geode comment */
		string	getComment() const {return fOffsetToVerPlaneObs->getComment();}


		TPositionVector	getStEstCoordinate() const {return fStPoint->getEstimatedValue();}

		TPositionVector	getFirstTgEstCoordinate() const {return fFirstTgPoint->getEstimatedValue();}

		TPositionVector	getSecondTgEstCoordinate() const {return fSecondTgPoint->getEstimatedValue();}

		MatrixIndex		getStXIndex() const {return fStPoint->getXIndex();}

		MatrixIndex		getStYIndex() const {return fStPoint->getYIndex();}

		MatrixIndex		getStZIndex() const {return fStPoint->getZIndex();}

		MatrixIndex		getFirstTgXIndex() const {return fFirstTgPoint->getXIndex();}

		MatrixIndex		getFirstTgYIndex() const {return fFirstTgPoint->getYIndex();}

		MatrixIndex		getFirstTgZIndex() const {return fFirstTgPoint->getZIndex();}

		MatrixIndex		getSecondTgXIndex() const {return fSecondTgPoint->getXIndex();}

		MatrixIndex		getSecondTgYIndex() const {return fSecondTgPoint->getYIndex();}

		MatrixIndex		getSecondTgZIndex() const {return fSecondTgPoint->getZIndex();}


		TALSCalcParameter::ELSStatus  getStXStatus() const { return fStPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getStYStatus() const { return fStPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getStZStatus() const { return fStPoint->getZStatus();}

		TALSCalcParameter::ELSStatus  getFirstTgXStatus() const { return fFirstTgPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getFirstTgYStatus() const { return fFirstTgPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getFirstTgZStatus() const { return fFirstTgPoint->getZStatus();}

		TALSCalcParameter::ELSStatus  getSecondTgXStatus() const { return fSecondTgPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getSecondTgYStatus() const { return fSecondTgPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getSecondTgZStatus() const { return fSecondTgPoint->getZStatus();}

		int	getObservationID() const { return fOffsetToVerPlaneObs->getObservationID(); }





	//@}

private:

	/*!@name Private copy methods */
	//@{
		/*! Copy assignment operator */
		TLSCalcOffsetToVerPlaneObservation& operator=(const TLSCalcOffsetToVerPlaneObservation& right);
	//@}

	/*!@name Observation attribute for calculation */
	//@{
		OffsetToVerPlaneObsConstIter fOffsetToVerPlaneObs; /*!< iterator of the corresponding offset observation */
	//@}

	/*!@name Variables attributes for calculation */
	//@{
		LSPosVecIter		fFirstTgPoint; /*!< iterator for the first target point used to define the target Plane*/
		LSPosVecIter		fSecondTgPoint; /*!< iterator for the second target point used to define the target Plane*/
		LSPosVecIter		fStPoint; /*!< iterator for the stationned point */
		LSLengthIter		fDistConst; /*! Iterator for the distance constant */
	//@}
};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc spatial dist observations  */
typedef list< TLSCalcOffsetToVerPlaneObservation >		LSOffsetToVerPlaneContainer;
/*! Iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToVerPlaneContainer::iterator			LSOffsetToVerPlaneIter;
/*! Constant iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToVerPlaneContainer::const_iterator		LSOffsetToVerPlaneConstIter;
//@}

#endif //LSCALC_OFFSET_ECHO_OBS



