#pragma once
/*
 Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_WPSR_WRITER
#	define SU_WPSR_WRITER

// LGC
#	include <MeasDef.h>
#	include <TObservationWriter.h>

struct TECWIROM;
class TLGCStatistic;

/*!
	\ingroup LGCObjectWriters
	\brief Write WPSR  definition and its observations to an LGC output file.
@{*/
class TWPSRWriter : public TObservationWriter
{
public:
	/// Constructor
	TWPSRWriter(TAStreamFormatter &stream, bool hist);
	/// Destructor
	virtual ~TWPSRWriter();

	/*!@name Headers */
	//@{
	//	/// Write reliability header for ECWI
	void writeECWIReliabilityHeader();

	///// Write the result synthesis header
	void writeWPSRSynthesisHeader();
	//@}

	/*!@name Results */
	//@{
	/// Write the result data for ECWI
	void writeECWIResults(const TECWIROM &ecwirom);

	/// Write the simulated result data for ECWI
	void writeECWISIMUResults(const TECWIROM &ecwirom);

	///// Write reliability data for ECWI
	void writeECWIReliabilityData(const TECWIROM &ecwirom, const TLGCStatistic &stat, const std::list<TECWI> &measECWI);

	///// Write the result synthesis for ECWI
	void writeECWIResultsSynthesis(const TECWIROM &ecwirom);

	////Write Default results data
	void writeDefResultsSynthesis(std::list<const TLGCObsSummary *> &meassum, int obsResWidth, int ResPrecision);

private:
	/// Write information about the instrument
	void writeWPSRHeader(const TECWIROM &ecwirom);

	/// Write the result header for WPSR
	void writeWPSRResultsHeader();
};

#endif // SU_WPSR_WRITER
