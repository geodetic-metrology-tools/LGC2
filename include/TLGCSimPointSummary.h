// TLGCSimPointSummarySummary.h
//
/*! Class containing all information concerning a point of the DataSet
that are necessary to write the summary of the simulations' results file.
*/
// Patterns:
//
// 
// Copyright 2000-2008 M.Jones CERN TS/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_LGC_SIM_PT_SUMMARY
#define SU_LGC_SIM_PT_SUMMARY


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//

#include  "TLSCalcPosVectorParam.h"
#include  "TFreeVector.h"
//
// typedefs
//
//
////////////////////////////////////////////////////////////////

//Class definition
class  TLGCSimPointSummary 

{
public:


	/*!@name Constructors and Destructors */
	//@{
		//! default constructor
		TLGCSimPointSummary();

		/*! constructor taking an iterator LSCalcPosVecParam*/
		TLGCSimPointSummary(LSPosVecIter point);

		//! copy constructor
		TLGCSimPointSummary(const TLGCSimPointSummary&);

		//! destructor
		virtual  ~TLGCSimPointSummary();

	//@}


	/*!@name member functions */
	//@{
		//! copy assignment operator
		TLGCSimPointSummary& operator=(const TLGCSimPointSummary& );

		//! equivalence operator
		bool	operator==(const TLGCSimPointSummary& ) const;

		/*! add a new value in the sum of the residus*/
		void	addNewResValue(const TFreeVector& res);

		/*! get the iterator to the LS Position Vector Parameters of this entity*/
		LSPosVecIter	getPosVec() const { return fPoint; }

		/*! get the sum of residuals*/
		TFreeVector		getSumRes() const { return fSumRes; }

		/*! get the sum of squares of residuals*/
		TFreeVector		getSumRes2() const { return fSumRes2; }

		/*! get the minimum of residuals*/
		TFreeVector		getMinRes() const { return fResMin; }

		/*! get the minimum of residuals*/
		TFreeVector		getMaxRes() const { return fResMax; }

private:
		/*! update, if necessary, the value of the minimum of residus*/
		void	ifNecessarySetMin(const TFreeVector& res);

		/*! update, if necessary, the value of the minimum of residus*/
		void	ifNecessarySetMax(const TFreeVector& res);

			
	//@}


private:

	LSPosVecIter				fPoint; /*!< iterator to a LSCalc Spatial Point*/

	TFreeVector					fSumRes; /*!< sum of the residus from all the simulations for this point */
	TFreeVector					fResMin; /*!< minimum of the residus from all the simulations for this point */
	TFreeVector					fResMax; /*!< maximum of the residus from all the simulations for this point */
	TFreeVector					fSumRes2; /*!< sum of the square of the residus from all the simulations for this point */

	bool						fFirstSim;
	
	//ClassDef(TLGCSimPointSummary, 1)
};


/*!@name Typedefs */
//@{
/*! List of working points from simulations */
typedef list< TLGCSimPointSummary >		SimPointContainer;
/*! Iterator of the list of TLGCSimPointSummary */
typedef SimPointContainer::iterator		SimPointIterator;
/*! Constant iterator of the list of TLGCSimPointSummary */
typedef SimPointContainer::const_iterator	SimPointConstIter;
//@}


#endif // SU_LGC_SIM_PT_SUMMARY