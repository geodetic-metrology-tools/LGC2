////////////////////////////////////////////////////////////////////
// TLGCObsSummary.cpp : implementation file
// Class containing summary data for a given observation type in a LS calculation.
// Mean, standard error, confidence limits and histogram information.
//
//
// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////


#include <ctime>

#include "TVAdjustableObject.h" // isnotanumber
#include "TLGCObsSummary.h"
#include "QuantileFunctions.h"
#include <Global.h>

///////////////////////
// Static variables:
bool TLGCObsSummary::fCreateHistogram = false;


//////////////////////
// Default Constructor
//////////////////////
TLGCObsSummary::TLGCObsSummary() 
{
	// clear the statistics 
	clear();
}


////////////////////////////////////////////
// add a new value in the sum of the residus
////////////////////////////////////////////
void	TLGCObsSummary::addNewResidual(const TReal res)
{
	// add the residual to the parameters to calculate the mean and variance
	fNumberOfObs++;
	fSumRes += res;
	fSumRes2 += powq(res,2);

    // Check if residual is smaller or bigger than current resMin or resMax:
    if(res > fResMax) fResMax = res;
    if(res < fResMin) fResMin = res;

	// add the residual to the histogram data list
	addHistoListItem(res); 
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
    fResMin = std::numeric_limits<float>::max();
    fResMax = std::numeric_limits<float>::lowest();
	fSumRes2 = LITERAL(0.0);
	fStdev = LITERAL(0.0);
	fStdLoLimit = LITERAL(0.0);
	fStdHiLimit = LITERAL(0.0);
	fHistoList.clear();
    fHistoData.clear();
	fNumberOutsideHisto = 0;

    fIsInitialised = false;
	return;
}

bool TLGCObsSummary::isInitialised() const {
    return fIsInitialised;
}

void TLGCObsSummary::initialise() {

    // Set the initialisation flag true here, because some
    // functions that check it are called in the following
    fIsInitialised = true;

    // ------------- RESIDUALS and VARIANCE: -------------

    // determine the mean for the observation residuals
    if(fNumberOfObs > 0)
        //calculate the mean
        fMean = (fSumRes) / (fNumberOfObs);

    // determine the standard deviation for the observation residuals
    if(fNumberOfObs > 1)
        //calculate the standard deviation
		fStdev = sqrtq((fSumRes2 - (fSumRes) * (fSumRes) / fNumberOfObs) / (fNumberOfObs - 1));

    //Degree of freedom
    int dof = fNumberOfObs - 1;

    // Calculate the mean low and high limits, and the standard deviation high and low limits
    if(dof > 0)
    {
        TReal prob, STLo, STHi, chiLo, chiHi;

        // initialise some parameters
        prob = LITERAL(0.975); 

        // calculate the confidence limits from the Student T distribution
        STLo = deviates_students_t_lower_tail(1 - prob, dof);
        fMeanLoLimit = STLo * fStdev / sqrtq(TReal(fNumberOfObs));

        // calculate the confidence limits from the Student T distribution
        STHi = deviates_students_t_upper_tail(1 - prob, dof);
		fMeanHiLimit = STHi * fStdev / sqrtq(TReal(fNumberOfObs));

        // chi test coefficients
        chiLo = deviates_chi_sq(prob, dof);
		fStdLoLimit = fStdev * sqrtq(dof / chiLo);

        // chi test coefficients
        chiHi = deviates_chi_sq(1 - prob, dof);
		fStdHiLimit = fStdev * sqrtq(dof / chiHi);
    }


    // ------------- HISTOGRAM: ------------

    // Histogram data:
    if(fCreateHistogram){

        fHistoData.clear();
        fNumberOutsideHisto = 0;

        // if there are some residuals create the histogram data
        if(getNumberOfObs() > 0)
        {
            bool done = false;
            TReal colWidth = 1;
            TReal minVal, maxVal;

            // Sort the histogram list:
            fHistoList.sort();

            // get the maximum and minimum residual values
            maxVal = getHistoHiLimit();
            minVal = getHistoLoLimit();

            // get the scale factor to apply for the residuals
            int k = getHistoScale();

            // if there are residuals beyond the scaled limits of the histogram, count them
            // and obtain an iterator to the first and the "one-beyond-last" residuals in range
            //
            //first the lower limit (scaled residuals less than -20, the min limit of the histogram)
            auto startIter = fHistoList.begin();
            auto finishIter = fHistoList.end();

            while(startIter != finishIter && (isnotanumber(*startIter) || (*startIter)*k <= minVal))
            {
                startIter++;
                fNumberOutsideHisto++;
            }

            // then the upper limit (scaled residuals greater than 20, the max limit of the histogram)
            while(finishIter != startIter && !done)
            {
                --finishIter;
                if(isnotanumber(*(finishIter)) || (*(finishIter))*k >= maxVal)
                {
                    fNumberOutsideHisto++;
                } else
                {
                    finishIter++;
                    done = true;
                }
            }

            // generate the histogram data
            while(startIter != finishIter || (minVal + colWidth) < maxVal)
            {
                int i = 0;//nbre d element dans une colonne
                while(startIter != finishIter && (*startIter)*k <= (minVal + colWidth))
                {
                    startIter++;
                    i++;
                }
                fHistoData.push_back(i);
                colWidth += LITERAL(1.0);
            }
        }
    }
}

/* get the mean of the residuals */
TReal TLGCObsSummary::getMean() const {
    assert(fIsInitialised);
    return fMean;
}


/* get the lower confidence limit for the mean */
TReal TLGCObsSummary::getMeanLoLimit() const {
    assert(fIsInitialised);
    return fMeanLoLimit;
}

/* get the upper confidence limit for the mean */
TReal TLGCObsSummary::getMeanHiLimit() const {
    assert(fIsInitialised);
    return fMeanHiLimit;
}

TReal TLGCObsSummary::getResMin() const {
    assert(fIsInitialised);
    return fResMin;
}

TReal TLGCObsSummary::getResMax() const {
    assert(fIsInitialised);
    return fResMax;
}

/*!get the standard deviation for the residuals */
TReal TLGCObsSummary::getStdev() const {
    assert(fIsInitialised);
    return fStdev;
}


/* get the lower confidence limit for the standard deviation */
TReal TLGCObsSummary::getStdLoLimit() const {
    assert(fIsInitialised);
	return fStdLoLimit;
}


/* get the upper confidence limit for the standard deviation */
TReal TLGCObsSummary::getStdHiLimit() const {
    assert(fIsInitialised);
	return fStdHiLimit;
}


/*! get the histogram data corresponding to the obersation residuals */
const std::list<int> TLGCObsSummary::getHistogramData() const {
    assert(fIsInitialised);
    assert(fCreateHistogram);
    return fHistoData;
}


/* get the lower limit for the histogram residuals */
TReal TLGCObsSummary::getHistoLoLimit() const {
    assert(fIsInitialised);
    assert(fCreateHistogram);

	// the lower limit is the greater of the lowest residual and -20
	auto maxval = std::max( (double) floorq(fHistoList.front()*getHistoScale()) - LITERAL(1.0), -LITERAL(20.0));
    return isnotanumber(maxval) ? -LITERAL(20.0) : maxval;
}


/* get the upper limit for the histogram residuals */
TReal TLGCObsSummary::getHistoHiLimit() const {
    assert(fIsInitialised);
    assert(fCreateHistogram);

	// the lower limit is the smaller of the highest residual and 20
	auto minval = std::min( (double) ceilq(fHistoList.back()*getHistoScale()) + LITERAL(2.0), LITERAL(20.0));
    return isnotanumber(minval) ? LITERAL(20.0) : minval;
}


/* get the scale factor for the histogram residuals */
int TLGCObsSummary::getHistoScale() const
{
    assert(fCreateHistogram);

	int k = 1;
	if(getStdev()<=1)
	{
		k = 10;
	}
	if (getStdev() <= LITERAL(0.1))
	{
		k = 100;
	}

	return k;
}

/* get the scale factor for the histogram residuals */
int TLGCObsSummary::getNumBeyondHistoLimits() const {
    assert(fIsInitialised);
    assert(fCreateHistogram);
    return fNumberOutsideHisto;
}

TLGCObsSummary TLGCObsSummary::merge(const std::list<const TLGCObsSummary*> &summaries) {
    TLGCObsSummary summary;
    if(summaries.size() == 0) return summary;

    bool isAnyInitialised = false;

    bool first = true;
    for(const auto &sum : summaries){
        // Don't merge uninitialised summaries:
        if(!sum->isInitialised()) continue;
        
        isAnyInitialised = true;
        
        if(first){
            // Use the first element as the base:
            summary.fObsText = sum->fObsText;
            summary.fAngleType = sum->fAngleType;

            first = false;
        }

        assert(summary.fAngleType == sum->fAngleType);

        // Statistics about number of observations and residuals summary:
        summary.fNumberOfObs += sum->fNumberOfObs;
        summary.fSumRes += sum->fSumRes;
        summary.fSumRes2 += sum->fSumRes2;

        // Check if residual is smaller or bigger than current resMin or resMax:
        if(sum->fResMax > summary.fResMax) summary.fResMax = sum->fResMax;
        if(sum->fResMin < summary.fResMin) summary.fResMin = sum->fResMin;

        // Merge histogramlists:
        summary.fHistoList.insert(summary.fHistoList.end(), sum->fHistoList.begin(), sum->fHistoList.end());
    }

    if(isAnyInitialised) summary.initialise();
    return summary;
}

#if USE_SERIALIZER
void TLGCObsSummary::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("fAngleType", fAngleType);
	obj.addProperty("fCreateHistogram", fCreateHistogram);
	obj.addProperty("fHistoData", fHistoData);
	obj.addProperty("fHistoList", fHistoList);
	obj.addProperty("fIsInitialised", fIsInitialised);
	obj.addProperty("fMean", fMean);
	obj.addProperty("fMeanHiLimit", fMeanHiLimit);
	obj.addProperty("fMeanLoLimit", fMeanLoLimit);
	obj.addProperty("fNumberOfObs", fNumberOfObs);
	obj.addProperty("fNumberOutsideHisto", fNumberOutsideHisto);
	obj.addProperty("fObsText", fObsText);
	obj.addProperty("fResMax", fResMax);
	obj.addProperty("fResMin", fResMin);
	obj.addProperty("fSumRes", fSumRes);
	obj.addProperty("fSumRes2", fSumRes2);
	obj.addProperty("fStdHiLimit", fStdHiLimit);
	obj.addProperty("fStdev", fStdev);
	obj.addProperty("fStdLoLimit", fStdLoLimit);
}
#endif // USE_SERIALIZER


