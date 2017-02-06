#ifndef SU_OTHERMEAS_WRITER
#define SU_OTHERMEAS_WRITER

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

//LGC
#include <TObservationWriter.h>
#include <MeasDef.h>
#include <RoundOfMeasurements.h>

/*!
	\ingroup LGCObjectWriters
	\brief Write DVER, ORIE, RADI and PDOR observations to an LGC output file.
@{*/
class  TOtherMeasurentWriter : public TObservationWriter
{
public:
	/// Constructor
	TOtherMeasurentWriter(TAStreamFormatter& stream, bool hist);
	/// Destructor
	virtual ~TOtherMeasurentWriter();

	/// Write DVER reliability header  
	void	writeDVERReliabilityHeader();
	/// Write ORIE reliability header
	void	writeORIEReliabilityHeader();
	/// Write RADI reliability header
	void	writeRADIReliabilityHeader();

	/// Write DVER reliability data
	void	writeDVERReliabilityData(const std::vector<TDVER>& fDVER, const TLGCStatistic& stat);
	/// Write ORIE reliability data
	void	writeORIEReliabilityData(const std::vector<TORIE>& fORIE, const TLGCStatistic& stat, const LGCAdjustablePoint& instPos);
	/// Write RADI reliability data
	void	writeRADIReliabilityData(const std::vector<TRADI>& fRADI, const TLGCStatistic& stat);

	/*!@name Headers */
	//@{
		/// Write DVER results header
		void writeDVERResultsHeader();
		/// Write ORIE results header
		void writeORIEResultsHeader();
		/// Write RADI results header
		void writeRADIResultsHeader();
		/// Write PDOR results header
		void writePDORResultsHeader();
	//@}

	/*!@name Results */
	//@{
		/// Write PDOR results data
		void writePDORResults(const TPdorObs& fPDOR);
		/// Write DVER results data
		void writeDVERResults(const std::vector<TDVER>& fDVER);
		/// Write ORIE results data
		void writeORIEResults(const std::vector<TORIE>& fORIE, const LGCAdjustablePoint& instPos);
		/// Write RADI results data
		void writeRADIResults(const std::vector<TRADI>& fRADI);

		/// Write DVER simulations
		void writeDVERSIMUResults(const std::vector<TDVER>& fDVER);
		/// Write ORIE simulations
		void writeORIESIMUResults(const TORIEROM& fOrieRom);
	//@}

		/// flag to write the histogram
		bool writeHist;

};



#endif //SU_OTHERMEAS_WRITER