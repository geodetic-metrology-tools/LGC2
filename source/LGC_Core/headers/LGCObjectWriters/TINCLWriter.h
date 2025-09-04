/*
Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
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
		/// Write reliability header for INCLY 
	void	writeINCLYReliabilityHeader();

		/// Write reliability header for ROLLY 
	void	writeROLLYReliabilityHeader();

	/// Write the result synthesis header
	void writeINCLSynthesisHeader();
	//@}

	/*!@name Results */
	//@{
		/// Write the result data for INCLY 
	void writeINCLYResults(const  TINCLYROM& inclyrom);

	/// Write the result data for ROLLY 
	void writeROLLYResults(const  TROLLYROM& rollyrom);

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