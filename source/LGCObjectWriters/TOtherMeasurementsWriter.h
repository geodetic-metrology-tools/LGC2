#ifndef SU_OTHERMEAS_WRITER
#define SU_OTHERMEAS_WRITER

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TObservationWriter.h"
#include "MeasDef.h"
#include "RoundOfMeasurements.h"
////////////////////////////////////////////////////////////////


/*!
	\ingroup LGCObjectWriters
@{*/
class  TOtherMeasurentWriter : public TObservationWriter
{
public:
	/// Constructor
	TOtherMeasurentWriter(TAStreamFormatter& stream);
	/// Destructor
	virtual ~TOtherMeasurentWriter();

	/// Write reliability header 
	void	writeDVERReliabilityHeader();
	void	writeORIEReliabilityHeader();
	void	writeRADIReliabilityHeader();

	/// Write reliability data
	void	writeDVERReliabilityData(const std::vector<TDVER>& fDVER, const TLGCStatistic& stat);
	void	writeORIEReliabilityData(const std::vector<TORIE>& fORIE, const TLGCStatistic& stat, const TAdjustablePoint& instPos);
	void	writeRADIReliabilityData(const std::vector<TRADI>& fRADI, const TLGCStatistic& stat);

	/*!@name Headers */
	//@{
		/// Write results header
		void writeDVERResultsHeader();
		void writeORIEResultsHeader();
		void writeRADIResultsHeader();
		void writePDORResultsHeader();
	//@}

	/*!@name Results */
	//@{
		/// Write results data
		void writePDORResults(const TPdorObs& fPDOR);
		void writeDVERResults(const std::vector<TDVER>& fDVER);
		void writeORIEResults(const std::vector<TORIE>& fORIE, const TAdjustablePoint& instPos);
		void writeRADIResults(const std::vector<TRADI>& fRADI);

		void writeDVERSIMUResults(const std::vector<TDVER>& fDVER);
		void writeORIESIMUResults(const TORIEROM& fOrieRom);
		void writeRADISIMUResults(const std::vector<TRADI>& fRADI);
	//@}


};



#endif //SU_OTHERMEAS_WRITER