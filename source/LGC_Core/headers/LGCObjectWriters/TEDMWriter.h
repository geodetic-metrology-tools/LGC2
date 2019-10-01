
/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_TEDM_WRITER
#define SU_TEDM_WRITER
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//LGC
#include <TObservationWriter.h>
#include <TLGCData.h>

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

	/// Write the result synthesis header
	void writeEDMSynthesisHeader();
	/// Write the result synthesis for DSPT 
	void writeDSPTResultsSynthesis(const  TEDM& fEdm);

	/// Set if ALLFIXED option is used
	void setAllfixed(bool fBool){ isAllfixed = fBool; }

    /// Get the sigma for the distance measurement
    static TReal getDistanceSigmaInMM(TDSPT const * const meas){
        // Return value in millimeters [mm]
        return (meas->target.sigmaDSpt + meas->target.ppmDSpt * meas->getDistance() / 1000) * M2MM;
    }
	
private:
	/// Write information about the instrument
	void writeEDMHeader(const TEDM& fEdm);

	/// Write measurements
	void writeDSPTResultsData(const std::list<TDSPT> measDSPT,const TInstrumentData::TEDM& instr , const LGCAdjustablePoint* instrPos);
	void writeDSPTResultsHeader(const int);

	bool isAllfixed;
};

#endif //SU_TEDM_WRITER