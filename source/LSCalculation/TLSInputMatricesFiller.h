//////////////////////////////////////////////////////////////////////
// TLSInputMatricesFiller.h
/*! 
	Class responsible for reading the least squares calculation measurements and constraints 
	and filling the least squares matrices,
	specific to a least squares calculation.
*/
//////////////////////////////////////////////////////////////////////
#ifndef LS_INPUT_MATRICES_FILLER
#define LS_INPUT_MATRICES_FILLER

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
class TLSInputMatrices;
class TLGCData;
struct TCalcType;
class  TPLR3D;
class  TDIR3D;
class  TLINE;

#include "ContributionsGenerators\TContributionsGenerator.h"

using namespace std;

//! Class used to fill input matrices with data for least-square calculations.
class TLSInputMatricesFiller
{

public :	
	/*!@name Constructor / Destructor*/
	//@{
		/*! Contructor
			\param[in] tree Tree of local object references.
			\param[in] calcType Structure of description of the calculation, such as what contraints introduced, etc.
		*/
		explicit TLSInputMatricesFiller(TDataTree* tree, const TCalcType& calcType);

		/*!Destructor */
		virtual ~TLSInputMatricesFiller();
	//@}
	
	/*!Fills the input matrices (inpTr), using information from 'projData'. 
		\param[in] projData Project data.
		\param[out] inpMtr Input matrices to be filled.
	*/
	virtual bool 	fillMatrices(TLGCData* projData, TLSInputMatrices*  inpMtr);

	//! Initialise the dimensions of the input matrices from 'projData'.
	void			initMatriceDimension(const TLGCData& projData, TLSInputMatrices* matrices);
private:

	/*!Default constructor: not implemented */
	TLSInputMatricesFiller();
	/*! Copy constructor: not implemented */
	TLSInputMatricesFiller( const TLSInputMatricesFiller& source );
	/*! Equality operator: not implemented */
	TLSInputMatricesFiller& operator=( const TLSInputMatricesFiller& right );

	/*!Structure describing the calculation type*/
	const TCalcType& fcalcType;
	
	/*! Contributions generator*/
	TContributionsGenerator  fCGenerator;

	//! Fill the weight matrix for unknown parameters.
	bool	fillWeightUnkMtrx(TLGCData* projData, TLSInputMatrices* matrices);


	/*!@name Methods to add the design matrix contributions for each type of observation*/
	//@{
		void addPLR3DContribution(const TPLR3D& meas, const TTSTN::TROM& rom, const TTSTN& station, TLSInputMatrices*  matrices);
		void addPLR3DContribution(const TPLR3D& meas, const TCAM& camera, TLSInputMatrices*  matrices);

		void addDIR3DContribution(const TDIR3D& meas, const TTSTN::TROM& rom, const TTSTN& station, TLSInputMatrices* matrices);
		void addDIR3DContribution(const TDIR3D& meas, const TCAM& camera, TLSInputMatrices*  matrices);

		/*! Add the design matrices contributions for the spatial dist. observations */
		void  addSpaDistContribution(const TLINE& meas, const TTSTN& station, TLSInputMatrices*  matrices);
		/*! Add the design matrices contributions for the hor. angle observations */
		void  addHorAngContribution(const TANGL& meas, const TTSTN::TROM& rom, const TTSTN& station, TLSInputMatrices*  matrices);
		/*! Add the design matrices contributions for the zenithal angle observations */
		void  addZenDistContributions(const TZEND& meas, const TTSTN& station, TLSInputMatrices*  matrices);
		/*! Add the design matrices contributions for the hor. dist. observations */
		void  addHorDistContributions(const TLINE& meas, const TTSTN& station, TLSInputMatrices*  matrices);
		/*! Add the design matrices contributions for the Levelling station, contatining a DLEV and dlev's DHOR measurements.*/
		void  addLevelStContributions(const TLEVEL& levelSt, TLSInputMatrices*  matrices);
	//@}


	/*!@name Supporting methods doing particular job for filling observation's contributions.*/
	//@{
		bool addTransformationContribution(const TAdjustableHelmertTransformation& trafo, const TransformationContrib& trContrib, int eqIndex, TLSInputMatrices* matrices);
		bool addPLR3DBaseContrib(const TAdjustablePoint* stationPos, const TPLR3D& meas, const MatrixIndex firstEqIdx, const MatrixIndex firstObsIdx, const PLR3DContrib& contributions, TLSInputMatrices*  matrices);
		bool addDIR3DBaseContrib(const TAdjustablePoint* stationPos, const TDIR3D& meas, const MatrixIndex firstEqIdx, const MatrixIndex firstObsIdx, const DIR3DContrib& contributions, TLSInputMatrices*  matrices);
		bool addPointContribution(const TAdjustablePoint& pointAdj, const TFreeVector& pointContrib, int eqIdx, TLSInputMatrices* matrices);
	//@}
};

#endif //TLS_INPUT_MATRICES_FILLER
