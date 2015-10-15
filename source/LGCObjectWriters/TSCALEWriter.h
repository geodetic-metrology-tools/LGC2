#ifndef SU_SCALE_WRITER
#define SU_SCALE_WRITER

#include "TObservationWriter.h"
#include "MeasDef.h"
struct TECHOROM;
class TLGCStatistic;

/*!
	\ingroup LGCObjectWriters
@{*/
class  TSCALEWriter : public TObservationWriter 
{
public:
	/// Constructor
	TSCALEWriter(TAStreamFormatter& stream);
	///Destructor
	virtual ~TSCALEWriter();


	

	/*!@name Headers */
	//@{
		/// Write the result header for ECHO 
		void writeECHOResultsHeader(int nOObs);
		/// Write the result header for ECSP 
		void writeECSPResultsHeader();
		/// Write the result header for ECVE 
		void writeECVEResultsHeader();

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
		/// Write the simulated result data for ECHO 
		void writeECHOSIMUResults(const  TECHOROM& echorom);
		/// Write the result data for ECSP 
		void writeECSPResults();
		/// Write the result data for ECVE 
		void writeECVEResults();

		/// Write reliability data for ECHO 
		void	writeECHOReliabilityData(const  TECHOROM& echorom, const TLGCStatistic& stat, const std::vector<TECHO> measECHO);
		/// Write reliability data for ECVE
		void	writeECVEReliabilityData();
		/// Write reliability data for ECSP
		void	writeECSPReliabilityData();
	//@}

};



#endif //SU_SCALE_WRITER