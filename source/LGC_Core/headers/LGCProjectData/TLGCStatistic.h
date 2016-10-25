// TLGCStatistic.h 
#ifndef LGC_STATISTIC
#define LGC_STATISTIC

#include "TSparseMatrix.h"
#include <memory>
#include "Defaults.h"

class TLSInputMatrices;
class TLSResultsMatrices;

/*!
	\ingroup LGCProjectData

	Statistics definitions
*/
class TLGCStatistic{

public:	
	TLGCStatistic();
	~TLGCStatistic();

    ///Computes the statistic vector
	void calcReliabilityVector(TReal alpha, TReal beta, const TLSInputMatrices* inputMtr, TLSResultsMatrices* rm, bool hasPdor);

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
	 //@}

	/// Returns TRUE if reliabilities have been calculated
	bool		getAreDetermined(int index) const {return (*fAreDetermined)(index);}
	/// Returns TRUE if normalised residual has been calculated
	bool		getWToCompute() const {return fWToCompute; }
	/// Returns TRUE if the probable size of the detected fault has been calculated
	bool		getGToCompute(int index) const { return (*fGToCompute)(index); }
	/// Returns TRUE if external reliability has been calculated
	bool		getDeltaComputed(int index) const { return (*fDeltaComputed)(index); }

	/// Reset the vectors to a null vector
	void clearVectors();
	/// Initialise the vectors with the size of the input matrices
	void initialiseStatVector(const TLSInputMatrices* im);

private:
	///Calculate the overall
	void    calcOverall(int nbObs);

	std::string			fError;		/*!< errors during calculation */

	/*No assignable nor copyable*/
	TLGCStatistic(const TLGCStatistic&) {}
    TLGCStatistic& operator=(const TLGCStatistic&) { return *this; }

	std::unique_ptr<TVector>		fZ; /*!< local reliability statistic */
	std::unique_ptr<TVector>		fW; /*!< Gross-error detection statistic */
	std::unique_ptr<TVector>		fT; /*!< Level of ease a gross error of size NABLA can be detected */
	std::unique_ptr<TVector>		fDelty; /*!< Maximum effect of an undetected gross-error */
	std::unique_ptr<TVector>		fNablaValue; /*!< Value for greatest undetected error */
	std::unique_ptr<TVector>		fGValue; /*!< Value for estimation of the gross-error made with the statistic wi */
	TReal							fOverall; /*!< global network reliability */

	std::unique_ptr<TVector>		fAreDetermined; /*!< indicates if the error detection params can be computed */
	bool							fWToCompute; /*!< Indicates if the w param is to compute: false if simulation */
	std::unique_ptr<TVector>		fGToCompute; /*!< indicates if G needs to be computed */
	std::unique_ptr<TVector>		fDeltaComputed; /*!< indicates that the parameters for statistic tests have been rightly chosen */

};
#endif 