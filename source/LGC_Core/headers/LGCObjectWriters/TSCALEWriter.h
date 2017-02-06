#ifndef SU_SCALE_WRITER
#define SU_SCALE_WRITER

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

//LGC
#include <TObservationWriter.h>
#include <MeasDef.h>

struct TECHOROM;
struct TECVEROM;
struct TECSPROM;
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
		/// Write reliability header for ECSP 
		void	writeECSPReliabilityHeader();
	//@}

	/*!@name Results */
	//@{
		/// Write the result data for ECHO 
		void writeECHOResults(const  TECHOROM& echorom);
		/// Write the result data for ECVE 
		void writeECVEResults(const TECVEROM& ecverom);
		/// Write the result data for ECSP 
		void writeECSPResults(const TECSPROM& ecsprom);
		/// Write the simulated result data for ECHO 
		void writeECHOSIMUResults(const  TECHOROM& echorom);
		/// Write the simulated result data for ECVE 
		void writeECVESIMUResults(const  TECVEROM& ecverom);
		/// Write the simulated result data for ECSP 
		void writeECSPSIMUResults(const  TECSPROM& ecsprom);

		/// Write reliability data for ECHO 
		void	writeECHOReliabilityData(const  TECHOROM& echorom, const TLGCStatistic& stat, const std::vector<TECHO> measECHO);
		/// Write reliability data for ECVE
		void	writeECVEReliabilityData(const TECVEROM& ecverom, const TLGCStatistic& stat, const std::vector<TECVE> measECVE);
		/// Write reliability data for ECSP
		void	writeECSPReliabilityData(const TECSPROM& ecsprom, const TLGCStatistic& stat, const std::vector<TECSP> measECSP);
	//@}

private:
	/// Write the result header for ECHO ECSP and ECVE
	void writeSCALEResultsHeader();

	bool writeHist;

};



#endif //SU_SCALE_WRITER