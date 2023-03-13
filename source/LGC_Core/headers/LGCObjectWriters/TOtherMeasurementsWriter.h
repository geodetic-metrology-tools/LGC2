/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_OTHERMEAS_WRITER
#define SU_OTHERMEAS_WRITER


//LGC
#include <TObservationWriter.h>
#include <MeasDef.h>
#include "TMeasurements.h"

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

		/// Write synthesis results header (CC)
		void writeResultsSynthesisHeaderAngles();
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

		//Write Default results data
		void writeDefResultsSynthesis(std::list<const TLGCObsSummary*> &meassum, int obsResWidth, int ResPrecision);

		/// Write DVER simulations
        void writeDVERSIMUResults(const TMeasurements &tmeas);
		/// Write RADI simulations
        void writeRADISIMUResults(const TMeasurements &tmeas);
        /// Write ORIE simulations
		void writeORIESIMUResults(const TORIEROM& orieRom);

		/// Write DVER reliability data
        void writeDVERReliabilityData(const std::list<TDVER>& fDVER, const TLGCStatistic& stat);
		/// Write ORIE reliability data
        void writeORIEReliabilityData(const std::list<TORIE>& fORIE, const TLGCStatistic& stat, const LGCAdjustablePoint& instPos);
		/// Write RADI reliability data
        void writeRADIReliabilityData(const std::list<TRADI>& fRADI, const TLGCStatistic& stat);
		/// Write OBSXYZ reliability data
		void writeOBSXYZReliabilityData(const std::list<TOBSXYZ>& fOBSXYZ, const TLGCStatistic& stat);
	//@}

};

#endif //SU_OTHERMEAS_WRITER
