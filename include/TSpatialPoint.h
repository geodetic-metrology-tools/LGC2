// TSpatialPoint.h
/*!	  
	TSpatialPoint gathered all the information about a spatial point useful for 
	survey purpose.

	Copyright 1999,2000, CERN, EST/SU. All rights reserved.
*/

#ifndef SU_SPATIAL_POINT
#define SU_SPATIAL_POINT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
//using namespace std;

//class TRoundOfMeasNetworkListnr;



class  TSpatialPosition;
class  TAReferenceFrame;

#include  "TVNumericValue.h"

//#include  "TWorkingPoints.h"
#include  "TSpatialPointName.h"
#include  "TVSpatialPtListener.h"
//#include  "TASpatialPtListener.h"


/*!\ingroup spatialobjects
	@{*/

//! Class for a spatial point. The current pointed-to spatial position will be deleted
class	TSpatialPoint //: public TObject
{

public:	
	
	enum  EPointStatus {kActive, kInactive,  kBad, kNew};
	/*!@name Constructors and Destructor*/
	//@{
		/*!Default constructor */
		TSpatialPoint();
		
		/*!Constructor
		\param ptName the point's name (will be assigned to the Name component of the TSpatialPointName)*/
		explicit  TSpatialPoint( const string& ptName );

		/*!Constructor
		\param ptName the point's name as a TSpatialPointName
		\param sp a pointer to the point's TSpatialPosition */
		TSpatialPoint( const TSpatialPointName& ptName, TSpatialPosition* sp);

		/*!Copy constructor */
		TSpatialPoint( const TSpatialPoint& );
		//TSpatialPoint( const string& name, const char point, const string& classifier );
		//TSpatialPoint( const string& name, const char point, const string& classifier, TSpatialPosition * );

		void swap(TSpatialPoint &) throw();
		
		/*!Destructor */
		virtual  ~TSpatialPoint();
	//@}


	/*!@name Overloaded operators*/
	//@{
		/*!Copy assignement operator */
		virtual  TSpatialPoint&  operator =( const TSpatialPoint& );

		/*! Equality operator */
		virtual  bool  operator ==( const TSpatialPoint & ) const;  

		/*! Less than operator */
		virtual  bool  operator <( const TSpatialPoint & ) const;  
	//@}

	/*!@name Public member functions */
	//@{
		/*!Sets the point's spatial position
		\param sp a pointer to the point's associated TSpatialPosition*/
		virtual  void 	 setPosition( TSpatialPosition* sp);

		void setPosition( TSpatialPosition* sp, int column, bool isNull);
		void isPointNew();
		
		//	virtual  void 	 doNothing();
		/*!Gets the point's position 
		\param rf a pointer to the reference frame in which the position has to be returned */
		virtual  TSpatialPosition  getPosition( TAReferenceFrame* rf) const;

		/*!Sets the point's name
		\param spn the point's name*/
		virtual  void  setName( const TSpatialPointName& spn);

		/*!Sets the point's name
		\param name the Name component of the TSpatialPointName
		\param element the Element component of the TSpatialPointName
		\param classifier the Classifier component of the TSpatialPointName*/
		//virtual	 void  setName( const string& name, const string& element, const string& classifier );
		/*!Returns the point's name */
		virtual  TSpatialPointName  getName()  const;
		
		/*!Sets the point's distance (for file from geode)
		\param id the point's distance as a TReal*/
		virtual  void  setDist( const TReal& id);

		/*!Returns the point's distance */
		virtual  TReal  getDist()  const {return fPointDist;};
	
	
		/*!Sets the point's header line of comment (for file from geode)
		\param com the point's comment as a string*/
		virtual  void  setHeaderComment( const string& com);

		/*!Returns the point's header comment */
		virtual  string  getHeaderComment()  const {return fHeaderComment;};

		/*!Sets the point's End Of Line comments (for file from geode)
		\param com the point's comment as a string*/
		virtual  void  setEOLComment( const string& com) {fEOLComment = com; return;};

		/*!Returns the point's header comment */
		virtual  string  getEOLComment()  const {return fEOLComment;};

		/*!Sets the point's name
		\param ptName the point's name (will be assigned to the Name component of the TSpatialPointName)*/
		virtual  void  setPtName( const string& ptName );

		/*!METHOD FOR CHABA INTERFACE ONLY - CALLED IF TWO POINTS HAVE IDENTICAL TIMESTAMPS*/
		virtual void setDuplicatePtName( const string& ptName ) ;


		/*! Returns the point's concatenated name as a string */
		virtual  string  getPtName() const;

		/*!Sets the point's status to active, inactive or bad */
		virtual void setStatus(const TSpatialPoint::EPointStatus status);
		
		/*!Returns the point's status*/
		virtual TSpatialPoint::EPointStatus getStatus() const {return fPointStatus;}; 


		/*!Sets a reference to the set of points which the individual belongs to*/
		virtual void setListener(TVSpatialPtListener* spl){flistener = spl; return;};
		/*!Returns a reference to the set of points which the individual point belongs to*/
		TVSpatialPtListener* getListener() {return flistener;};

		//virtual  string  getDBName()  const;
		/*!Tells if the point is active or not
		\return a true boolean if the point is active*/
		virtual bool	isActive() const;

		/*! Disactivates the point */
		virtual void	disactivate();

		/*! Activated the point */
		virtual void	activate();

		/*! Tells all the point listeners that the point has been disactivated */
		virtual void	sendDisactivatedToAll();

		/*! Tells all the point listeners that the point has been activated */
		virtual void	sendActivatedToAll();

		/*!Returns the point's equations count */
		virtual int		equationsCount() const;

		/*!Updates the point's equation count
		\param eqCountUpdate the update (+ or -) to the point's equation count*/
		virtual void	updateEqCount(int eqCountUpdate);

		/*! Adds a listener to the brodcast list */
		virtual void	addListener(TVSpatialPtListener *);

		/*! Returns the point's spatial position */
		virtual TSpatialPosition&	getPosition() const;

		/*! Returns the point's number of variable coordinates*/
		virtual int					getVariableDimension() const;

	
		virtual bool getXSet() const {return fXSet;};
		virtual bool getYSet() const {return fYSet;};
		virtual bool getZSet() const {return fZSet;};
		virtual bool getNameSet() const {return fNameSet;};

		virtual void setXSet(bool state);
		virtual void setYSet(bool state);
		virtual void setZSet(bool state);
		virtual void setNameSet(bool state);





	//@}

private:
	/*!@name Private member functions */
	//@{
		/*! updates the number of needed equations for the point's determination */
		virtual void	updateNeededEquations();
	//@}

protected:
	
	bool						fUsedInCalc; /*!< gives the state of the point whether it is used or not*/
	TSpatialPointName*			fName; /*!< pointer to the point's TSpatialPointName*/

	TReal						fPointDist; /*!< distance (from geode's DB point) for the point */
	
	string						fHeaderComment; /*!<line of comments before the line of coordinate*/
	string						fEOLComment;/*!<end of line comments*/

	TSpatialPosition*			fPosition; /*!< pointer to the point's TSpatialPosition */

	int							fEqCount; /*!< point's equations count */
	int							fNeededEquations; /*!< number of needed equations to deterine the point */

	PtListnrContainer			fPtListeners; /*!< list of the point's listener */

	TVSpatialPtListener*		flistener;	
	
	EPointStatus				fPointStatus;

	bool fNameSet;
	bool fXSet;
	bool fYSet;
	bool fZSet;
	//ClassDef(TSpatialPoint, 1)

};
/*@}*/

/*!@name Typedefs*/
//@{
	/*! Type of the container used to store points */
	typedef list<TSpatialPoint> SpatialPointContainer;
	/*! Type of an iterator pointing to an element of the container */
	typedef SpatialPointContainer::iterator PointIterator;
	/*! Type of a const iterator pointing to an element of the container */
	typedef SpatialPointContainer::const_iterator PointConstIter;
//@}

namespace std {
template <>
inline void swap(TSpatialPoint & first, TSpatialPoint & second)
{
	first.swap(second);
}
}

#endif
