#ifndef LS_INPUT_MATRICES_FILLER
#define LS_INPUT_MATRICES_FILLER

// STL
#include <vector>
// LGC
#include <Global.h>
#include <TContributionsGenerator.h>
#include <TLGCData.h>
#include <TPointTransformer.h>

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
struct INCLYROM;
struct TECWSROM;
struct TECWIROM;

/*!
	\ingroup LSCalculation

	\brief Class used to fill the input LS matrices with contributions obtained from the contribution generator.
*/
class TLSInputMatricesFiller
{
public:
	/*!@name Constructor / Destructor*/
	//@{
	/*!
		\brief Contructor

		\param[in] tree Tree of local object reference frames.
		\param[in] referentiel Global reference frame of the project.
	*/
	TLSInputMatricesFiller(const TDataTree *tree, const TRefSystemFactory::ERefFrame &referentiel, const TLGCData &data);

	/// Destructor
	~TLSInputMatricesFiller();
	//@}

	/*!
		\brief Fills the input matrices with calculated contributions of the measurements.

		\param[in] projData Project data.
		\param[in] fillWeightUnkn If the weight unknown matrix will be filled or not.
		\param[out] inpMtr Input matrices to be filled.
	*/
	bool fillMatrices(TLGCData *projData, TLSInputMatrices *matrices);

private:
	/// Default constructor: not implemented
	TLSInputMatricesFiller();
	///  Copy constructor: not implemented
	TLSInputMatricesFiller(const TLSInputMatricesFiller &source);
	/// Equality operator: not implemented
	TLSInputMatricesFiller &operator=(const TLSInputMatricesFiller &right);

	/// Contributions generator, used to calculated the contributions to be inserted into the input matrices.
	TContributionsGenerator fCGenerator;

	/// Point transformation used to calculate the contribution generators
	TPointTransformer fPointTransformer;

	/// Initialise the dimensions of the input matrices from the project data.
	void initMatriceDimension(const TLGCData &projData, TLSInputMatrices *matrices);

	// fill in the parameter weight constraints
	bool fillParameterWeights(TLGCData *projData, TLSInputMatrices *matrices);
	// fill in the slave constraints
	bool fillSlaveConstraints(TLGCData *projData, TLSInputMatrices *matrices);
	// fill in the point group constraints
	bool fillPointGroupConstraints(TLGCData *projData, TLSInputMatrices *matrices);

	/*!@name Methods to add the design matrix contributions for each type of observation*/
	//@{
	/// Add the design matrices contributions for the PLR3D observation
	void addPLR3DContributions(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the spatial dist. observations
	void addSpaDistContributions(std::list<TLINE> &distMeas, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices);
	void addSpaDistContributionsFrame(std::list<TLINE> &distMeas, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the hor. angle observations
	void addHorAngContributions(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices);
	void addHorAngContributionsFrame(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the zenithal angle observations
	void addZenDistContributions(std::list<TZEND> &zendMeas, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices);
	void addZenDistContributionsFrame(std::list<TZEND> &zendMeas, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the hor. dist. observations
	void addHorDistContributions(std::list<TLINE> &dhorMeas, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the ecth observations
	void addECTHContributions(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the ecdir observations
	void addECDIRContributions(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices);

	/// Add CAMERA UVEC observation
	void addUVECContribution(TCAM &camera, TLSInputMatrices *matrices);
	/// Add CAMERA UVD observation
	void addUVDContribution(TCAM &camera, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the Levelling station, contatining a DLEV and dlev's DHOR measurements.
	void addLevelStContributions(TLEVEL &levelSt, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the DSPT measurement (spatial measurement done be electronic distance meter)
	void addDSPTContribution(std::list<TDSPT> &dsptMeas, const TEDM &edmST, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the DVER measurement.
	void addDVERContribution(const std::list<TDVER> &dverMeas, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the ECHO measurement.
	void addECHOContributions(TECHOROM &echoRom, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the ECVE measurement.
	void addECVEContributions(TECVEROM &ecveRom, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the ecth observations
	void addECSPContributions(TECSPROM &ecspRom, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the ORIE measurement.
	void addORIEContributions(TORIEROM &orieRom, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the PDOR measurement.
	void addPDORContributions(const TPdorObs &pdorObs, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the RADI measurement.
	void addRADIContributions(const std::list<TRADI> &radiMeas, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the OBSXYZ measurement.
	void addOBSXYZContributions(const std::list<TOBSXYZ> &OBSXYZMeas, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the INCLY measurement.
	void addINCLYContributions(TINCLYROM &inclyROM, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the ROLLY measurement.
	void addROLLYContributions(TROLLYROM &rollyROM, TLSInputMatrices *matrices);

	/// Private helper function to unify INCLY and ROLLY contribution processing
	template<typename TROM, typename TMeasList, typename TGetContrib>
	void addINCLContributionsHelper(TROM &rom, TMeasList &measurements, TGetContrib getContrib, 
		TLSInputMatrices *matrices, const std::string &measurementType);

	/// Add the design matrices contributions for the ECWS measurement.
	void addECWSContributions(TECWSROM &ecwsROM, TLSInputMatrices *matrices);

	/// Add the design matrices contributions for the ECWI measurement.
	void addECWIContributions(TECWIROM &ecwiROM, TLSInputMatrices *matrices);
	//@}

	/*!@name Supporting methods*/
	//@{
	/// Fills contributions of an adjustable transformation
	bool addTransformationContribution(const TAdjustableHelmertTransformation &trafo, const TransformationContrib &trContrib, int eqIndex, TLSInputMatrices *matrices);
	bool addConstraintTransformationContribution(const TAdjustableHelmertTransformation &trafo, const TransformationContrib &trContrib, int eqIndex, TLSInputMatrices *matrices);
	/// Fills contributions of an adjustable point
	bool addPointContribution(const LGCAdjustablePoint &pointAdj, const TFreeVector &pointContrib, int eqIdx, TLSInputMatrices *matrices);
	bool addPointConstraintContribution(const LGCAdjustablePoint &pointAdj, const TFreeVector &pointContrib, int eqIdx, TLSInputMatrices *matrices);
	//@}
};

#endif // TLS_INPUT_MATRICES_FILLER
