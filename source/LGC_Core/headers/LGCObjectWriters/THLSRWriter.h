/*
 Copyright CERN 2000-2022. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_HLSR_WRITER
#define SU_HLSR_WRITER


//LGC
#include <TObservationWriter.h>
#include <MeasDef.h>
//#include <TLGCData.h>

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

	///// Write the result synthesis for ECWS
	void writeECWSRResultsSynthesis(const  TECWSROM& ecwsrom);

	////Write Default results data
	void writeDefResultsSynthesis(std::list<const TLGCObsSummary*>& meassum, int obsResWidth, int ResPrecision);

private:
	/// Write information about the instrument
	void writeHLSRHeader(const  TECWSROM& ecwsrom);

	/// Write the result header for HLSR
	void writeHLSRResultsHeader();
};



#endif //SU_HLSR_WRITER

