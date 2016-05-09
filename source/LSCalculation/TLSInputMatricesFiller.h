#ifndef LS_INPUT_MATRICES_FILLER
#define LS_INPUT_MATRICES_FILLER

#include <vector>
#include "TContributionsGenerator.h"
#include "TPointTransformer.h"
#include "Global.h"

class TLSInputMatrices;
class TLGCData;
struct TCalcType;
class TPLR3D;
class TDIR3D;
class TLINE;
class TRADI;
struct TECHOROM;
struct TECVEROM;
struct TORIEROM;

/*! 
	\ingroup LSCalculation

	\brief Class used to fill the input LS matrices with contributions obtained from the contribution generator.
*/
class TLSInputMatricesFiller
{

public :	
	/*!@name Constructor / Destructor*/
	//@{
		/*! 
			\brief Contructor

			\param[in] tree Tree of local object reference frames.
			\param[in] referentiel Global reference frame of the project.
		*/
		TLSInputMatricesFiller(const TDataTree* tree, const TRefSystemFactory::ERefFrame& referentiel);

		/// Destructor 
		 ~TLSInputMatricesFiller();
	//@}
	
	/*!
		\brief Fills the input matrices with calculated contributions of the measurements.

		\param[in] projData Project data.
		\param[in] fillWeightUnkn If the weight unknown matrix will be filled or not.
		\param[out] inpMtr Input matrices to be filled.
	*/
	bool 	fillMatrices(TLGCData* projData, bool fillWeightUnkn, TLSInputMatrices*  matrices);
private:

	/// Default constructor: not implemented 
	TLSInputMatricesFiller();
	///  Copy constructor: not implemented 
	TLSInputMatricesFiller( const TLSInputMatricesFiller& source );
	/// Equality operator: not implemented 
	TLSInputMatricesFiller& operator=( const TLSInputMatricesFiller& right );
	
	/// Contributions generator, used to calculated the contributions to be inserted into the input matrices.
	TContributionsGenerator  fCGenerator;

	/// Point transformation used to calculate the contribution generators
	TPointTransformer fPointTransformer;

	/// Initialise the dimensions of the input matrices from the project data.
	void			initMatriceDimension(const TLGCData& projData, TLSInputMatrices* matrices);
 
	/// Fills the weight unknown matrix.
	bool	fillWeightUnkMtrx(TLGCData* projData, TLSInputMatrices* matrices);

	/*!@name Methods to add the design matrix contributions for each type of observation*/
	//@{
		/// Add the design matrices contributions for the PLR3D observation 
		void addPLR3DContributions(shared_ptr<TTSTN::TROM> rom, shared_ptr<TTSTN> station, TLSInputMatrices*  matrices);
		/// Add the design matrices contributions for the spatial dist. observations 
		void  addSpaDistContributions(const std::vector<TLINE>& distMeas, shared_ptr<TTSTN> station, TLSInputMatrices*  matrices);
		/// Add the design matrices contributions for the hor. angle observations 
		void  addHorAngContributions(shared_ptr<TTSTN::TROM> rom, shared_ptr<TTSTN> station, TLSInputMatrices*  matrices);
		/// Add the design matrices contributions for the zenithal angle observations 
		void  addZenDistContributions(const std::vector<TZEND>& zendMeas, shared_ptr<TTSTN> station, TLSInputMatrices*  matrices);
		/// Add the design matrices contributions for the hor. dist. observations 
		void  addHorDistContributions(const std::vector<TLINE>& dhorMeas, shared_ptr<TTSTN> station, TLSInputMatrices*  matrices);
		/// Add the design matrices contributions for the ecth observations 
		void  addECTHContributions(shared_ptr<TTSTN::TROM> rom, shared_ptr<TTSTN> station, TLSInputMatrices*  matrices);
		/// Add the design matrices contributions for the ecdir observations 
		void  addECDIRContributions(shared_ptr<TTSTN::TROM> rom, shared_ptr<TTSTN> station, TLSInputMatrices*  matrices);

		/// Add CAMERA UVEC observation
		void addUVECContribution(const TCAM& camera, TLSInputMatrices*  matrices);
		/// Add CAMERA UVD observation
		void addUVDContribution(const TCAM& camera, TLSInputMatrices*  matrices);

		/// Add the design matrices contributions for the Levelling station, contatining a DLEV and dlev's DHOR measurements.
		void  addLevelStContributions(const TLEVEL& levelSt, TLSInputMatrices*  matrices);

		/// Add the design matrices contributions for the DSPT measurement (spatial measurement done be electronic distance meter) 
		void  addDSPTContribution(const std::vector<TDSPT>& dsptMeas, const TEDM& edmST, TLSInputMatrices*  matrices);

		/// Add the design matrices contributions for the DVER measurement. 
		void  addDVERContribution(const std::vector<TDVER>& dverMeas, TLSInputMatrices*  matrices);

		/// Add the design matrices contributions for the ECHO measurement. 
		void addECHOContributions(const TECHOROM& echoRom, TLSInputMatrices*  matrices);

		/// Add the design matrices contributions for the ECVE measurement. 
		void addECVEContributions(const TECVEROM& ecveRom, TLSInputMatrices*  matrices);

		/// Add the design matrices contributions for the ecth observations 
		void  addECSPContributions(const TECSPROM& ecspRom, TLSInputMatrices*  matrices);

		/// Add the design matrices contributions for the ORIE measurement. 
		void addORIEContributions(const TORIEROM& orieRom, TLSInputMatrices*  matrices);

		/// Add the design matrices contributions for the PDOR measurement. 
		void addPDORContributions(const TPdorObs& pdorObs, TLSInputMatrices*  matrices);

		/// Add the design matrices contributions for the RADI measurement. 
		void addRADIContributions(const std::vector<TRADI>& radiMeas, TLSInputMatrices*  matrices);
	//@}


	/*!@name Supporting methods*/
	//@{
		/// Fills contributions of an adjustable transformation
		bool addTransformationContribution(const TAdjustableHelmertTransformation& trafo, const TransformationContrib& trContrib, int eqIndex, TLSInputMatrices* matrices);
		/// Fills contributions of an adjustable point
		bool addPointContribution(const TAdjustablePoint& pointAdj, const TFreeVector& pointContrib, int eqIdx, TLSInputMatrices* matrices);
	//@}
};

#endif //TLS_INPUT_MATRICES_FILLER
