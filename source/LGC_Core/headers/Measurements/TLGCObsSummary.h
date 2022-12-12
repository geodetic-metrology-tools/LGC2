/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_LGC_OBS_SUMMARY
#define SU_LGC_OBS_SUMMARY


//STL
#include <list>
#include <map>
#include <string>
//LGC
#include <TALGCObjectWriter.h>

#if USE_SERIALIZER
    #include <Serializer.hpp>
#endif // USE_SERIALIZER

/*!
\ingroup Measurements
\brief Class containing summary data for a given observation type in a LS calculation. Mean, standard error, confidence limits and histogram information.
*/
#if USE_SERIALIZER
class TLGCObsSummary : public Serializable
#else
class TLGCObsSummary
#endif // USE_SERIALIZER
{
public:
    /*!@name Constructors and Destructors */
    //@{
    /// default constructor
    TLGCObsSummary();
    //@}


    /*!@name public member functions */
    //@{

    /// add a new value in the sum of the residus
    /**
    * @note After adding all the residuals, the obsSummary must be
    * initialised (TLGCObsSummary::initialise()) before it can be used.
    * @note getResMax() and getResMin() will return the residuals in
    * the same units as they are added originally.
    */
    void		addNewResidual(const TReal res);

    /// clear the observation summary statistics 
    void		clear();

    /// Check if the obsSummary statistics are initialised or not
    bool        isInitialised() const;

    /// Initialise the observation summary
    void        initialise();

    /// get the mean of the residuals 
    TReal		getMean() const;

    /// get the lower confidence limit for the mean 
    TReal		getMeanLoLimit() const;

    /// get the upper confidence limit for the mean 
    TReal		getMeanHiLimit() const;

    /// Get the minimum residual of the added residuals
    TReal       getResMin() const;

    /// Get the maximum residual of the added residuals
    TReal       getResMax() const;

    /// get the standard deviation for the residuals 
    TReal		getStdev() const;

    /// get the lower confidence limit for the standard deviation 
    TReal		getStdLoLimit() const;

    /// get the upper confidence limit for the standard deviation 
    TReal		getStdHiLimit() const;

    /// set a string to identify the observation type 
    void		setObsText(std::string keyWord) noexcept { fObsText = keyWord;  }

    /// get the observation type 
	std::string		getObsText() const { return fObsText; }

    /// define if the observation is an angle 
    void		defineAngleObservation() { fAngleType = true; };

    /// define if the observation is a length 
    void		defineLengthObservation() { fAngleType = false; };

    /// indicate if the summary is for an angle type measurement or a length type measurement 
    bool		isAngleType() const { return fAngleType; }

    /// get the number of observations 
    int			getNumberOfObs() const { return fNumberOfObs; }

    /// get the histogram data corresponding to the obersation residuals 
    const std::list<int>	getHistogramData() const;

    /// get the lower limit for the histogram data 
    TReal		getHistoLoLimit() const;

    /// get the higher limit for the histogram data 
    TReal		getHistoHiLimit() const;

    /// get the scale factor for the histogram residuals 
    int			getHistoScale() const;

    /// get the number of residuals outside the histogram limits 
    int			getNumBeyondHistoLimits() const;

    /// Merge the given obsSummaries into one and return it initialised if *summaries* was not an empty list
    /**
    * @note The summaries to merge must be of same type (i.e., either length or angle type)
    * @note The obs text is defined by the first obsSummary in the given list
    */
    static TLGCObsSummary merge(const std::list<const TLGCObsSummary*> &summaries);

    /// Determine if the histogram data should be initialised and used (NB. Affects all created observation summaries)
    static void createHistogram(bool create){ fCreateHistogram = create; }

    //@}
    #if USE_SERIALIZER
	    // Inherited via Serializable
	    virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
    #endif

protected:

    /// add an item to the list used for histogram
    void	addHistoListItem(const TReal item) { fHistoList.push_back(item); }


private:
    // statistics on the observation residuals
	std::string			fObsText;
    bool			fAngleType;
    int				fNumberOfObs;
    TReal			fSumRes;
    TReal			fMean;
    TReal			fMeanLoLimit;
    TReal			fMeanHiLimit;
    TReal           fResMin;
    TReal           fResMax;
    TReal			fSumRes2;
    TReal			fStdev;
    TReal			fStdLoLimit;
    TReal			fStdHiLimit;

    // histogram data
    static bool     fCreateHistogram;
	std::list<TReal>	    fHistoList;
	std::list<int>       fHistoData;
    int				fNumberOutsideHisto;

    bool            fIsInitialised;
};

/*!
\ingroup Measurements
\brief A structure containing observation summaries (\ref TLGCObsSummary) of the three observations of the TPLR3D observation.
*/
#if USE_SERIALIZER
struct TPOLARObsSummary : public Serializable
#else
struct TPOLARObsSummary
#endif // USE_SERIALIZER
{
    TLGCObsSummary distObsSum;
    TLGCObsSummary anglObsSum;
    TLGCObsSummary zendObsSum;

#if USE_SERIALIZER
	// Inherited via Serializable
	inline void serialize(SerializerObject::SerializationHelper &obj) const
	{
		if (distObsSum.getNumberOfObs())
		    obj.addProperty("distObsSum", distObsSum);
		if (anglObsSum.getNumberOfObs())
			obj.addProperty("anglObsSum", anglObsSum);
		if (zendObsSum.getNumberOfObs())
			obj.addProperty("zendObsSum", zendObsSum);
	}
#endif
};

/*!
\ingroup Measurements
\brief A structure containing observation summaries (TLGCObsSummary) of the three observations of the TUVD observation.
*/
#if USE_SERIALIZER
struct TUVDObsSummary : public Serializable
#else
struct TUVDObsSummary
#endif // USE_SERIALIZER
{
    TLGCObsSummary distObsSum;
    TLGCObsSummary xVectorCompObsSum;
    TLGCObsSummary yVectorCompObsSum;

#if USE_SERIALIZER
	// Inherited via Serializable
	inline void serialize(SerializerObject::SerializationHelper &obj) const
	{
		if (distObsSum.getNumberOfObs())
			obj.addProperty("distObsSum", distObsSum);
		if (yVectorCompObsSum.getNumberOfObs())
			obj.addProperty("yVectorCompObsSum", yVectorCompObsSum);
		if (yVectorCompObsSum.getNumberOfObs())
			obj.addProperty("yVectorCompObsSum", yVectorCompObsSum);
	}
#endif
};

/*!
\ingroup Measurements
\brief A structure containing observation summaries (TLGCObsSummary) of the three observations of the TUVEC observation.
*/
#if USE_SERIALIZER
struct TUVECObsSummary : public Serializable
#else
struct TUVECObsSummary
#endif // USE_SERIALIZER
{
    TLGCObsSummary xVectorCompObsSum;
    TLGCObsSummary yVectorCompObsSum;

#if USE_SERIALIZER
	// Inherited via Serializable
	inline void serialize(SerializerObject::SerializationHelper &obj) const
	{
		if (xVectorCompObsSum.getNumberOfObs())
			obj.addProperty("xVectorCompObsSum", xVectorCompObsSum);
		if (yVectorCompObsSum.getNumberOfObs())
			obj.addProperty("yVectorCompObsSum", yVectorCompObsSum);
	}
#endif
};

/*!
\ingroup Measurements
\brief A structure containing observation summaries (\ref TLGCObsSummary) of the three observations of the OBSXYZ observation.
*/
#if USE_SERIALIZER
struct TOBSXYZObsSummary : public Serializable
#else
struct TOBSXYZObsSummary
#endif // USE_SERIALIZER
{
    TLGCObsSummary obsXObsSum;
    TLGCObsSummary obsYObsSum;
    TLGCObsSummary obsZObsSum;
    
#if USE_SERIALIZER
	// Inherited via Serializable
	inline void serialize(SerializerObject::SerializationHelper &obj) const
	{
		if (obsXObsSum.getNumberOfObs())
			obj.addProperty("obsXObsSum", obsXObsSum);
		if (obsYObsSum.getNumberOfObs())
			obj.addProperty("obsYObsSum", obsYObsSum);
		if (obsZObsSum.getNumberOfObs())
			obj.addProperty("obsZObsSum", obsZObsSum);
	}
#endif
};

/*!
\ingroup Measurements
\brief A structure containing observation summaries (TLGCObsSummary) of the two observations of the ECWI observation.
*/
struct TECWIObsSummary
{
	TLGCObsSummary xObsSum;
	TLGCObsSummary zObsSum;
};

#endif // SU_LGC_OBS_SUMMARY
