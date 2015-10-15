//TLSCalcHorDistObservation.h

/*! 
	Class for a horizontal distance observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				distance observations, position parameters)

	Copyright 2002 CERN EST/SU. All rights reserved.

*/

#ifndef LSCALC_HORDIST_OBS
#define LSCALC_HORDIST_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
using namespace std;

#include "TALSCalcLengthObservation.h"
#include "THorizontalDistObservation.h"
#include "TLSCalcPosVectorParam.h" 
#include "TLSCalcLengthParam.h" 


/*!\ingroup lgc++
	@{*/

//! Class for a horizontal distance observation to be used in the least square algorithms
class TLSCalcHorDistObservation : public TALSCalcLengthObservation
{
public:
	/*!@name Constructors / Destructors */
	//@{
	/*! Default constructor */
	TLSCalcHorDistObservation();
	/*! Constructor
	\param hdmeas horizontal distance measurement observation 
	\param st pointer to the stationned point
	\param tg pointer to the target point */
	TLSCalcHorDistObservation(HorDistObsConstIter hdmeas,LSPosVecIter st,
		LSPosVecIter tg, LSLengthIter cte);
	/*! Copy constructor */
	TLSCalcHorDistObservation(const TLSCalcHorDistObservation& source);
	/*! Destructor */
	virtual ~TLSCalcHorDistObservation();
	//@}
	
	/*! Overloaded equality operator */
	bool	operator==(const TLSCalcHorDistObservation& right);

	/*!@name Access methods : Iterator*/
	//@{
		/*! Returns a iterator to the corresponding facade hor. dist observation */
		HorDistObsConstIter	getHorDistObs() const { return fHorDistObs; }

		/*! Returns a pointer to the stationned point */
		LSPosVecIter		getStPoint() const {return fStPoint;} 

		/*! Returns a pointer to the target point */
		LSPosVecIter		getTgPoint() const {return fTgPoint;} 

		/*!Returns an iterator of the distance cste */
		LSLengthConstIter	getDistConst() const {return fDistConst;}

		/*! Returns the estimated value of the dist constant as a TLenght */
		virtual TLength		getDistConstValue() const {return getDistConst()->getEstimatedValue();}
	//@}


	/*!@name Access methods : Facade */
	//@{
		/*!Return the corrected observation (obsvervation + constant)*/
		TLength		getCorrectedObsDist() const {return getObsDist() + getProvDistConst();}

		/*! Returns the provisional value of the distance constant as a TAngle */
		TLength		getProvDistConst() const {return fDistConst->getProvisionalValue();}

		/*! Returns the estimated value of the distance constant as a TAngle */
		TLength		getEstDistConst() const {return fDistConst->getEstimatedValue();}

		/*! Returns the compensated angle after calculation  add to constante*/
		TLength		getCorrectedEstDist() const {return getEstDist()+ getEstDistConst();}

		/*!Return the stationned point Name*/
		string		getStPointName() const {return fStPoint->getName();}

		/*!Return the target point Name*/
		string		getTgPointName() const {return fTgPoint->getName();}

		/*! Returns the geode identifiant*/
		int		getId() const {return fHorDistObs->getId();}

		/*! Returns the geode comment */
		string	getComment() const {return fHorDistObs->getComment();}

		TPositionVector	getStEstCoordinate() const {return fStPoint->getEstimatedValue();}

		TPositionVector	getTgEstCoordinate() const {return fTgPoint->getEstimatedValue();}

		MatrixIndex		getStXIndex() const {return fStPoint->getXIndex();}

		MatrixIndex		getStYIndex() const {return fStPoint->getYIndex();}

		MatrixIndex		getStZIndex() const {return fStPoint->getZIndex();}

		MatrixIndex		getTgXIndex() const {return fTgPoint->getXIndex();}

		MatrixIndex		getTgYIndex() const {return fTgPoint->getYIndex();}

		MatrixIndex		getTgZIndex() const {return fTgPoint->getZIndex();}

		MatrixIndex		getDistConstIndex() const {return getDistConst()->getIndex();}


		TALSCalcParameter::ELSStatus  getStXStatus() const { return fStPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getStYStatus() const { return fStPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getStZStatus() const { return fStPoint->getZStatus();}

		TALSCalcParameter::ELSStatus  getTgXStatus() const { return fTgPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getTgYStatus() const { return fTgPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getTgZStatus() const { return fTgPoint->getZStatus();}

		TALSCalcParameter::ELSStatus  getDistConstStatus() const { return getDistConst()->getStatus();}

		int	getObservationID() const { return fHorDistObs->getObservationID(); }
		
		
		
	//@}

private:

	/*!@name Private copy methods */
	//@{
	/*! Copy assignment operator */
	TLSCalcHorDistObservation& operator=(const TLSCalcHorDistObservation& right);
	//@}

	/*!@name Observation attribute for calculation */
	//@{
	HorDistObsConstIter fHorDistObs; /*!< iterator of the corresponding horizontal distance observation */
	//@}

	/*!@name Variables attributes for calculation */
	//@{
	LSPosVecIter		fTgPoint; /*!< iterator for the target point */
	LSPosVecIter		fStPoint; /*!< iterator for the stationned point */
	LSLengthIter		fDistConst; /*! Iterator for the distance constant */
	//@}

};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc horizontal dist observations  */
typedef list< TLSCalcHorDistObservation >		LSHorDistContainer;
/*! Iterator of the list of LSCalc horizontal dist observations */
typedef LSHorDistContainer::iterator				LSHorDistIter;
/*! Constant iterator of the list of LSCalc horizontal dist observations */
typedef LSHorDistContainer::const_iterator			LSHorDistConstIter;
//@}

#endif //LSCALC_HORDIST_OBS

