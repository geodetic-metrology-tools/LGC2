#ifndef SU_LEVEL_WRITER
#define SU_LEVEL_WRITER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLGCData.h"
#include "TObservationWriter.h"
////////////////////////////////////////////////////////////////

/*!
	\ingroup LGCObjectWriters
@{*/
class  TLEVELWriter : public TObservationWriter 
{
public:

	/// Constructor
	TLEVELWriter(TAStreamFormatter& stream, bool hist);
	/// Destructor
	virtual ~TLEVELWriter();
	
	/// Write results
	void writeLEVELResults(const TLEVEL& fLevel);
	/// Write simulated results
	void writeLEVELSIMUResults(const TLEVEL& fLevel);

	/// Write reliability header
	void writeReliabilityHeader();

	/// Write reliability data for DHOR (from DLEV)
	void writeDHORReliabilityData(const TLEVEL& fLevel, const TLGCStatistic& stat );
	/// Write reliability data for DLEV
	void writeDLEVReliabilityData(const TLEVEL& fLevel, const TLGCStatistic& stat);

	/// Set if ALLFIXED option is used
	void setAllfixed(bool fBool){ isAllfixed = fBool; }
	
private:
	// Information about the instrument/ station
	void writeLEVELHeader(const TLEVEL& fLevel);
	void writeLEVELData(const TLEVEL& fLevel);

	// DLEV measurements
	void writeDLEVResults(std::vector<TDLEV> measDLEV, const TInstrumentData::TLEVEL& instr);
	void writeDLEVResultsHeader(int nOObs);

	bool isAllfixed;
	bool writeHist;
};

#endif //SU_LEVEL_WRITER