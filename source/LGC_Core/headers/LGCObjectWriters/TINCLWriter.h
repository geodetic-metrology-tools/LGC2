/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_INCL_WRITER
#define SU_INCL_WRITER


//LGC
#include <TObservationWriter.h>
#include <MeasDef.h>

struct TINCLYROM;
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
	virtual ~TINCLWriter();

	/*!@name Headers */
	//@{
		/// Write reliability header for INCLY 
	void	writeINCLYReliabilityHeader();

	/// Write the result synthesis header
	void writeINCLSynthesisHeader();
	//@}

	/*!@name Results */
	//@{
		/// Write the result data for INCLY 
	void writeINCLYResults(const  TINCLYROM& inclyrom);

	/// Write the simulated result data for INCLY 
	void writeINCLYSIMUResults(const  TINCLYROM& inclyrom);

	/// Write reliability data for ECHO 
	void	writeINCLYReliabilityData(const  TINCLYROM& inclyrom, const TLGCStatistic& stat, const std::list<TINCLY> measINCLY);

	/// Write the result synthesis for INCLY
	void writeINCLYResultsSynthesis(const  TINCLYROM& inclyrom);
	//@}

private:
	/// Write the result header for INCLY
	void writeINCLResultsHeader();
};



#endif //SU_INCL_WRITER