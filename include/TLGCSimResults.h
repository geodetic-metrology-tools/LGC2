// TLGCSimResults.h
//
/*! List of TLGCSimWorkingPoints.
	Class used for storage of the parts of the simulations' results file.
*/
// Patterns:
//
// 
// Copyright 2000-2008 CERN TS/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_LGC_SIM_RES
#define SU_LGC_SIM_RES


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//

#include  "TLGCSimPointSummary.h"
#include  "TLGCCalcRecord.h"
#include  "TLSCalcWorkingPosVec.h"
//
// typedefs
//
//
////////////////////////////////////////////////////////////////

//Class definition
class  TLGCSimResults 

{
public:


	/*!@name Constructors and Destructors */
	//@{
		//! default constructor
		TLGCSimResults();

		/*! constructor taking an iterator LSCalcPtIterator*/
		explicit TLGCSimResults(TLSCalcWorkingPosVec* workingPts);

		//! copy constructor
		TLGCSimResults(const TLGCSimResults&);

		//! destructor
		virtual  ~TLGCSimResults();

	//@}


	/*!@name member functions */
	//@{
		//! copy assignment operator
		TLGCSimResults& operator=(const TLGCSimResults& );

		/*! update the values of residus in LGCSimPoints */
		bool updateResValues(TLSCalcWorkingPosVec* workingPts);
	//@}

	/*!@ Specific methods for point data summaries */
	//@{
		/*! Returns an iterator to the first item in the list */
		SimPointIterator	beginPoint() {return fSimPoints.begin();}

		/*! Returns a const iterator to the first item in the list */
		SimPointConstIter	beginPoint() const {return fSimPoints.begin();}

		/*! Returns an iterator to the position after the last item of the list */
		SimPointIterator	endPoint() {return fSimPoints.end();}

		/*! Returns an iterator to the first item in the list */
		SimPointConstIter	endPoint() const {return fSimPoints.end();}

		/*! Adds a new item to the list 
		\return an iterator to the new item */
		void				push_backPoint(const TLGCSimPointSummary& point);

		/*!\return an iterator pointing to a simulated point summary specified through the point name */
		SimPointIterator	getPoint(const string name);

		/*!\return a const iterator pointing to a simulated point summary specified through the point name */
		SimPointConstIter	getPoint(const string name) const;

		/*!\return an iterator pointing to a simulated point summary specified through an LS position vector parameter iterator */
		SimPointIterator	getPoint(const LSPosVecIter posVec);
		
		/*! Erases the selected TLGCSimPoint and deletes its corresponding pointer from the list */
		bool	erase(TLGCSimPointSummary& simpt);
		
		/*! Returns the size of the list*/
		int		size() const {return fSimPoints.size();}

	//@}

	/*!@ Specific methods for sim calculation records */
	//@{
		/*! Returns an iterator to the first item in the list */
		CalcRecordIter		beginSimCalc() {return fSimCalcRecords.begin();}

		/*! Returns a const iterator to the first item in the list */
		CalcRecordConstIter	beginSimCalc() const {return fSimCalcRecords.begin();}

		/*! Returns an iterator to the position after the last item of the list */
		CalcRecordIter		endSimCalc() {return fSimCalcRecords.end();}

		/*! Returns an iterator to the first item in the list */
		CalcRecordConstIter	endSimCalc() const {return fSimCalcRecords.end();}

		/*! Adds a new sim calc record to the list 
		\return an iterator to the new item */
		CalcRecordIter		addSimCalc();
	//@}


private:
	/*!@name Private Methods */
	//@{
		/*! initialise TLGCSimResults with a set of LSPosVecParam*/
		void				init(TLSCalcWorkingPosVec* workingPts);
	//@}

	SimPointContainer		fSimPoints; /*!< list of simulations' working points*/

	CalcRecordContainer		fSimCalcRecords; /*!< vector of simulation calc records*/

	//ClassDef(TLGCSimResults, 1)
};


#endif // SU_LGC_SIM_RES
