//TLSCalcVertDistObservation.h

/*! 
	Class for a vertical distance observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				distance observations, position parameters)

	Copyright 2002 CERN EST/SU. All rights reserved.

*/

#ifndef LSCALC_VERTDIST_OBS
#define LSCALC_VERTDIST_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
using namespace std;

#include "TVertDistObservation.h"
#include "TLSCalcPosVectorParam.h" 
#include "TLSCalcLengthParam.h" 

#include "TALSCalcLengthObservation.h"

/*!\ingroup lgc++
	@{*/

//!	Class for a vertical distance observation to be used in the least square algorithms.
class TLSCalcVertDistObservation : public TALSCalcLengthObservation
{
public:
	/*!@name Constructors / Destructors */
	//@{
	/*! Default constructor */
	TLSCalcVertDistObservation();
	/*! Constructor
	\param vdmeas vertical distance observation 
	\param ref pointer to the reference point */
	TLSCalcVertDistObservation(VertDistObsConstIter vdmeas,LSPosVecIter ref,LSPosVecIter tg, LSLengthIter cte);
	/*! Copy constructor */
	TLSCalcVertDistObservation(const TLSCalcVertDistObservation& source);
	/*! Destructor */
	virtual ~TLSCalcVertDistObservation();
	//@}

	/*! Overloaded equality operator */
	bool	operator==(const TLSCalcVertDistObservation& right);

	/*!@name Access methods */
	//@{
		/*! Returns a iterator to the corresponding facade vert. dist observation */
		VertDistObsConstIter	getVertDistObs() const { return fVertDistObs; }

		/*! Returns the reference point */
		LSPosVecIter			getRefPoint() const {return fRefPoint;} 

		/*! Returns the target point */
		LSPosVecIter			getTgPoint() const {return fTgPoint;}

		/*!Returns an iterator of the distance cste */
		LSLengthConstIter		getDistConst() const {return fDistConst;}

		/*! Returns the estimated value of the dist constant as a TLenght */
		virtual TLength			getDistConstValue() const {return getDistConst()->getEstimatedValue();}
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
		string		getRefPointName() const {return fRefPoint->getName();}

		/*!Return the target point Name*/
		string		getTgPointName() const {return fTgPoint->getName();}

		/*! Returns the geode identifiant*/
		int		getId() const {return fVertDistObs->getId();}

		/*! Returns the geode comment */
		string	getComment() const {return fVertDistObs->getComment();}


		TPositionVector	getRefEstCoordinate() const {return fRefPoint->getEstimatedValue();}

		TPositionVector	getTgEstCoordinate() const {return fTgPoint->getEstimatedValue();}

		MatrixIndex		getRefXIndex() const {return fRefPoint->getXIndex();}

		MatrixIndex		getRefYIndex() const {return fRefPoint->getYIndex();}

		MatrixIndex		getRefZIndex() const {return fRefPoint->getZIndex();}

		MatrixIndex		getTgXIndex() const {return fTgPoint->getXIndex();}

		MatrixIndex		getTgYIndex() const {return fTgPoint->getYIndex();}

		MatrixIndex		getTgZIndex() const {return fTgPoint->getZIndex();}


		TALSCalcParameter::ELSStatus  getRefXStatus() const { return fRefPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getRefYStatus() const { return fRefPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getRefZStatus() const { return fRefPoint->getZStatus();}

		TALSCalcParameter::ELSStatus  getTgXStatus() const { return fTgPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getTgYStatus() const { return fTgPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getTgZStatus() const { return fTgPoint->getZStatus();}

		int	getObservationID() const { return fVertDistObs->getObservationID(); }


	//@}

private:

	/*!@name Private copy methods */
	//@{
	/*! Copy assignment operator */
	TLSCalcVertDistObservation& operator=(const TLSCalcVertDistObservation& right);
	//@}

	/*!@name Observation attribute for calculation */
	//@{
	VertDistObsConstIter fVertDistObs; /*!< iterator of the corresponding vertical distance observation */
	//@}

	/*!@name Variables attributes for calculation */
	//@{
	LSPosVecIter	fRefPoint; /*!< a pointer to the reference point */
	LSPosVecIter	fTgPoint; /*!< a pointer to the target point */
	LSLengthIter	fDistConst; /*! Iterator for the distance constant */
	//@}
};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc vertical dist observations  */
typedef list< TLSCalcVertDistObservation >			LSVertDistContainer;
/*! Iterator of the list of LSCalc vertical dist observations */
typedef LSVertDistContainer::iterator				LSVertDistIter;
/*! Constant iterator of the list of LSCalc vertical dist observations */
typedef LSVertDistContainer::const_iterator			LSVertDistConstIter;
//@}

#endif //LSCALC_VERTDIST_OBS

