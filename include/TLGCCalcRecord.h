// TLGCCalcRecord.h
//
/*! Class containing a record of data from a simulation calculation.
	Sigma0 parameters, calculated point data, and observation type summaries.
*/
// Patterns:
//
// 
// Copyright 2007-2008 CERN TS/SU. All rights reserved. 
// Author M. Jones
//////////////////////////////////////////////////////////////////////



#ifndef SU_LGC_CALC_RECORD
#define SU_LGC_CALC_RECORD


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include "TScalar.h"
#include "TLGCObsSummary.h"
#include "TLSCalcPosVectorParam.h"
#include "TALGCConverter.h"
//
// typedefs
//
//
////////////////////////////////////////////////////////////////

//Class definition
class  TLGCCalcRecord 

{
public:


	/*!@name Constructors and Destructors */
	//@{
		//! default constructor
		TLGCCalcRecord();

		//! copy constructor
		TLGCCalcRecord(const TLGCCalcRecord&);

		//! destructor
		virtual  ~TLGCCalcRecord();

	//@}


	/*!@name public member functions */
	//@{
		//! copy assignment operator
		TLGCCalcRecord& operator=(const TLGCCalcRecord& );

		//! equivalence operator
		bool	operator==(const TLGCCalcRecord& ) const;

	//@}
			
	/*!@name Data access methods for the sigma zero parameters */
	//@{
		/*! Returns the standard deviation a posteriori of ls calc observations */
		TScalar		getS0APosteriori() const { return fS0APosteriori; }
		/*! returns the upper chi-test limit for s0 a posteriori */
		TScalar		getChiHiLimit() const { return fChiHiLimit; }
		/*! returns the lower chi-test limit for s0 a posteriori */
		TScalar		getChiLoLimit() const { return fChiLoLimit; }

		/*! Sets the standard deviation a posteriori of ls calc observations */
		void		setS0APosteriori(TScalar s0Post) { fS0APosteriori = s0Post; }
		/*! sets the upper chi-test limit for s0 a posteriori */
		void		setChiHiLimit(TScalar high) { fChiHiLimit = high; }
		/*! sets the lower chi-test limit for s0 a posteriori */
		void		setChiLoLimit(TScalar low) { fChiLoLimit = low; }
	//@}
			

	/*!@ Specific methods for simulation calc position vector (point) data */
	//@{
		/*! Returns an iterator to the first item in the list */
		LSPosVecIter		beginPoint() {return fSimPoints.begin();}

		/*! Returns a const iterator to the first item in the list */
		LSPosVecConstIter	beginPoint() const {return fSimPoints.begin();}

		/*! Returns an iterator to the position after the last item of the list */
		LSPosVecIter		endPoint() {return fSimPoints.end();}

		/*! Returns an iterator to the first item in the list */
		LSPosVecConstIter	endPoint() const {return fSimPoints.end();}

		/*! Adds a new item to the list 
		\return an iterator to the new item */
		void				push_backPoint(TLSCalcPosVectorParam& point) {fSimPoints.push_back(point); return;}

		/*!\return an iterator pointing to a simulated point summary specified through the point name */
		LSPosVecIter		getPoint(const string name);

		/*!\return a const iterator pointing to a simulated point summary specified through the point name */
		LSPosVecConstIter	getPoint(const string name) const;

// **** from LSCalcDataSet: is it needed?
		/*!\class the list like the input point'list
		\return error */
		//string				orderPVList(const TWorkingPoints* ptList);
	//@}


	/*!@ Specific methods for summary iteration summary observation data */
	//@{
		/*! Returns an iterator to the first item in the container */
		ObsSummaryIter		beginObsSumm() {return fObsSummaries.begin();}

		/*! Returns a const iterator to the first item in the container */
		ObsSummaryConstIter	beginObsSumm() const {return fObsSummaries.begin();}

		/*! Returns an iterator to the end item of the container */
		ObsSummaryIter		endObsSumm() {return fObsSummaries.end();}

		/*! Returns a const iterator to the end item of the container */
		ObsSummaryConstIter	endObsSumm() const {return fObsSummaries.end();}

		/*! Adds a new item to the container */
		bool				addObsSumm(const TLGCObsSummary&, TALGCConverter::ELGCObservations);

	//@}


private:
	// Sigma Zero parameters
	/*! get the sigma zero a posteriori value */
	TScalar					fS0APosteriori; /*!< standard deviation for observations post calculation */
	/*! get the upper test limit for the sigma zero a posteriori value */
	TScalar					fChiHiLimit; /*!< upper chi-test limit for S0 a posteriori */
	/*! get the lower test limit for the sigma zero a posteriori value */
	TScalar					fChiLoLimit; /*!< lower chi-test limit for S0 a posteriori */

	// Point Data from an LGC simulation calculation
	LSPosVecContainer	    fSimPoints; /*!< container of simulation calc point data */

	// Container of Observation Summary Data from an LGC simulation calculation
	ObsSummaryContainer		fObsSummaries; /*!< container of LGC calc Observation Summary Data */

	//ClassDef(TLGCCalcRecord, 1)
};


/*!@name Typedefs */
//@{
	/*! Container of LGC calc records */
	typedef vector< TLGCCalcRecord >			CalcRecordContainer;
	/*! Iterator of the container of LGC calc records */
	typedef CalcRecordContainer::iterator		CalcRecordIter;
	/*! Constant iterator of the container of LGC calc records */
	typedef CalcRecordContainer::const_iterator	CalcRecordConstIter;
//@}


#endif // SU_LGC_CALC_RECORD


