
#ifndef SU_TEDM_WRITER
#define SU_TEDM_WRITER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TObservationWriter.h"
#include "TLGCData.h"
////////////////////////////////////////////////////////////////

/*!
	\ingroup LGCObjectWriters
	\brief Write the TSTN instrument for an LGC output file

@{*/
class  TEDMWriter : public TObservationWriter
{
public:
	/// Constructor
	TEDMWriter(TAStreamFormatter& stream, bool hist);
	///Destructor
	virtual ~TEDMWriter();

	///	Writes all information about EDM, its definition, calculated parameters and observations which includes.
	void writeEDMResults(const TEDM& fEdm);
	///	Writes all information about simulated EDM, its definition, calculated parameters and observations which includes.
	void writeEDMSIMUResults(const TEDM& fEdm);
	
	/// Write reliabilty data
	void	writeReliabilityData(const TEDM& fEdm, const TLGCStatistic& stat);
	/// Write reliabilty header
	void	writeReliabilityHeader();

	/// Set if ALLFIXED option is used
	void setAllfixed(bool fBool){ isAllfixed = fBool; }
	
private:
	/// Write information about the instrument
	void writeEDMHeader(const TEDM& fEdm);
	void writeEDMData(const TEDM& fEdm);

	/// Write measurements
	void writeDSPTResultsData(const std::vector<TDSPT> measDSPT,const TInstrumentData::TEDM& instr , const LGCAdjustablePoint* instrPos);
	void writeDSPTResultsHeader(const int);

	bool isAllfixed;
	bool writeHist;
};

#endif //SU_TEDM_WRITER