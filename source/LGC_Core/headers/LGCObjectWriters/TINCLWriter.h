/*
 * SPDX-FileCopyrightText: 2025 CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SU_INCL_WRITER
#define SU_INCL_WRITER


//LGC
#include <TObservationWriter.h>
#include <MeasDef.h>
#include <TLGCData.h>

struct TINCLYROM;
struct TROLLYROM;
class TLGCStatistic;

/*!
	\ingroup LGCObjectWriters
	\brief Write SCALE  definition and its observations to an LGC output file.
@{*/
class  TINCLWriter : public TObservationWriter
{
public:
	/// Constructor
	TINCLWriter(TAStreamFormatter& stream, bool hist);
	///Destructor
	virtual ~TINCLWriter() override;

	/*!@name Headers */
	//@{
		/// Write reliability header for INCL measurements (common for both INCLY and ROLLY)
	void writeINCLReliabilityHeader();

	/// Write the result synthesis header
	void writeINCLSynthesisHeader();
	//@}

	/*!@name Results */
	//@{
		/// Write the result data for INCLY 
	void writeINCLYResults(const  TINCLYROM& inclyrom);

	/// Write the result data for ROLLY 
	void writeROLLYResults(const  TROLLYROM& rollyrom);

private:
	/// Common template helper for both INCLY and ROLLY results output
	template<typename MeasurementList>
	void writeINCLResultsHelper(const MeasurementList& measurements, const char* sectionTitle, TALGCObjectWriter::ELGCObservations obsType);
	
	/// Common template helper for both INCLY and ROLLY reliability data output
	template<typename ROMType, typename MeasurementList>
	void writeINCLReliabilityDataHelper(const ROMType& rom, const TLGCStatistic& stat, const MeasurementList& measurements);
	
	/// Common helper for both INCLY and ROLLY results synthesis output
	void writeINCLResultsSynthesisHelper(std::list<const TLGCObsSummary *> &summaries);

public:

	/// Write the simulated result data for INCLY 
	void writeINCLYSIMUResults(const  TINCLYROM& inclyrom);

	/// Write the simulated result data for ROLLY 
	void writeROLLYSIMUResults(const  TROLLYROM& rollyrom);

	/// Write reliability data for INCLY
	void writeINCLYReliabilityData(const TINCLYROM &inclyrom, const TLGCStatistic &stat, const std::list<TINCLY> &measINCLY);

	/// Write reliability data for ROLLY
	void writeROLLYReliabilityData(const TROLLYROM &rollyrom, const TLGCStatistic &stat, const std::list<TROLLY> &measROLLY);

	/// Write the result data for INCLY in all frames 
	void writeINCLYResultsSynthesis(std::list<const TLGCObsSummary*> &inclysum);

	/// Write the result data for ROLLY in all frames 
	void writeROLLYResultsSynthesis(std::list<const TLGCObsSummary*> &rollysum);
	//@}

private:
	/// Write the result header for INCLY
	void writeINCLResultsHeader();

	/// Project data
	const TLGCData* fProjectData;
};



#endif //SU_INCL_WRITER