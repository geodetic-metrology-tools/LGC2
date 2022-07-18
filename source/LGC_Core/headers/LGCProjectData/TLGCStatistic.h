/*
© Copyright CERN 2000-2022. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef LGC_STATISTIC
#define LGC_STATISTIC

//STL
#include <memory>
//SURVEYLIB
#include <Serializer.hpp>
#include <TSparseMatrix.h>
//LGC
#include <Defaults.h>


class TLSInputMatrices;
class TLSResultsMatrices;

/*!
	\ingroup LGCProjectData

	Statistics definitions
*/
class TLGCStatistic : public Serializable{

public:	
	TLGCStatistic();
	~TLGCStatistic();

	///Computes the statistic vector
	void calcReliabilityVector(TReal alpha, TReal beta, const TLSInputMatrices* inputMtr, TLSResultsMatrices* rm, bool hasPdor);

	// Inherited via Serializable
	virtual void serialize(SerializerObject::SerializationHelper &obj) const override;

	/// Access to eventual error 
	std::string		getError() const { return fError; }

	/*!@name Access to statistic vectors  */
	//@{ 
	/// Returns vector of local reliability
	const TVector&			getZi() const {return *fZ.get();}
	/// Returns vector of normalised residual
	const TVector&			getWi() const {return *fW.get();} 
	/// Returns vector of internal reliability
	const TVector&			getTi() const {return *fT.get();} 
	/// Returns vector of external reliability
	const TVector&			getDelty() const {return *fDelty.get();}
	/// Returns vector of the smallest detectable fault for the observation in question
	const TVector&			getNabla() const {return *fNablaValue.get();}
	/// Returns vector of probable size of the detected fault
	const TVector&			getGi() const {return *fGValue.get();}
	/// Returns the global reliability
	const TReal				getOVERALL() const {return fOverall;}
	/// Returns the degrees of freedom
	const TReal				getDOF() const {return fDegreesOfFreedom;}
	//@}

	/// Returns TRUE if reliabilities have been calculated
	bool		getAreDetermined(int index) const {return (*fAreDetermined)(index) != 0;}
	/// Returns TRUE if normalised residual has been calculated
	bool		getWToCompute() const {return fWToCompute; }
	/// Returns TRUE if the probable size of the detected fault has been calculated
    bool		getGToCompute(int index) const { return (*fGToCompute)(index) != 0; }
	/// Returns TRUE if external reliability has been calculated
    bool		getDeltaComputed(int index) const { return (*fDeltaComputed)(index) != 0; }

	/// Reset the vectors to a null vector
	void clearVectors();
	/// Initialise the vectors with the size of the input matrices
	void initialiseStatVector(const TLSInputMatrices* im);

    /// Assignmen operator
    TLGCStatistic& operator=(const TLGCStatistic&);

private:
	///Calculate the overall
	void    calcOverall(int nbObs);
	///Calculate the degrees of freedom
	void    calcDegreesOfFreedom(int nbObs);

	std::string			fError;		/*!< errors during calculation */

	/*Not copyable*/
	TLGCStatistic(const TLGCStatistic&) {}

	std::unique_ptr<TVector>		fZ; /*!< local reliability statistic */
	std::unique_ptr<TVector>		fW; /*!< Gross-error detection statistic */
	std::unique_ptr<TVector>		fT; /*!< Level of ease a gross error of size NABLA can be detected */
	std::unique_ptr<TVector>		fDelty; /*!< Maximum effect of an undetected gross-error */
	std::unique_ptr<TVector>		fNablaValue; /*!< Value for greatest undetected error */
	std::unique_ptr<TVector>		fGValue; /*!< Value for estimation of the gross-error made with the statistic wi */
	TReal							fOverall; /*!< global network reliability */
	TReal							fDegreesOfFreedom; /*!< network degrees of freedom */

	std::unique_ptr<TVector>		fAreDetermined; /*!< indicates if the error detection params can be computed */
	bool							fWToCompute; /*!< Indicates if the w param is to compute: false if simulation */
	std::unique_ptr<TVector>		fGToCompute; /*!< indicates if G needs to be computed */
	std::unique_ptr<TVector>		fDeltaComputed; /*!< indicates that the parameters for statistic tests have been rightly chosen */

};
#endif 