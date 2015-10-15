// TLGCObsSummary.h
//
/*! Class containing summary data for a given observation type in a LS calculation.
	Mean, standard error, confidence limits and histogram information.
*/
// Patterns:
//
// 
// Copyright 2007-2008 CERN TS/SU. All rights reserved. 
// Author M. Jones
//////////////////////////////////////////////////////////////////////



#ifndef SU_LGC_OBS_SUMMARY
#define SU_LGC_OBS_SUMMARY


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include "TALGCConverter.h"
#include	<list>
#include	<map>
#include	<string>
using namespace std;
//
// typedefs
//
//
////////////////////////////////////////////////////////////////

//Class definition
class  TLGCObsSummary 
{
public:


	/*!@name Constructors and Destructors */
	//@{
		//! default constructor
		TLGCObsSummary();

		//! copy constructor
		TLGCObsSummary(const TLGCObsSummary&);

		//! destructor
		virtual  ~TLGCObsSummary();

	//@}


	/*!@name public member functions */
	//@{
		//! copy assignment operator
		TLGCObsSummary& operator=(const TLGCObsSummary& );

		//! equivalence operator
		bool		operator==(const TLGCObsSummary& ) const;

		/*! add a new value in the sum of the residus*/
		void		addNewResidual(const TReal res);

		/*! clear the observation summary statistics */
		void		clear();

		/*! get the mean of the residuals */
		TReal		getMean();

		/*! get the lower confidence limit for the mean */
		TReal		getMeanLoLimit();
		
		/*! get the upper confidence limit for the mean */
		TReal		getMeanHiLimit();

		/*!get the variance for the residuals */
		TReal		getVariance();

		/*! get the lower confidence limit for the variance */
		TReal		getVarLoLimit();
		
		/*! get the upper confidence limit for the variance */
		TReal		getVarHiLimit();

		/*! set a string to identify the observation type */
		void		setObsText(const string keyWord) {fObsText = keyWord; return;}

		/*! get the observation type */
		string		getObsText() const {return fObsText;}

		/*! define if the observation is an angle */
		void		defineAngleObservation() {fAngleType = true; return;};

		/*! define if the observation is a length */
		void		defineLengthObservation() {fAngleType = false; return;};

		/*! indicate if the summary is for an angle type measurement or a length type measurement */
		bool		isAngleType() const { return fAngleType; }

		/*! get the number of observations */
		int			getNumberOfObs() const {return fNumberOfObs; }

		/*! get the histogram data corresponding to the obersation residuals */
		const list<int>	getHistogramData();

		/*! get the lower limit for the histogram data */
		TReal		getHistoLoLimit();

		/*! get the higher limit for the histogram data */
		TReal		getHistoHiLimit();

		/*! get the scale factor for the histogram residuals */
		int			getHistoScale();

		/*! get the number of residuals outside the histogram limits */
		int			getNumBeyondHistoLimits();
//@}


protected:

	/*!add an item to the list used for histogram*/
	void	addHistoListItem(const TReal item) {fHistoList.push_back(item); return;}
			

private:
	// statistics on the observation residuals
	string			fObsText;
	bool			fAngleType;
	int				fNumberOfObs;
	TReal			fSumRes;
	TReal			fMean;
	TReal			fMeanLoLimit;
	TReal			fMeanHiLimit;
	bool			fMeanCalculated;
	TReal			fSumRes2;
	TReal			fVariance;
	TReal			fVarLoLimit;
	TReal			fVarHiLimit;
	bool			fVarianceCalculated;

	// histogram data
	list<TReal>	fHistoList;
	bool			fHistoListSorted;
	int				fNumberOutsideHisto;

	//ClassDef(TLGCObsSummary, 1)
};


/*!@name Typedefs */
//@{
	/*! List of LGC observation summary data */
	typedef list< pair<TALGCConverter::ELGCObservations, TLGCObsSummary> >	ObsSummaryContainer;
	/*! Iterator of the list of TLGCObsSummary */
	typedef ObsSummaryContainer::iterator		ObsSummaryIter;
	/*! Constant iterator of the list of TLGCObsSummary */
	typedef ObsSummaryContainer::const_iterator	ObsSummaryConstIter;
//@}


#endif // SU_LGC_OBS_SUMMARY
