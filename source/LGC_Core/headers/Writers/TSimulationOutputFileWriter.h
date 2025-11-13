/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SU_RESSIM_WRITER
#define SU_RESSIM_WRITER

// LGC
#include <TLSCalcRelativeError.h>
#include <TResultsFileWriter.h>
#include <TSimFrameSummary.h>
#include <TSimPointSummary.h>

class TLGCData;

/*!
	\ingroup Writers
	\brief Write standard LGC output file if the simulation (SIMU) option is enabled.

	Provides functions to write different parts of the simulation file.
*/
struct ERELPointStat
{
	std::vector<TLSCalcRelativeErrorPoint> MinErel;
	std::vector<TLSCalcRelativeErrorPoint> MaxErel;
	std::vector<TLSCalcRelativeErrorPoint> MeanErel;
};

class TSimulationOutputFileWriter : public TResultsFileWriter
{
public:
	/*!@name Constructors and Destructors*/
	//@{
	/// constructor
	TSimulationOutputFileWriter(TAStreamFormatter *stream, const TLGCData *project);

	/// Destructor
	virtual ~TSimulationOutputFileWriter();
	//@}

	/*!@name Public member functions*/
	//@{
	///  Write the beginning of the LGC simulation file: the header, calculation summary and similar.
	virtual void writeFileBegin();

	/// Write a simulation summary for one specific simulation.
	virtual void writeSimSummary(TLGCData &project, int numberOfSim);

	/// Write a full observation description and summary for the last iteration.
	virtual void writeLastSimResult(TLGCData &project, int numberOfSim);

	/// Write a final TABLE which summarises POINT statistics.
	virtual void writeSimPointsSummary(const std::string &projTitle, const std::list<TSimPointSummary> &dataSum, int numbOfSimu);

	/// Write a final TABLE which summarises FRAME statistics.
	virtual void writeSimFramesSummary(const std::list<TSimFrameSummary> &dataSum, int numbOfSimu);

	///  Write the relative errors header
	void writeRelErrorHeader();
	/// Write the relative errors results
	void writeRelErrorResults(const TLGCData &);
	///  Write the relative errors frames header
	void writeRelErrorFrameHeader();
	/// Write the relative errors frames results
	void writeRelErrorFrameResults(const TLGCData &);
	//@}

private:
	/*!default constructor*/
	TSimulationOutputFileWriter();

	ERELPointStat calculateStatForERELPoint(std::vector<TLSCalcRelativeErrorPoint> ERELdata);

	/*!@name Private member functions*/
	//@{
	/*!
		\brief Write simulation calculation header

		\param[in] i simulation number
	*/
	void writeSimHeader(TLGCData &calcRec, int i);

	/*! \brief Write simulation calculation header

		\param[in] objectType  e.g. Point, DX, DY, DZ, SX, SY, SZ
	*/
	void writeSimTableDescription(const std::string &projTitle, const std::string &objectType, int i);

	/*! \brief Write simulation calculation header

		\param[in] objectType  e.g. frame, DTX, DTY, DTZ, DRX, DRY, DRZ, DSCALE, STX, STY, STZ...
	*/
	void writeSimFRAMEDescription();

	/// Write point data from simualtions
	void writeSimPointData(const TSimPointSummary &simPt, const int i);

	/// Write frame data from simualtions
	void writeSimFRAMEData(const TSimFrameSummary &simFr, const int i);

	//@}
};

#endif
