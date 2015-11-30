#ifndef SU_TSTN_WRITER
#define SU_TSTN_WRITER

#include "TTSTN.h"
#include "TObservationWriter.h"
#include "TAngle.h"

/*!
	\ingroup LGCObjectWriters
	\brief Write TSTN  definition and its observations to an LGC output file.
@{*/
class  TTSTNWriter : public TObservationWriter
{
public:
	/// Constructor
	TTSTNWriter(TAStreamFormatter& stream);
	///Destructor
	virtual ~TTSTNWriter();

	///	Writes information about TSTN, its definition, calculated values and each ROM (round of measurements) as well as the observations included.
	void writeTSTNResults(const TTSTN& tstn);

	///	Writes information about TSTN, its definition, calculated values and each ROM (round of measurements) as well as the observations included.
	void writeTSTNResultsSIMU(const TTSTN& tstn);

	/// Writes TSTN header.
	void writeTSTNHeader(const TTSTN& tstn);
	/// Writes TSTN data, definition, provisional and calculated values.
	void writeTSTNData(const TTSTN& tstn);
	
	/// Writes V0 (ROM = round of measurement) header.
	void writeV0Header();
	/// Writes V0 (ROM = round of measurement) definition.
	void writeV0Data(const TTSTN::TROM& rom);

	/// Write reliability header for ANGL
	void	writeANGLReliabilityHeader();
	/// Write reliability header for ZEND
	void	writeZENDReliabilityHeader();
	/// Write reliability header for DIST
	void	writeDISTReliabilityHeader();
	/// Write reliability header for PLR
	void	writePLRReliabilityHeader();
	/// Write reliability header for DHOR
	void	writeDHORReliabilityHeader();
	/// Write reliability header for ECTH
	void	writeECTHReliabilityHeader();
	/// Write reliability header for ECSP
	void	writeECSPReliabilityHeader();

	/// Write reliability data for ANGL
	void	writeANGLReliabilityData(const TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TANGL>& measANGL);
	/// Write reliability data for ZEND
	void	writeZENDReliabilityData(const TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TZEND>& measZEND);
	/// Write reliability data for DIST
	void	writeDISTReliabilityData(const TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TLINE>& measDIST);
	/// Write reliability data for PLR
	void	writePLRReliabilityData(const TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TPLR3D>& measPLR3D);
	/// Write reliability data for DHOR
	void	writeDHORReliabilityData(const TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TLINE>& measDHOR);
	/// Write reliability data for ECTH
	void	writeECTHReliabilityData(const TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TECTH>& measECTH);
	/// Write reliability data for ECSP
	void	writeECSPReliabilityData(const TTSTN& tstn, const TLGCStatistic& stat, const std::vector<TECSP>& measECSP);



private:
	/*!@name Headers */
	//@{
		void writeANGLResultsHeader(int nOObs);
		void writeZENDResultsHeader(int nOObs);
		void writeDISTResultsHeader(int nOObs);
		void writePLRResultsHeader(int nOObs);
		void writeDHORResultsHeader(int nOObs);
		void writeECTHResultsHeader(int nOObs);
		void writeECSPResultsHeader(int nOObs);
	//@}

	/*!@name Results */
	//@{
		void writeANGLResults(const std::vector<TANGL>& measANGL, const TAdjustablePoint* instrPos, const TAngle& V);
		void writeZENDResults(const std::vector<TZEND>& measZEND, const TAdjustablePoint* instrPos);
		void writeDISTResults(const std::vector<TLINE>& measDIST, const TInstrumentData::TPOLAR& instr, const TAdjustablePoint* instrPos);
		void writePLRResults(const std::vector<TPLR3D>& measPLR3D, const TInstrumentData::TPOLAR& instr , const TAdjustablePoint* instrPos, const TAngle& V);
		void writeDHORResults(const std::vector<TLINE>& measDHOR);
		void writeECTHResults(const std::vector<TECTH>& measECTH, const TAdjustablePoint* instrPos);
		void writeECSPResults(const std::vector<TECSP>& measECSP, const TAdjustablePoint* instrPos);
	//@}

};



#endif //SU_TSTN_WRITER