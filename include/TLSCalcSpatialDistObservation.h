//TLSCalcSpatialDistObservation.h

/*! 
	Class for a spatial distance observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				distance observations, position and length parameters)

	Copyright 2002 CERN EST/SU. All rights reserved.

*/

#ifndef LSCALC_SPADIST_OBS
#define LSCALC_SPADIST_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
using namespace std;

#include "TSpatialDistMeasurement.h"
#include "lsalgo/TLSCalcPosVectorParam.h" 
#include "lsalgo/TLSCalcLengthParam.h"

#include "TALSCalcLengthObservation.h"
#include "TheodoliteStation.h"
#include "EDMTarget.h"

/*!\ingroup lgc++
	@{*/

//! Class for a spatial distance observation to be used in the least square algorithms
template <typename T>
class TLSCalcSpatialDistObservation : public TALSCalcLengthObservation
{
public:
	/*!@name Constructors / Destructors */
	//@{

		/*! Constructor
		\param sdObs iterator of the spatial dist observations container  
		\param hi iterator for the instrument's height
		\param hp prism's height
		\param st iterator for the stationned point
		\param tg iterator for the target point */
		TLSCalcSpatialDistObservation(const TSpatialDistMeasurement<T>* sdObs,LSLengthIter hi,TLength hp,
			LSLengthIter cst,LSPosVecIter st,LSPosVecIter tg, const Station* theoStation):
				TALSCalcLengthObservation(*(sdObs->getObservedValue()),	TLength(0)),
				fSpaDistObs(sdObs),fInstrumentHeight(hi),fPrismHeight(hp),fDistConst(cst),fStPoint(st),
				fTgPoint(tg), fHasOwnInstrumentHeight(false), fAllPointsFixedInstrumentHeight(TLength(0)) 
		{
			tStation = theoStation;
		}

		~TLSCalcSpatialDistObservation()
		{
		}

	//@}

		void calculateSigma()
		{
			const TPositionVector& st = fStPoint->getEstimatedValue();
			const TPositionVector& tg = fTgPoint->getEstimatedValue();

			TReal dz = tg.getZ().getMetresValue() - st.getZ().getMetresValue();
			TReal dxy = sqrtq(powq(tg.getX().getMetresValue() - st.getX().getMetresValue(), 2) +
				powq(tg.getY().getMetresValue() - st.getY().getMetresValue(), 2));
			TReal dxyz = sqrtq(powq(tg.getX().getMetresValue() - st.getX().getMetresValue(), 2) +
				powq(tg.getY().getMetresValue() - st.getY().getMetresValue(), 2) +
				powq(tg.getZ().getMetresValue() - st.getZ().getMetresValue(), 2));

			TReal a = powq((dz - fInstrumentHeight->getEstimatedValue().getMetresValue() +
				fPrismHeight.getMetresValue()) / dxyz, 2);
			TReal b = powq(dxyz / dxy, 2);

			TReal distSigma = fSpaDistObs->getSigma()->getMetresValue() +
					fSpaDistObs->getObservedValue()->getKMetresValue() * fSpaDistObs->getPPM()->getMetresValue();
			TReal icSigma = tStation->getInstrumentCenteringSigma()->getMetresValue() * tStation->getInstrumentCenteringSigma()->getMetresValue();
			TReal ihSigma = tStation->getInstrumentHeightSigma()->getMetresValue() * tStation->getInstrumentHeightSigma()->getMetresValue();
			TReal tcSigma = fSpaDistObs->getTargetCenteringSigma()->getMetresValue() * fSpaDistObs->getTargetCenteringSigma()->getMetresValue();
			TReal thSigma = fSpaDistObs->getTargetHeightSigma()->getMetresValue() * fSpaDistObs->getTargetHeightSigma()->getMetresValue();
			TReal dcSigma = fSpaDistObs->getTarget()->getDistanceCorrectionSigma()->getMetresValue();

			TLength sigma(sqrtq(distSigma * distSigma + a * (ihSigma + thSigma) +
				b * (icSigma + tcSigma) + dcSigma * dcSigma));

			this->setSigmaAPriori(sigma);
		}


	/*!@name Access methods : iterator */
	//@{
		/*! Returns a iterator to the corresponding facade spatial dist observation */
		const TSpatialDistMeasurement<T>*	getSpaDistObs() const { return fSpaDistObs; }

		/*! Returns a pointer to the instrument's height */
		LSLengthConstIter	getHInstrument() const {return fInstrumentHeight;}
		
		/*!Returns an iterator of the distance cste */
		LSLengthConstIter	getDistConst() const {return fDistConst;}

		/*! Returns a pointer to the stationned point */
		LSPosVecIter		getStPoint() const{	return fStPoint;}
		
		/*! Returns a pointer to the target point */
		LSPosVecIter		getTgPoint() const {return fTgPoint;}

	//@}



	/*!@name Access methods : facade*/
	//@{
		/*! Returns the estimated value of the dist constant as a TLength */
		virtual TLength					getDistConstValue() const {return getDistConst()->getEstimatedValue();}
		
		/*! Returns the prism's height */
		TLength							getHPrism() const {return fPrismHeight;}
	
		/*! Returns the estimated instrument's height */
		TLength							getEstHInst() const { return hasOwnInstrumentHeight() ? getAllPointsFixedInstrumentHeight() : fInstrumentHeight->getEstimatedValue(); }
		
		/*!Return the corrected observation (obsvervation + constant)*/
		TLength							getCorrectedObsDist() const {return getObsDist() + getProvDistConst();}

		/*! Returns the provisional value of the distance constant as a TAngle */
		TLength							getProvDistConst() const {return fDistConst->getProvisionalValue();}

		/*! Returns the estimated value of the distance constant as a TAngle */
		TLength							getEstDistConst() const {return fDistConst->getEstimatedValue();}

		TLength							getEstValueForVariableDistConst() const
		{
			
			if(getDistConstStatus() == TALSCalcParameter::kVariable)
			{
				return getEstDistConst();
			}
			else
			{
				TLength zero (0);
				return (zero);
			}
		}

		TLength							getEstSigmaValueForVariableDistConst() const
		{
			if(getDistConstStatus() == TALSCalcParameter::kVariable)
			{
				return (getDistConst()->getEstimatedPrecision());
			}
			else
			{
				TLength zero (0);
				return (zero);
			}
		}

		/*! Returns the compensated angle after calculation  add to constante*/
		TLength							getCorrectedEstDist() const {return getEstDist()+ getEstDistConst();}

		/*!Return the stationned point Name*/
		string							getStPointName() const {return fStPoint->getName();}

		/*!Return the target point Name*/
		string							getTgPointName() const {return fTgPoint->getName();}

		TDouble							getSensitivity() const
		{
			TLength deltaZ =	fTgPoint->getEstimatedValue().getZ()
								+ getHPrism()
								- fStPoint->getEstimatedValue().getZ()
								- getEstHInst();
			TLength dist = getEstimatedDist() + getEstDistConst();
			TDouble sensitivity ((deltaZ.getMMetresValue()/dist.getMMetresValue())*10);

			return sensitivity;
		}

		/*! Returns the geode identifiant*/
		int								getId() const {return fSpaDistObs->getId();}

		/*! Returns the geode comment */
		string							getComment() const {return fSpaDistObs->getComment();}

		TPositionVector	getStEstCoordinate() const {return fStPoint->getEstimatedValue();}

		TPositionVector	getTgEstCoordinate() const {return fTgPoint->getEstimatedValue();}

		MatrixIndex		getStXIndex() const {return fStPoint->getXIndex();}

		MatrixIndex		getStYIndex() const {return fStPoint->getYIndex();}

		MatrixIndex		getStZIndex() const {return fStPoint->getZIndex();}

		MatrixIndex		getTgXIndex() const {return fTgPoint->getXIndex();}

		MatrixIndex		getTgYIndex() const {return fTgPoint->getYIndex();}

		MatrixIndex		getTgZIndex() const {return fTgPoint->getZIndex();}

		MatrixIndex		getHInstIndex() const {return getHInstrument()->getIndex();}

		MatrixIndex		getDistConstIndex() const {return getDistConst()->getIndex();}


		TALSCalcParameter::ELSStatus  getStXStatus() const { return fStPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getStYStatus() const { return fStPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getStZStatus() const { return fStPoint->getZStatus();}

		TALSCalcParameter::ELSStatus  getTgXStatus() const { return fTgPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getTgYStatus() const { return fTgPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getTgZStatus() const { return fTgPoint->getZStatus();}

		TALSCalcParameter::ELSStatus  getHInstStatus() const { return getHInstrument()->getStatus();}

		TALSCalcParameter::ELSStatus  getDistConstStatus() const { return getDistConst()->getStatus();}

		int	getObservationID() const { return fSpaDistObs->getObservationID(); }

		
		
	//@}
		void setHasOwnInstrumentHeight() { fHasOwnInstrumentHeight = true; }
		bool hasOwnInstrumentHeight() const { return fHasOwnInstrumentHeight; }
		TLength getAllPointsFixedInstrumentHeight() const { return fAllPointsFixedInstrumentHeight; }
		void setAllPointsFixedInstrumentHeight(TLength newIH) { fAllPointsFixedInstrumentHeight = newIH; }

private:

	/*!@name Private copy methods */
	//@{
	//@}

	/*!@name Observation attribute for calculation */
	//@{
	const TSpatialDistMeasurement<T>* fSpaDistObs; /*!< iterator of the corresponding spatial distance observation */
	//@}

	/*!@name Variables attributes for calculation */
	//@{
		bool				fHasOwnInstrumentHeight;
		TLength				fAllPointsFixedInstrumentHeight; // instrument height for the observation when all the points are fixed
	LSLengthIter		fInstrumentHeight; /*!< iterator for the instrument's height */
	TLength				fPrismHeight; /*!<prism's height */
	LSLengthIter		fDistConst; /*! Iterator for the distance constant */
	LSPosVecIter		fTgPoint; /*!< iterator for the target point */
	LSPosVecIter		fStPoint; /*!< iterator for the stationned point */

	const Station* tStation;
	//@}

};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc spatial dist observations  */
typedef list< TLSCalcSpatialDistObservation<TheodoliteTarget> >	LSSpaDistContainer;
/*! Iterator of the list of LSCalc spatial dist observations */
typedef LSSpaDistContainer::iterator						LSSpaDistIter;
/*! Constant iterator of the list of LSCalc spatial dist observations */
typedef LSSpaDistContainer::const_iterator					LSSpaDistConstIter;
//@}

typedef list< TLSCalcSpatialDistObservation<EDMTarget> >	LSEDMSpaDistContainer;
/*! Iterator of the list of LSCalc spatial dist observations */
typedef LSEDMSpaDistContainer::iterator						LSEDMSpaDistIter;
/*! Constant iterator of the list of LSCalc spatial dist observations */
typedef LSEDMSpaDistContainer::const_iterator				LSEDMSpaDistConstIter;

#endif //LSCALC_SPADIST_OBS



