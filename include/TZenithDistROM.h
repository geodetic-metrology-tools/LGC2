
#ifndef SU_ZENITH_DIST_ROM
#define SU_ZENITH_DIST_ROM


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000


// Forward declarations
//
#include  <list>
#include  <string>
#include  <iostream>
using namespace std;

class  TTheodoliteStation;

#include  "TAUnreferencedROM.h"
#include  "TZenithDistMeasurement.h"

/*!\ingroup spatialmeasurements
	@{*/

//! Class for a round of zenithal distance measurements
class  TZenithDistROM : public TAUnreferencedROM  
{
public:

	/*!@name Constructors and Destructors */
	//@{
	/*! Default constructor */
	TZenithDistROM();
	/*! Copy Constructor */
	TZenithDistROM(const  TZenithDistROM& source);
	/*! Destructor */
	virtual  ~TZenithDistROM();
	//@}

	//! Copy Assignment Operator 
	TZenithDistROM& operator=( const TZenithDistROM& source);

	/*! Adds a zenithal distance to this ROM
	@param ha a pointer to the zenithal distance to be inserted. NB : a copy of the meas. pointed to will be made*/
	virtual TZenithDistMeasurement*  addZenithDist(TZenithDistMeasurement*ha);

	/*!@name Zenithal distance measurement related*/
	//@{
	virtual ZenithDistMeasIterator	getZenithDistMeas(TSpatialPointName iterTg);
	virtual ZenithDistMeasConstIter getZenithDistMeas(TSpatialPointName iterTg) const;
	/*!@returns a boolean indicating if the pointer to angle measurement is already stored by this ROM*/
	virtual bool  notInContainer(TZenithDistMeasurement*) const;
	/*!@returns an iterator to the first zenithal distance measurement of this ROM's container*/
	virtual ZenithDistMeasIterator  getZenithDistMeasBeginIterator();
	/*!@returns a const iterator to the first zenithal distance measurement of this ROM's container*/
	virtual ZenithDistMeasConstIter  getZenithDistMeasBeginIterator() const;
	/*!@returns an iterator to one past the last zenithal distance measurement of this rom's container*/
	virtual ZenithDistMeasIterator  getZenithDistMeasEndIterator();
	/*!@returns a const iterator to one past the last zenithal distance measurement of this rom's container*/
	virtual ZenithDistMeasConstIter  getZenithDistMeasEndIterator() const;
	
//	virtual void							sendEnoughTotalEqns();

	/*!@returns a non-const pointer to this object*/
	virtual TZenithDistROM*					getPointer() const;

protected:


private:
	
	ZenithDistContainer				fMeasuredAngles; /*!< list of TZenithDistMeasurement */
	
};


/*@}*/



	/*!@name Typedefs*/
	//@{
	/*! Defines the container storing the zenithal distance ROMs as a type */
	typedef list<TZenithDistROM > ZenithDistROMContainer;
	/*! Defines an iterator to a TZenithDistROM as a type */
	typedef ZenithDistROMContainer::iterator ZenithDistROMIterator;
	/*! Defines a const iterator to a TZenithDistROM as a type */
	typedef ZenithDistROMContainer::const_iterator ZenithDistROMConstIter;
	//@}


#endif // SU_##TEMPLATE##











////////////////////////////////////////////////////////////////////
//end
////////////////////////////////////////////////////////////////////


//#include  "UEOIndices.h"


	/*@Constructor
	\param parent a reference to the TTheodoliteStation from which this ROM has been made*/
//	explicit TZenithDistROM(TTheodoliteStation& parent);


	/*! Gives access to a zenith dist measurement specified by its target point
	@param iterTg an iterator pointing to the target TSpatialPoint of wanted measurement
	@returns an iterator pointing to the zenithal distance measurement made to the specified point*/
//	virtual ZenithDistMeasIterator  getZenithDistMeas(TWorkingPoints::PointIterator iterTg);

	/*! Gives access to a zenith dist measurement specified by its target point
	@param iterTg an iterator pointing to the target TSpatialPoint of wanted measurement
	@returns a const iterator pointing to the zenithal distance measurement made to the specified point*/
//	virtual ZenithDistMeasConstIter  getZenithDistMeas(TWorkingPoints::PointIterator iterTg) const;

	/*!@name Activation/Disactivation related*/
	//@{
	//!Tells the children measurements that this ROM has been disactivated
//	virtual void							sendDisactivatedToChildren();
	//!Tells the children measurements that this ROM has been activated
//	virtual void							sendActivatedToChildren();

	//!Tells the parent theodolite station that this ROM has been disactivated
//	virtual void							sendDisactivatedToParent();
	//!Tells the parent theodolite station that this ROM has been activated
//	virtual void							sendActivatedToParent();
	//@}

//	virtual void							parentEnoughTotalEqns();
//	virtual void							activateReadyChildren();

	
//	TTheodoliteStation*				fParentStation;
//	TWorkingPoints::PointIterator	fReferencePoint;





