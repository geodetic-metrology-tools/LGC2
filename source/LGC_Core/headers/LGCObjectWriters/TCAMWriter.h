/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_CAM_WRITER
#define SU_CAM_WRITER

//LGC
#include <TLGCData.h>
#include <TObservationWriter.h>

/*!
	\ingroup LGCObjectWriters
	\brief Write the TCAM camera definition and results of its measurements.
@{*/
class  TCAMWriter : public TObservationWriter
{
public:
	/// Constructor
	TCAMWriter(TAStreamFormatter& stream, bool hist);
	/// Destructor
	virtual ~TCAMWriter();

	/*!@name Headers */
	//@{
		/// Writes CAM definition header.
		void writeCAMHeader(const TCAM& camera);

		/// Writes UVEC measurement reliability header
		void	writeUVECReliabilityHeader();
		/// Writes UVD measurement reliability header
		void	writeUVDReliabilityHeader();

		/// Write the UVEC synthesis header
		void writeUVECSynthesisHeader();
		/// Write the UVD synthesis header
		void writeUVDSynthesisHeader();

		/// Write the Unitless synthesis header
		void writeResultsSynthesisHeaderUnitless();
		/// Write the Distance synthesis header
		void writeResultsSynthesisHeaderDistance();
	//@}

	/*!@name Results */
	//@{
		///	Writes information about CAM, its definition, calculated values and all the observations included.
		void writeCAMResults(const TCAM& camera);

		///	Writes information about simulated CAM.
		void writeCAMResultsSIMU(const TCAM& camera);

		/// Writes UVEC measurement reliability data
		void	writeUVECReliabilityData(const TCAM& fCam, const TLGCStatistic& stat);
		/// Writes UVD measurement reliability data
		void	writeUVDReliabilityData(const TCAM& fCam, const TLGCStatistic& stat);

		/// Write Default results synthesis
		void writeDefResultsSynthesis(std::list<const TLGCObsSummary*> &meassum, int obsResWidth, int ResPrecision);
	//@}


private:
	/*!@name Write headers */
	//@{
		void writeUVDResultsHeader(int nOObs);
		void writeUVECResultsHeader(int nOObs);
	//@}

	/*!@name Write results */
	//@{
        void writeUVECResults(const std::list<TUVEC>& measUVEC);
        void writeUVDResults(const std::list<TUVD>& measUVD);
	//@}	
};

#endif //SU_CAM_WRITER