#ifndef SU_OTHERMEAS_WRITER
#define SU_OTHERMEAS_WRITER

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TObservationWriter.h"
#include "MeasDef.h"
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

	/// Write reliability header for DVER
	void	writeDVERReliabilityHeader();

	/// Write reliability data for DVER
	void	writeDVERReliabilityData(const std::vector<TDVER>& fDVER, const TLGCStatistic& stat);

	/*!@name Headers */
	//@{
		/// Write DVER results header
		void writeDVERResultsHeader(int nOObs);
	//@}

	/*!@name Results */
	//@{
		/// Write DVER results data
		void writeDVERResults(const std::vector<TDVER>& fDVER);
	//@}

};



#endif //SU_OTHERMEAS_WRITER