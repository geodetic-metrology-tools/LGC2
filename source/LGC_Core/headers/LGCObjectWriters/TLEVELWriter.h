/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_LEVEL_WRITER
#define SU_LEVEL_WRITER
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//LGC
#include <TLGCData.h>
#include <TObservationWriter.h>

/*!
	\ingroup LGCObjectWriters
	\brief Write LEVEL  definition and its observations to an LGC output file.
@{*/
class  TLEVELWriter : public TObservationWriter
{
public:

	/// Constructor
	TLEVELWriter(TAStreamFormatter& stream, bool hist);
	/// Destructor
	virtual ~TLEVELWriter();

	/*!@name Headers */
	//@{
		/// Write reliability header
		void writeReliabilityHeader();

		/// Write the result synthesis header
		void writeLEVELSynthesisHeader();
	//@}

	/*!@name Results */
	//@{
		/// Write results
		void writeLEVELResults(const TLEVEL& fLevel);
		/// Write simulated results
		void writeLEVELSIMUResults(const TLEVEL& fLevel);

		/// Write reliability data for DHOR (from DLEV)
		void writeDHORReliabilityData(const TLEVEL& fLevel, const TLGCStatistic& stat);
		/// Write reliability data for DLEV
		void writeDLEVReliabilityData(const TLEVEL& fLevel, const TLGCStatistic& stat);

		/// Write results synthesis
		void writeLEVELResultsSynthesis(const TLEVEL& fLevel);
	//@}

	/// Set if ALLFIXED option is used
	void setAllfixed(bool fBool){ isAllfixed = fBool; }
	
private:
	// Information about the instrument/ station
	void writeLEVELHeader(const TLEVEL& fLevel);

	// DLEV measurements
	void writeDLEVResults(std::list<TDLEV> measDLEV, const TInstrumentData::TLEVEL& instr);
	void writeDLEVResultsHeader(int nOObs);

	bool isAllfixed;
};

#endif //SU_LEVEL_WRITER