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
		/// Write OBSXYZ results header
		void writeOBSXYZResultsHeader();

		/// Write DVER reliability header  
		void writeDVERReliabilityHeader();
		/// Write ORIE reliability header
		void writeORIEReliabilityHeader();
		/// Write RADI reliability header
		void writeRADIReliabilityHeader();
		/// Write OBSXYZ reliability header
		void writeOBSXYZReliabilityHeader();

		/// Write synthesis results header
		void writeResultsSynthesisHeader();
	//@}

	/*!@name Results */
	//@{
		/// Write PDOR results data
		void writePDORResults(const TPdorObs& fPDOR);
		/// Write DVER results data
		void writeDVERResults(const std::list<TDVER>& fDVER);
		/// Write ORIE results data
		void writeORIEResults(const std::list<TORIE>& fORIE, const LGCAdjustablePoint& instPos);
		/// Write RADI results data
		void writeRADIResults(const std::list<TRADI>& fRADI);
		/// Write OBSXYZ results data
		void writeOBSXYZResults(const std::list<TOBSXYZ>& fOBSXYZ);

		/// Write DVER simulations
        void writeDVERSIMUResults(const std::list<TDVER>& fDVER);
		/// Write RADI simulations
        void writeRADISIMUResults(const std::list<TRADI>& fRADI);
        /// Write ORIE simulations
		void writeORIESIMUResults(const TORIEROM& fOrieRom);

		/// Write DVER reliability data
        void writeDVERReliabilityData(const std::list<TDVER>& fDVER, const TLGCStatistic& stat);
		/// Write ORIE reliability data
        void writeORIEReliabilityData(const std::list<TORIE>& fORIE, const TLGCStatistic& stat, const LGCAdjustablePoint& instPos);
		/// Write RADI reliability data
        void writeRADIReliabilityData(const std::list<TRADI>& fRADI, const TLGCStatistic& stat);
		/// Write OBSXYZ reliability data
		void writeOBSXYZReliabilityData(const std::list<TOBSXYZ>& fOBSXYZ, const TLGCStatistic& stat);

		/// Write DVER synthesis data
		void writeDVERResultsSynthesis(const std::list<TDVER>& fDVER);
		/// Write ORIE synthesis data
		void writeORIEResultsSynthesis(const std::list<TORIE>& fORIE, const LGCAdjustablePoint& instPos);
		/// Write RADI synthesis data
		void writeRADIResultsSynthesis(const std::list<TRADI>& fRADI);
		/// Write OBSXYZ synthesis data
		void writeOBSXYZResultsSynthesis(const std::list<TOBSXYZ>& fOBSXYZ);
	//@}

};

#endif //SU_OTHERMEAS_WRITER