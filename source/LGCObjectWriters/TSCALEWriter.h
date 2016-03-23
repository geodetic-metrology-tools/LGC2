#ifndef SU_SCALE_WRITER
#define SU_SCALE_WRITER

#include "TObservationWriter.h"
#include "MeasDef.h"
struct TECHOROM;
struct TECVEROM;
class TLGCStatistic;

/*!
	\ingroup LGCObjectWriters
	\brief Write SCALE  definition and its observations to an LGC output file.
@{*/
class  TSCALEWriter : public TObservationWriter 
{
public:
	/// Constructor
	TSCALEWriter(TAStreamFormatter& stream, bool hist);
	///Destructor
	virtual ~TSCALEWriter();

	/*!@name Headers */
	//@{
		/// Write reliability header for ECHO 
		void	writeECHOReliabilityHeader();
		/// Write reliability header for ECVE 
		void	writeECVEReliabilityHeader();
	//@}

	/*!@name Results */
	//@{
		/// Write the result data for ECHO 
		void writeECHOResults(const  TECHOROM& echorom);
		/// Write the result data for ECVE 
		void writeECVEResults(const TECVEROM& ecverom);
		/// Write the simulated result data for ECHO 
		void writeECHOSIMUResults(const  TECHOROM& echorom);
		/// Write the simulated result data for ECVE 
		void writeECVESIMUResults(const  TECVEROM& ecverom);

		/// Write reliability data for ECHO 
		void	writeECHOReliabilityData(const  TECHOROM& echorom, const TLGCStatistic& stat, const std::vector<TECHO> measECHO);
		/// Write reliability data for ECVE
		void	writeECVEReliabilityData(const TECVEROM& ecverom, const TLGCStatistic& stat, const std::vector<TECVE> measECVE);
	//@}

private:
	/// Write the result header for ECHO ECSP and ECVE
	void writeSCALEResultsHeader();

	bool writeHist;

};



#endif //SU_SCALE_WRITER