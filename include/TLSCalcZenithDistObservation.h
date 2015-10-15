//TLSCalcZenithDistObservation.h

/*! 
	Class for a zenithal angle observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				angle observations, position and length parameters)

	Copyright 2002 CERN EST/SU. All rights reserved.

*/

#ifndef LSCALC_ZENDIST_OBS
#define LSCALC_ZENDIST_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
using namespace std;

#include "TLSCalcPosVectorParam.h" 
#include "TLSCalcLengthParam.h" 
#include "TZenithDistObservation.h"

#include "TALSCalcAngleObservation.h"

/*!\ingroup lgc++
	@{*/

//! Class for a zenithal observation to be used in least square algorithms
class TLSCalcZenithDistObservation : public TALSCalcAngleObservation
{
public:
	/*!@name Constructors / Destructors */
	//@{
		/*! Default constructor */
		TLSCalcZenithDistObservation();

		/*! Constructor
		\param zdObs iterator to a zenithal distance observation
		\param hi iterator to the instrument's height
		\param hg prism's height
		\param st iterator to the stationned point
		\param tg iterator to the target point */
		TLSCalcZenithDistObservation(ZenDistObsConstIter zdObs,LSLengthIter hi,TLength hp,
			LSPosVecIter st,LSPosVecIter tg);

		/*! Copy constructor */
		TLSCalcZenithDistObservation(const TLSCalcZenithDistObservation& source);

		/*! Destructor */
		virtual ~TLSCalcZenithDistObservation();
	//@}


	/*! Overloaded equality operator */
	virtual bool operator==(const TLSCalcZenithDistObservation& right);


	/*!@name Access methods : iterator */
	//@{
		/*! Returns a iterator to the corresponding facade zenith dist observation */
		ZenDistObsConstIter	getZenDistObs() const { return fZenDistObs; }

		/*! Returns an iterator to the instrument's height */
		LSLengthConstIter	getHInstrument() const {return fInstrumentHeight;}

		/*! Returns an iterator to the stationned point */
		LSPosVecIter		getStPoint() const {return fStPoint;}
		
		/*! Returns an iterator to the target point */
		LSPosVecIter		getTgPoint() const  {return fTgPoint;}
	//@}


	/*!@name Access methods : Facade */
	//@{
		/*! Returns the prism's height */
		TLength				getHPrism() const {return fPrismHeight;}

		/*!Return the corrected observation (obsvervation + constant)*/
		TAngle		getCorrectedObsAngle() const {return getObsAngle();}

		/*!Return the stationned point Name*/
		string		getStPointName() const {return fStPoint->getName();}

		/*!Return the target point Name*/
		string		getTgPointName() const {return fTgPoint->getName();}

		/*!return the estimated instrument's height as a Tlength*/
		TLength		getEstimatedHInstr() const	{ return hasOwnInstrumentHeight() ? getAllPointsFixedInstrumentHeight() : fInstrumentHeight->getEstimatedValue(); }

		/*!return the status of instrument's height (ZENI or ZENH)*/
		TALSCalcParameter::ELSStatus		getHInstrStatus() const	{return getHInstrument()->getStatus();}

		/*!return the movement of the target (length)*/
		TLength		getTgMovement() const;

		/*! Returns the geode identifiant*/
		int			getId() const {return fZenDistObs->getId();}

		/*! Returns the geode comment */
		string		getComment() const {return fZenDistObs->getComment();}

		TPositionVector	getStEstCoordinate() const {return fStPoint->getEstimatedValue();}

		TPositionVector	getTgEstCoordinate() const {return fTgPoint->getEstimatedValue();}

		MatrixIndex		getStXIndex() const {return fStPoint->getXIndex();}

		MatrixIndex		getStYIndex() const {return fStPoint->getYIndex();}

		MatrixIndex		getStZIndex() const {return fStPoint->getZIndex();}

		MatrixIndex		getTgXIndex() const {return fTgPoint->getXIndex();}

		MatrixIndex		getTgYIndex() const {return fTgPoint->getYIndex();}

		MatrixIndex		getTgZIndex() const {return fTgPoint->getZIndex();}

		MatrixIndex		getHInstIndex() const {return getHInstrument()->getIndex();}

		TALSCalcParameter::ELSStatus	getStXStatus() const { return fStPoint->getXStatus();}

		TALSCalcParameter::ELSStatus	getStYStatus() const { return fStPoint->getYStatus();}

		TALSCalcParameter::ELSStatus	getStZStatus() const { return fStPoint->getZStatus();}

		TALSCalcParameter::ELSStatus	getTgXStatus() const { return fTgPoint->getXStatus();}

		TALSCalcParameter::ELSStatus	getTgYStatus() const { return fTgPoint->getYStatus();}

		TALSCalcParameter::ELSStatus	getTgZStatus() const { return fTgPoint->getZStatus();}

		TALSCalcParameter::ELSStatus	getHInstStatus() const {return getHInstrument()->getStatus();}

		int	getObservationID() const { return fZenDistObs->getObservationID(); }


		
	//@}
		void setHasOwnInstrumentHeight() { fHasOwnInstrumentHeight = true; }
		bool hasOwnInstrumentHeight() const { return fHasOwnInstrumentHeight; }
		TLength getAllPointsFixedInstrumentHeight() const { return fAllPointsFixedInstrumentHeight; }
		void setAllPointsFixedInstrumentHeight(TLength newIH) { fAllPointsFixedInstrumentHeight = newIH; }



private:

	/*!@name Private copy method */

	//@{
	/*! Copy assignment operator */
		TLSCalcZenithDistObservation& operator=(const TLSCalcZenithDistObservation& right);
	//@}

	/*!@name Observation attributes for calculation */
	//@{
		ZenDistObsConstIter fZenDistObs; /*!< iterator to the corresponding zenithal angle observation (facade to spatial measurements) */
	//@}

	/*!@name Variables attributes for calculation */
	//@{
		bool				fHasOwnInstrumentHeight;
		TLength				fAllPointsFixedInstrumentHeight; // instrument height for the observation when all the points are fixed
		LSLengthIter		fInstrumentHeight; /*!< iterator to the instrument's height */
		TLength				fPrismHeight; /*!< prism's height */
		LSPosVecIter		fTgPoint; /*!< iterator to the target point */
		LSPosVecIter		fStPoint; /*!< iterator to the stationned point */
	//@}
	
};

/*@}*/

/*!@name Constants definition */
//@{
	/*! List of LSCalc zenithal angle observations  */
	typedef list< TLSCalcZenithDistObservation >	LSZenDistContainer;
	/*! Iterator of the list of LSCalc zenithal angle observations */
	typedef LSZenDistContainer::iterator			LSZenDistIter;
	/*! Constant iterator of the list of LSCalc zenithal angle observations */
	typedef LSZenDistContainer::const_iterator		LSZenDistConstIter;
//@}

#endif //LSCALC_ZENDIST_OBS

