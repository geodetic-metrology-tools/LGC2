// TWireToWireDistMeas.h
//
/*! 
	Class representing a wire to wire distance measurement

	Patterns:

 
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TWireToWireDistMeas
#define SU_TWireToWireDistMeas


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <**classname**>
//using namespace std;
//
class  TWTWDistStation;
#include "TLength.h"
#include "TSpatialPointName.h"

#include  "TAObjectMeasurement.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class definition
class  TWireToWireDistMeas : public TAObjectMeasurement  
{
public:

	/*!@name Constructors and Destructors */
	//@{
		/*!Default constructor */
		TWireToWireDistMeas();
		/*! Constructor
		\param w1End1Name the first wire's first end point's name
		\param w1End2Name the first wire's second end point's name
		\param wire1Pos the measurement's position along the first wire (distance from the wire's first point)
		\param w2End1Name the second wire's first end point's name
		\param w2End2Name the second wire's second end point's name
		\param wire2Pos the measurement's position along the second wire (distance from the wire's first point)
		\param wToWDist the measured distance
		\param sigma the measured distance's sigma a priori	*/
		TWireToWireDistMeas(TLength wire1Pos, TLength wire2Pos, string wire1, string wire2,
			TLength hDist, TLength hSigma, TLength vDist, TLength vSigma /*, TSpatialPointName w1End1Name, TSpatialPointName w1End2Name, TSpatialPointName w2End1Name, TSpatialPointName w2End2Name*/ );
		/*! Copy Constructor */
		TWireToWireDistMeas(const  TWireToWireDistMeas&);
		/*! Destructor */
		virtual  ~TWireToWireDistMeas();
	//@}

	/*!@name Overloaded operators */
	//@{
	/*! Copy Assignment Operator */ 
	TWireToWireDistMeas& operator=( const TWireToWireDistMeas& );
	/*! Equality operator */
	virtual bool								operator==(const TWireToWireDistMeas&) const;
	//@}

	/*! return a string describing the measurement's kind*/
	virtual string								getMeasKind() const;

	/*!@name Access methods*/
	//@{
	/*! returns the position of the measurement along the first wire */
	virtual TLength								getWire1Pos() const;
	/*! returns the position of the measurement along the second wire */
	virtual TLength								getWire2Pos() const;
	/*! returns the measured horizontal distance as a TLength object */
	virtual TLength								getHDist() const;
	/*! returns the horizontal measurement's sigma a priori as a TLength object */
	virtual TLength								getHSigma() const;
	/*! returns the measured vertical distance as a TLength object */
	virtual TLength								getVDist() const;
	/*! returns the vertical measurement's sigma a priori as a TLength object */
	virtual TLength								getVSigma() const;
	// Temporary...Used to define the wires
	string	getWire1Name() const;
	string	getWire2Name() const;
	//@}


	/*!@name Set methods*/
	//@{
	/*! Sets the horizontal sigma a priori if it isn't done yet
	\param sigma the value to assign to the h sigma if the latter is still null*/
	virtual void								ifNotDoneSetHSigma(TLength sigma);
	/*! Sets the vertical sigma a priori if it isn't done yet
	\param sigma the value to assign to the v sigma if the latter is still null*/
	virtual void								ifNotDoneSetVSigma(TLength sigma);
	// Temporary...Used to define the wires
	/*! Sets the first wire's name */
	void setWire1Name(string w1);
	/*! Sets the second wire's name */
	void setWire2Name(string w2);
	//@}

private:

	string									fWire1Name;
	string									fWire2Name;
	TLength									fPositionOnWire1; /*!< position of the measurement along the first wire */ 
	TLength									fPositionOnWire2; /*!< position of the measurement along the second wire */
	TLength									fHDist; /*!< horizontal distance measurement */
	TLength									fHSigma; /*!< horizontal distance error */
//	bool									fHUsedInCalc; /*!< indicator for horizontal distance's activation in calculation */
	TLength									fVDist; /*!<vertical distance measurement */
	TLength									fVSigma; /*!<vertical distance error */
//	bool									fVUsedInCalc; /*!< indicator for vertical distance's activation in calculation */

	

	//ClassDef(TWireToWireDistMeas, 1)
};

/*@}*/
 
//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////

inline TLength	TWireToWireDistMeas::getHDist() const {return fHDist;}

inline TLength	TWireToWireDistMeas::getVDist() const {return fVDist;}

inline TLength	TWireToWireDistMeas::getHSigma() const {return fHSigma;}

inline TLength	TWireToWireDistMeas::getVSigma() const {return fVSigma;}

inline TLength	TWireToWireDistMeas::getWire1Pos() const {return fPositionOnWire1;}

inline TLength	TWireToWireDistMeas::getWire2Pos() const {return fPositionOnWire2;}



	/*!@name Type definitions */
	//@{
	typedef TWireToWireDistMeas* WTWDistMeasIterator;
	typedef const TWireToWireDistMeas* WTWDistMeasConstIter;
	//@}




#endif // SU_TWireToWireDistMeas


//class TWorkingStations;
//#include "TWorkingTargetObjects.h"
//#include "TVMeasurableEntityLstnr.h"

		/*! Constructor 
		@param parent a reference to the parent station
		@param wire1 an iterator pointing to the first wire
		@param wire1Pos the measurement's position along the first wire (distance from the wire's first point)
		@param wire2 an iterator pointing to the second wire
		@param wire2Pos the measurement's position along the second wire (distance from the wire's first point)
		@param wToWDist the measured distance
		@param sigma the measured distance's sigma a priori	*/
/*TWireToWireDistMeas(TWTWDistStation& parent, TWorkingTargetObjects::WireIterator wire1, TLength wire1Pos, 
			TWorkingTargetObjects::WireIterator wire2, TLength wire2Pos, TLength wToWHDist, TLength hSigma, TLength wToWVDist, TLength vSigma);
*/

/*	/// Less than operator
	virtual bool								operator<(const TWireToWireDistMeas&) const;*/

	/**@return an iterator pointing to the first wire*/
//	virtual TWorkingTargetObjects::WireIterator	getFirstWire() const;
	/**@return an iterator pointing to the second wire*/
//	virtual TWorkingTargetObjects::WireIterator	getSecondWire() const;

	/*! Sets the horizontal sigma a priori if it isn't done yet
	virtual void								ifNotDoneSetHSigma(TLength sigma);
	/**Sets the iterators to the two wires from the end point names
	@param wp a pointer to the TWorkingPoints object
	@param wto a pointer to the TWorkingTargetObjects object*/
//	virtual bool								setWires(TWorkingPoints* wp, TWorkingTargetObjects* wto);
	/**Creates the TWTWDistStation station
	@param wp a pointer to the TWorkingPoints object
	@param ws a pointer to the TWorkingStations object*/
//	virtual bool								addToStation(TWorkingStations* ws);

	///tells the parent station that this measurement is disactivated
//	virtual void								sendDisactivatedToParent();
	///tells the parent station that this measurement is activated
//	virtual void								sendActivatedToParent();
	//@}
//	bool									setFirstWire(TWorkingPoints* wp, TWorkingTargetObjects* wto);
//	bool									setSecondWire(TWorkingPoints* wp, TWorkingTargetObjects* wto);


/*inline TWorkingTargetObjects::WireIterator TWireToWireDistMeas::getFirstWire() const
{	return fFirstWire;}

  //inline bool	TWireToWireDistMeas::hActive() const {return fHUsedInCalc;}

//inline bool	TWireToWireDistMeas::vActive() const {return fVUsedInCalc;}



inline TWorkingTargetObjects::WireIterator TWireToWireDistMeas::getSecondWire() const
{	return fSecondWire;}*/

	/*!@name Active / inactive related*/
	//@{
	/*! disactivates the horizontal dist measurement */
//	virtual void								disactivateHDist();
	/*! activates the horizontal dist measurement */
//	virtual void								activateHDist();
	/*! return a true boolean if the horizontal dist measurement is active */
//	virtual bool								hActive() const;
	/*! disactivates the vertical dist measurement */
//	virtual void								disactivateVDist();
	/*! activates the vertical dist measurement */
//	virtual void								activateVDist();
	/*! return a true boolean if the vertical dist measurement is active */
//	virtual bool								vActive() const;
	//@}


//	TWorkingTargetObjects::WireIterator		fFirstWire;
//	TWorkingTargetObjects::WireIterator		fSecondWire;

//	TWTWDistStation*						fParentStation;
//	TSpatialPointName						fWire1End1Name; /*!< name of the wire first's ending point 1 */
//	TSpatialPointName						fWire1End2Name; /*!< name of the wire first's ending point 2 */
//	TSpatialPointName						fWire2End1Name; /*!< name of the wire second's ending point 1 */
//	TSpatialPointName						fWire2End2Name; /*!< name of the wire second's ending point 2 */
