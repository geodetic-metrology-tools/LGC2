////////////////////////////////////////////////////////////////////
// TLGCObsSummary.cpp : implementation file
// Class containing summary data for a given observation type in a LS calculation.
// Mean, standard error, confidence limits and histogram information.
//
//
// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////


#include <ctime>

#include "TLGCObsSummary.h"
#include "QuantileFunctions.h"
#include <Global.h>

//////////////////////
// Default Constructor
//////////////////////
TLGCObsSummary::TLGCObsSummary() 
{
	// clear the statistics 
	clear();
}




///////////////////
// copy constructor
///////////////////
TLGCObsSummary::TLGCObsSummary(const TLGCObsSummary& source) 
{
	*this = source;
}

/////////////
// destructor
/////////////
TLGCObsSummary::~TLGCObsSummary()
{
}


///////////////////////////
// copy assignment operator
///////////////////////////
TLGCObsSummary& TLGCObsSummary::operator=(const TLGCObsSummary& right)
{
	// compare values to see if objects are the same
	if (this != &right)
	{
	// statistics on the observation residuals
		fObsText = right.fObsText;
		fAngleType = right.fAngleType;
		fNumberOfObs = right.fNumberOfObs;
		fSumRes = right.fSumRes;
		fMean = right.fMean;
		fMeanLoLimit = LITERAL(0.0);
		fMeanHiLimit = LITERAL(0.0);
		fMeanCalculated = right.fMeanCalculated;
        fResMin = right.fResMin;
        fResMax = right.fResMax;
		fSumRes2 = right.fSumRes2;
		fVariance = right.fVariance;
		fVarLoLimit = LITERAL(0.0);
		fVarHiLimit = LITERAL(0.0);
		fVarianceCalculated = right.fVarianceCalculated;
		// histogram data
		fHistoList = right.fHistoList;
		fHistoListSorted = right.fHistoListSorted;
        fIsInitialised = right.fIsInitialised;
	}

	return *this;
}


///////////////////////
// equivalence operator
///////////////////////
bool	TLGCObsSummary::operator==(const TLGCObsSummary& right) const
{
	return (
		// statistics on the observation residuals
		fObsText == right.fObsText &&
		fAngleType == right.fAngleType &&
		fNumberOfObs == right.fNumberOfObs &&
		fSumRes == right.fSumRes &&
		fMean == right.fMean &&
		fMeanLoLimit == right.fMeanLoLimit &&
		fMeanHiLimit == right.fMeanHiLimit &&
		fMeanCalculated == right.fMeanCalculated &&
        fResMin == right.fResMin &&
        fResMax == right.fResMax && 
		fSumRes2 == right.fSumRes2 &&
		fVariance == right.fVariance &&
		fVarLoLimit == right.fVarLoLimit &&
		fVarHiLimit == right.fVarHiLimit &&
		fVarianceCalculated == right.fVarianceCalculated &&
		// histogram data
		fHistoList == right.fHistoList &&
		fHistoListSorted == right.fHistoListSorted &&
        fIsInitialised == right.fIsInitialised
		);
}


////////////////////////////////////////////
// add a new value in the sum of the residus
////////////////////////////////////////////
void	TLGCObsSummary::addNewResidual(const TReal res)
{
    fIsInitialised = true;

	// add the residual to the parameters to calculate the mean and variance
	fNumberOfObs++;
	fSumRes += res;
	fSumRes2 += powq(res,2);
	fMeanCalculated = false;
	fVarianceCalculated = false;

    // Check if residual is smaller or bigger than current resMin or resMax:
    if(res > fResMax) fResMax = res;
    if(res < fResMin) fResMin = res;

	// add the residual to the histogram data list
	addHistoListItem(res); 
	fHistoListSorted = false;

	return;
}


////////////////////////////////////////////
// add a new value in the sum of the residus
////////////////////////////////////////////
void	TLGCObsSummary::clear()
{
    fObsText = "";
    fAngleType = true;
	// reset the statistics and clear the histogram list
	fNumberOfObs = 0;
	fSumRes = LITERAL(0.0);
	fMean = LITERAL(0.0);
	fMeanLoLimit = LITERAL(0.0);
	fMeanHiLimit = LITERAL(0.0);
	fMeanCalculated = false;
    fResMin = 100.0;
    fResMax = 0.0;
	fSumRes2 = LITERAL(0.0);
	fVariance = LITERAL(0.0);
	fVarLoLimit = LITERAL(0.0);
	fVarHiLimit = LITERAL(0.0);
	fVarianceCalculated = false;
	fHistoList.clear();
	fHistoListSorted = false;
	fNumberOutsideHisto = 0;

    fIsInitialised = false;
	return;
}

bool TLGCObsSummary::isInitialised() const {
    return fIsInitialised;
}


/* get the mean of the residuals */
TReal		TLGCObsSummary::getMean()
{
	// if not already calculated, determine the mean for the observation residuals
	if( !fMeanCalculated && fNumberOfObs > 0 )
	{
		//calculate the mean
		fMean = (fSumRes)/(fNumberOfObs);
		
		fMeanCalculated = true;
	}

	return fMean;
}


/* get the lower confidence limit for the mean */
TReal		TLGCObsSummary::getMeanLoLimit()
{
	TReal var, prob, STLo;
	int	dof;

	//Degree of freedom
	dof = fNumberOfObs - 1;

	// if not already calculated, determine the mean for the observation residuals
	if( dof > 0 )
	{
		// initialise some parameters
		prob = LITERAL(0.975); 
		var = getVariance();

		// calculated the confidence limits from the Student T distribution
        STLo = deviates_students_t_lower_tail(1-prob, dof);

		//Limits
		fMeanLoLimit = STLo * var / sqrtq(TReal(fNumberOfObs));
	}

	return fMeanLoLimit;
}

/* get the upper confidence limit for the mean */
TReal		TLGCObsSummary::getMeanHiLimit()
{
	TReal var, prob, STHi;
	int	dof;

	//Degree of freedom
	dof = fNumberOfObs - 1;

	// if not already calculated, determine the mean for the observation residuals
	if( dof > 0 )
	{
		// initialise some parameters
		prob = LITERAL(0.975); 
		var = getVariance();

		// calculated the confidence limits from the Student T distribution
        STHi = deviates_students_t_upper_tail(1-prob, dof);

		//Limits
		fMeanHiLimit = STHi * var / sqrtq(TReal(fNumberOfObs));
	}

	return fMeanHiLimit;
}


/*!get the variance for the residuals */
TReal		TLGCObsSummary::getVariance()
{
	// if not already calculated, determine the variance for the observation residuals
	if( !fVarianceCalculated && fNumberOfObs > 1 )
	{
		//calculate the variance
		fVariance = sqrtq((fSumRes2 - (fSumRes)*(fSumRes)/fNumberOfObs)/(fNumberOfObs-1));

		fVarianceCalculated = true;
	}

	return fVariance;
}


/* get the lower confidence limit for the variance */
TReal		TLGCObsSummary::getVarLoLimit()
{
	TReal var, prob, chiLo;
	int	dof;

	//Degree of freedom
	dof = fNumberOfObs - 1;

	// if not already calculated, determine the variance for the observation residuals
	if( dof > 0 )
	{
		// initialise some parameters
		prob = LITERAL(0.975); 
		var = getVariance();

		// chi test coefficients
        chiLo = deviates_chi_sq(prob, dof);

		//Limits
		fVarLoLimit = fVariance * sqrtq(dof/chiLo);
	}

	return fVarLoLimit;
}


/* get the upper confidence limit for the variance */
TReal		TLGCObsSummary::getVarHiLimit()
{
	TReal var, prob, chiHi;
	int	dof;

	//Degree of freedom
	dof = fNumberOfObs - 1;

	// if not already calculated, determine the variance for the observation residuals
	if( dof > 0 )
	{
		// initialise some parameters
		prob = LITERAL(0.975); 
		var = getVariance();

		// chi test coefficients
        chiHi = deviates_chi_sq(1-prob, dof);

		//Limits
		fVarHiLimit = fVariance * sqrtq(dof/chiHi);
	}

	return fVarHiLimit;
}


/*! get the histogram data corresponding to the obersation residuals */
const list<int>		TLGCObsSummary::getHistogramData()
{
	list<int> result;
	list<TReal>::iterator startIter, finishIter;


	result.clear();
	fNumberOutsideHisto = 0;

	// if there are some residuals create the histogram data
	if( getNumberOfObs() > 0)
	{
		bool done = false;
		TReal colWidth = 1;
		TReal minVal, maxVal;

		// get the maximum and minimum residual values
		maxVal = getHistoHiLimit();
		minVal = getHistoLoLimit();

		// get the scale factor to apply for the residuals
		int k = getHistoScale();
		
		// if there are residuals beyond the scaled limits of the histogram, count them
		// and obtain an iterator to the first and the "one-beyond-last" residuals in range
		//
		//first the lower limit (scaled residuals less than -20, the min limit of the histogram)
		startIter = fHistoList.begin();
		finishIter = fHistoList.end();

		while( startIter != finishIter && (*startIter)*k <= minVal )
		{
			startIter++;
			fNumberOutsideHisto++;
		}
		
		// then the upper limit (scaled residuals greater than 20, the max limit of the histogram)
		while( finishIter != startIter && !done )
		{
			if( (*(--finishIter))*k >= maxVal )
			{
				fNumberOutsideHisto++;
			}
			else
			{
				finishIter++;
				done = true;
			}
		}

		// generate the histogram data
		while( startIter != finishIter || (minVal + colWidth) < maxVal )
		{
			int i = 0;//nbre d element dans une colonne
			while( startIter != finishIter && (*startIter)*k <= (minVal + colWidth) )
			{
				startIter++;
				i++;
			}
			result.push_back(i);
			colWidth += LITERAL(1.0);
		}
	}

	return result;
}


/* get the lower limit for the histogram residuals */
TReal		TLGCObsSummary::getHistoLoLimit()
{
	TReal loLimit;
	
	if( !fHistoListSorted )
	{
        fHistoList.sort();
		fHistoListSorted = true;
	}
	
	// the lower limit is the greater of the lowest residual and -20
	loLimit = max( (double) floorq(fHistoList.front()*getHistoScale()) - LITERAL(1.0), -LITERAL(20.0));

	return loLimit;
}


/* get the upper limit for the histogram residuals */
TReal		TLGCObsSummary::getHistoHiLimit()
{
	TReal hiLimit;
	
	if( !fHistoListSorted )
	{
        fHistoList.sort();
		fHistoListSorted = true;
	}
	
	// the lower limit is the smaller of the highest residual and 20
	hiLimit = min( (double) ceilq(fHistoList.back()*getHistoScale()) + LITERAL(2.0), LITERAL(20.0));

	return hiLimit;
}


/* get the scale factor for the histogram residuals */
int		TLGCObsSummary::getHistoScale()
{
	int k = 1;
	if(getVariance()<=1)
	{
		k = 10;
	}
	if(getVariance()<=LITERAL(0.1))
	{
		k = 100;
	}

	return k;
}

/* get the scale factor for the histogram residuals */
int		TLGCObsSummary::getNumBeyondHistoLimits()
{
	getHistogramData();

	return fNumberOutsideHisto;
}

