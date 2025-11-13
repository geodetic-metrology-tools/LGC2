/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SU_HLSR_WRITER
#define SU_HLSR_WRITER


//LGC
#include <TObservationWriter.h>
#include <MeasDef.h>

struct TECWSROM;
class TLGCStatistic;

/*!
	\ingroup LGCObjectWriters
	\brief Write HLSR  definition and its observations to an LGC output file.
@{*/
class  THLSRWriter : public TObservationWriter
{
public:
	/// Constructor
	THLSRWriter(TAStreamFormatter& stream, bool hist);
	///Destructor
	virtual ~THLSRWriter();

	/*!@name Headers */
	//@{
	//	/// Write reliability header for ECWS 
	void writeECWSReliabilityHeader();

	///// Write the result synthesis header
	void writeHLSRSynthesisHeader();
	//@}

	/*!@name Results */
	//@{
		/// Write the result data for ECWS 
	void writeECWSResults(const  TECWSROM& ecwsrom);

	/// Write the simulated result data for ECWS 
	void writeECWSSIMUResults(const  TECWSROM& ecwsrom);

	///// Write reliability data for ECWS 
	void  writeECWSReliabilityData(const  TECWSROM& ecwsrom, const TLGCStatistic& stat, const std::list<TECWS>& measECWS);

	////Write Default results data
	void writeDefResultsSynthesis(std::list<const TLGCObsSummary*>& meassum, int obsResWidth, int ResPrecision);

private:
	/// Write information about the instrument
	void writeHLSRHeader(const  TECWSROM& ecwsrom);

	/// Write the result header for HLSR
	void writeHLSRResultsHeader();
};



#endif //SU_HLSR_WRITER

