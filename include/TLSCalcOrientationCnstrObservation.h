//TLSCalcOrientationCnstrObservation.h
/*! 
	Class for a offset (PDOR) observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				distance observations, position and length parameters)

	Copyright 2003 CERN EST/SU. All rights reserved.
*/


#ifndef LSCALC_OFFSET_PDOR_OBS
#define LSCALC_OFFSET_PDOR_OBS


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)
#endif // _MSC_VER >= 1000

#ifdef _DEBUG
#define TLSCalcOrientationCnstrObservation TLSCPDOROBS
#endif

#include <list>
#include <string>
using namespace std;

#include "TALSCalcObservation.h"
#include "TOrientationCnstrObservation.h"
#include "lsalgo/TLSCalcPosVectorParam.h" 

/*!\ingroup lgc++
	@{*/

//! Class for a offset to vertical Plane observation to be used in the least square algorithms
class TLSCalcOrientationCnstrObservation : public TALSCalcObservation  
{
public:
	/*!@name Constructors / Destructors */
	//@{
		/*! Constructor*/
		TLSCalcOrientationCnstrObservation(OrieCnstrIterator obs, LSPosVecIter fixedPoint, LSPosVecIter refPoint);
		/*! Copy constructor */
		TLSCalcOrientationCnstrObservation(const TLSCalcOrientationCnstrObservation& source);
		/*! Destructor */
		virtual ~TLSCalcOrientationCnstrObservation();
	//@}

	/*! Overloaded equality operator */
	bool	operator==(const TLSCalcOrientationCnstrObservation& right);

	/*!@name Access methods */
	//@{
		/*! Returns a iterator to the corresponding facade spatial dist observation */
		OrieCnstrIterator		getOrieCnstrObs() const { return fOrieCnstrObs; }
		/*! Returns a pointer to the point */
		LSPosVecIter			getFixedPoint() const; 
		/*! Returns a pointer to the point */
		LSPosVecIter			getRefPoint() const; 
		/*! Returns  the angle used to defined the bearing */
		TAngle					getBearing() const; 
		/*! Returns sigma */
		TAngle					getSigmaAPriori() const; 

	//@}

	/*!@name Access methods : facade */
	//@{
		/*! Returns the geode identifiant*/
		int		getId() const {return fOrieCnstrObs->getId();}
		/*! Returns the geode comment */
		string	getComment() const {return fOrieCnstrObs->getComment();}

		TPositionVector	getFixedEstCoordinate() const {return fFixedPoint->getEstimatedValue();}

		TPositionVector	getRefEstCoordinate() const {return fRefPoint->getEstimatedValue();}

		MatrixIndex		getFixedXIndex() const {return fFixedPoint->getXIndex();}

		MatrixIndex		getFixedYIndex() const {return fFixedPoint->getYIndex();}

		MatrixIndex		getFixedZIndex() const {return fFixedPoint->getZIndex();}

		MatrixIndex		getRefXIndex() const {return fRefPoint->getXIndex();}

		MatrixIndex		getRefYIndex() const {return fRefPoint->getYIndex();}

		MatrixIndex		getRefZIndex() const {return fRefPoint->getZIndex();}

		TALSCalcParameter::ELSStatus  getFixedXStatus() const { return fFixedPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getFixedYStatus() const { return fFixedPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getFixedZStatus() const { return fFixedPoint->getZStatus();}

		TALSCalcParameter::ELSStatus  getRefXStatus() const { return fRefPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getRefYStatus() const { return fRefPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getRefZStatus() const { return fRefPoint->getZStatus();}

		int	getObservationID() const { return fOrieCnstrObs->getObservationID(); }



	//@}


private:

	/*!@name Private copy methods */
	//@{
		/*! Copy assignment operator */
		TLSCalcOrientationCnstrObservation& operator=(const TLSCalcOrientationCnstrObservation& right);
	//@}

	/*!@name Observation attributes for calculation */
	//@{
		OrieCnstrIterator		fOrieCnstrObs; /*!< iterator of the corresponding offset observation */
	//@}

	/*!@name Variables attributes for calculation */
	//@{
		LSPosVecIter		fFixedPoint; /*!< iterator for the first target point used to define the target Plane*/
		LSPosVecIter		fRefPoint; /*!< iterator for the first target point used to define the target Plane*/
	//@}

};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc spatial dist observations  */
typedef list< TLSCalcOrientationCnstrObservation >		LSOrieCnstrContainer;
/*! Iterator of the list of LSCalc spatial dist observations */
typedef LSOrieCnstrContainer::iterator					LSOrieCnstrIter;
/*! Constant iterator of the list of LSCalc spatial dist observations */
typedef LSOrieCnstrContainer::const_iterator			LSOrieCnstrConstIter;
//@}

#endif //LSCALC_OFFSET_PDOR_OBS



