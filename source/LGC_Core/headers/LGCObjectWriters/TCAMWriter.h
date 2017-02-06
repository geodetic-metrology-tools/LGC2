#ifndef SU_CAM_WRITER
#define SU_CAM_WRITER

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

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

	///	Writes information about CAM, its definition, calculated values and all the observations included.
	void writeCAMResults(const TCAM& camera);

	///	Writes information about simulated CAM.
	void writeCAMResultsSIMU(const TCAM& camera);

	/// Writes CAM definition header.
	void writeCAMHeader(const TCAM& camera);

	/// Writes CAM data, defined in the header.
	void writeCAMData(const TCAM& camera);

	/// Writes UVEC measurement reliability header
	void	writeUVECReliabilityHeader();
	/// Writes UVD measurement reliability header
	void	writeUVDReliabilityHeader();
	/// Writes UVEC measurement reliability data
	void	writeUVECReliabilityData(const TCAM& fCam, const TLGCStatistic& stat);
	/// Writes UVD measurement reliability data
	void	writeUVDReliabilityData(const TCAM& fCam, const TLGCStatistic& stat);

private:
	/*!@name Write headers */
	//@{
		void writeUVDResultsHeader(int nOObs);
		void writeUVECResultsHeader(int nOObs);
	//@}

	/*!@name Write results */
	//@{
		void writeUVECResults(const std::vector<TUVEC>& measUVEC);
		void writeUVDResults(const std::vector<TUVD>& measUVD);
	//@}

		bool writeHist;

	
};

#endif //SU_CAM_WRITER