#ifndef SU_TSTN_WRITER
#define SU_TSTN_WRITER

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

//SURVEYLIB
#include <TAngle.h>
//LGC
#include <TTSTN.h>
#include <TObservationWriter.h>


/*!
	\ingroup LGCObjectWriters
	\brief Write TSTN  definition and its observations to an LGC output file.
@{*/
class  TTSTNWriter : public TObservationWriter
{
public:
	/// Constructor
	TTSTNWriter(TAStreamFormatter& stream, bool hist);
	///Destructor
	virtual ~TTSTNWriter();

	///	Writes information about TSTN, its definition, calculated values and each ROM (round of measurements) as well as the observations included.
	void writeTSTNResults(shared_ptr<TTSTN> tstn);

	///	Writes information about TSTN, its definition, calculated values and each ROM (round of measurements) as well as the observations included.
	void writeTSTNResultsSIMU(shared_ptr<TTSTN> tstn);

	/// Writes TSTN header.
	void writeTSTNHeader(shared_ptr<TTSTN> tstn);
	/// Writes TSTN data, definition, provisional and calculated values.
	void writeTSTNData(shared_ptr<TTSTN> tstn);
	
	/// Writes V0 (ROM = round of measurement) header.
	void writeV0Header();
	/// Writes V0 (ROM = round of measurement) definition.
	void writeV0Data(shared_ptr<TTSTN::TROM> rom);

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
	/// Write reliability header for ECDIR
	void	writeECDIRReliabilityHeader();

	/// Write reliability data for ANGL
	void	writeANGLReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::vector<TANGL>& measANGL);
	/// Write reliability data for ZEND
	void	writeZENDReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::vector<TZEND>& measZEND);
	/// Write reliability data for DIST
	void	writeDISTReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::vector<TLINE>& measDIST);
	/// Write reliability data for PLR
	void	writePLRReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::vector<TPLR3D>& measPLR3D);
	/// Write reliability data for DHOR
	void	writeDHORReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::vector<TLINE>& measDHOR);
	/// Write reliability data for ECTH
	void	writeECTHReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::vector<TECTH>& measECTH);
	/// Write reliability data for ECDIR
	void	writeECDIRReliabilityData(shared_ptr<TTSTN> tstn, const TLGCStatistic& stat, const std::vector<TECDIR>& measECDIR);

	/// Set if ALLFIXED option is used
	void setAllfixed(bool fBool){ isAllfixed = fBool; }

private:
	/*!@name Headers */
	//@{
		void writeANGLResultsHeader(int nOObs);
		void writeZENDResultsHeader(int nOObs);
		void writeDISTResultsHeader(int nOObs);
		void writePLRResultsHeader(int nOObs);
		void writeDHORResultsHeader(int nOObs);
		void writeECTHResultsHeader(int nOObs);
		void writeECDIRResultsHeader(int nOObs);
	//@}

	/*!@name Results */
	//@{
		void writeANGLResults(const std::vector<TANGL>& measANGL, const LGCAdjustablePoint* instrPos, const TAngle& V);
		void writeZENDResults(const std::vector<TZEND>& measZEND, const LGCAdjustablePoint* instrPos);
		void writeDISTResults(const std::vector<TLINE>& measDIST, const TInstrumentData::TPOLAR& instr, const LGCAdjustablePoint* instrPos);
		void writePLRResults(const std::vector<TPLR3D>& measPLR3D, const TInstrumentData::TPOLAR& instr, const LGCAdjustablePoint* instrPos, const TAngle& V, const TAngle& rx, const TAngle& ry);
		void writeDHORResults(const std::vector<TLINE>& measDHOR);
		void writeECTHResults(const std::vector<TECTH>& measECTH, const LGCAdjustablePoint* instrPos);
		void writeECDIRResults(const std::vector<TECDIR>& measECDIR, const LGCAdjustablePoint* instrPos);
	//@}

		bool isAllfixed;
		bool writeHist;

};



#endif //SU_TSTN_WRITER