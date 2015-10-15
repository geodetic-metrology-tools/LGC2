// THorAngleObservation.h
/*!
	Class for an horizontal angle observation

	Pattern: facade (for the components of an horizontal angle measurement :
				station, ROM, measurement, spatial points)

	Copyright 2002 CERN EST/SU. All rights reserved.
*/

////////////////////////////////////////////////////////////////

#ifndef SU_HOR_ANG_OBS
#define SU_HOR_ANG_OBS


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
using namespace std;


class THorAngleROM;
//class TLSCalcObservationMaker;

#include "TTheodoliteStation.h"
#include "TAMeasurement.h"
#include "TSpatialPoint.h"
#include "TSpatialPosition.h"

#include "TRotationMatrix.h"
#include "TAngle.h"
#include "THorAngleMeasurement.h"






////////////////////////////////////////////////////////////////

/*! \addtogroup lgc++
	@{*/

//! Class for a horizontal angle measurement 
class  THorAngleObservation
{
public:

	//friend TLSCalcObservationMaker;

	/*!@name Constructors and Destructors */
	//@{
	//! Default Constructor 
	THorAngleObservation();
	/*!Constructor
	\param station a pointer to the horizontal angle measurement's station
	\param ROM a pointer to the horizontal angle measurement's ROM
	\param meas a pointer to the horizontal angle measurement
	\param stationedPt an iterator of the stationed points list 
	\param targetedPt an iterator of the targeted points list 
	\param cte angle measurement constant*/
	THorAngleObservation(	TheodStConstIter station, HorAngROMConstIter ROM, 
							HorAngMeasConstIter meas, PointConstIter stationedPt,
							PointConstIter targetPt, AngleConstantConstIter cte, int id);
	//! Copy Constructor 
	THorAngleObservation(const THorAngleObservation &source);
	//! Destructor
	virtual  ~THorAngleObservation();
	//@}


	/*! Overloaded equality operator */
	virtual bool  operator==(const THorAngleObservation& right) const;

	/*! returns true if the observation is active */
	bool	isActive() const;

	/*@name Access to observed values */
	//@{
	/*! returns the horizontal angle value  from the measurement added to the constant angle (if there is one) */
	TAngle		getHorAngleValue() const;
	/*! returns the horizontal angle sigma value from the measurement */
	TAngle		getSigmaValue() const;
	//@}

	/*!@name Access to data on parameter V0 */
	//@{
	/*! returns the V0 from the rom */
	struct Angles	getRomV0() const;
	/*! returns V0's status from the rom */
	TAMeasurement::ECalcStatus	getRomV0Status() const;
	/*! returns V0's identifier */
	string		getRomV0Name() const;
	//@}

	/*!@name Access to data on parameter stationed point */
	//@
	/*! Returns the position vector of the stationed point */
	TPositionVector		getStationedPosition() const;
	/*! Returns the spatial status of the stationed point */
	TSpatialStatus::ESpatialStatus		getStationedPosStatus() const;
	/*! Returns the concatenated name of the stationed point */
	string		getStationedPosName() const;
	//@}

	/*!@name Access to data on parameter target point */
	//@{
	/*! Returns the position vector of the target point */
	TPositionVector		getTargetPosition() const;
	/*! Returns the spatial status of the target point */
	TSpatialStatus::ESpatialStatus		getTargetPosStatus() const;
	/*! Returns the concatenated name of the target point */
	string		getTargetPosName() const;
	//@}

	/*!@name Access to data on parameter constant distance */
	//@{
	/*! Returns the constant angle */
	TAngle		getConstantValue() const;
	/*! Returns the constant angle'status */
	TAMeasurement::ECalcStatus		getConstantStatus() const;
	/*! Returns the constant angle's identifier */
	string		getConstantName() const;
	//@}

	/*!@name Access to database' data */
	//@{
		/*! Returns the geode identifiant*/
		int		getId() const {return fMeasurement->getId();}
		/*! Returns the geode comment */
		string	getComment() const {return fMeasurement->getComment();}
	//@}

	/*!@name Methods converting points in 2D+H coordinate system in 3D cartesian system */ 
	//@{
		/*! Converts the stationed point from 2D+H to 3D cartesian coordinate system, if necessary */
		TPositionVector					get3DStationPosVec() const;
		/*! Converts the stationed point from 2D+H to 3D cartesian coordinate system, if necessary */
		TPositionVector					get3DTargetPosVec() const;

		int								getObservationID() const { return observationID; }
	//@}
	
protected:

	/*!@name Access methods */
	//@{
	/*! returns a pointer to the thedolite station of this measurement*/
	TheodStConstIter					getStation() const;
	/*! returns a pointer to the horizontal angle round of measurements of this measurement*/
	HorAngROMConstIter				getRoundOfMeas() const;
	/*! returns a pointer to the horizontal angle measurement*/
	HorAngMeasConstIter				getMeasurement() const;
	/*! returns an iterator to the stationed point of this measurement*/
	PointConstIter					getStationedPoint() const;
	/*! returns an iterator to the targeted point of this measurement*/
	PointConstIter					getTargetPoint() const;

	//@}

private:
	//! Copy Assignment Operator
	THorAngleObservation& operator=(const THorAngleObservation &right);
	

	int									observationID;
	TheodStConstIter					fStation; /*!< theodolite station of this measurement */
	HorAngROMConstIter					fROM; /*!< horizontal angle ROM of this measurement */
	HorAngMeasConstIter					fMeasurement; /*!< horizontal angle measurement */
	PointConstIter						fStationedPoint; /*!< stationed point of this horizontal angle measurement */
	PointConstIter						fTargetPoint; /*!< targeted point of this horizontal angle measurement */
	AngleConstantConstIter				fMeasConst; /*!< measurement constant of this observation */
	bool								fIsActive; /*< activation level of the observation based on the activation of the other attributes */

};

/*!@name Constants definition */
//@{
/*! List of horizontal angle observations  */
typedef list< THorAngleObservation >	HorAngObsContainer;
/*! Iterator of the list of horizontal angle observations */
typedef HorAngObsContainer::iterator		HorAngObsIter;
/*! Constant iterator of the list of horizontal angle observations */
typedef HorAngObsContainer::const_iterator	HorAngObsConstIter;
//@}

#endif  //SU_HOR_ANG_OBS

/*@}*/

inline bool THorAngleObservation::isActive() const { return fIsActive; }

inline TheodStConstIter		THorAngleObservation::getStation() const { return fStation; }
inline HorAngROMConstIter	THorAngleObservation::getRoundOfMeas() const { return fROM; }
inline HorAngMeasConstIter	THorAngleObservation::getMeasurement() const { return fMeasurement; }
inline PointConstIter THorAngleObservation::getStationedPoint() const { return fStationedPoint; }
inline PointConstIter THorAngleObservation::getTargetPoint() const { return fTargetPoint; }
