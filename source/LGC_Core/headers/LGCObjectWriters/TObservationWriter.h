/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SU_TOBS_WRITER
#define SU_TOBS_WRITER

// STL
#include <list>
// SURVEYLIB
#include <TSpatialPointName.h>
// LGC
#include <TALGCObjectWriter.h>
#include <TLGCStatistic.h>

class TLGCData;
class TLGCObsSummary;

/*!
	\ingroup LGCObjectWriters
	Base Class for writing observations for an output LGC file.
@{*/
class TObservationWriter : public TALGCObjectWriter
{
public:
	/*!@name Constructors and Destructors*/
	//@{
	/*!
	\brief Constructor

	\param TAStreamFormatter* stream used to write the data */
	TObservationWriter(TAStreamFormatter &stream);

	/// Destructor
	virtual ~TObservationWriter();
	//@}

	/// get the descriptive text in French for an observation type
	static std::string getObsDescriptionFR(TALGCObjectWriter::ELGCObservations key);

	/// get the descriptive text in English for an observation type
	static std::string getObsDescriptionEN(TALGCObjectWriter::ELGCObservations key);

	/// write a title of an observation, including description and number of obs.
	void writeObsTitle(const std::string &description, const int numObs);

	/// write summary of statistics for a group of angle measurements (RESIDU MOYEN and LIMITES DE CONFIANC), (French version)
	void writeAngleResultsSummary(const TLGCObsSummary &summary, std::string TABs);
	/// write summary of statistics for a group of distance measurements (RESIDU MOYEN and LIMITES DE CONFIANC), (French version)
	void writeDistanceResultsSummary(const TLGCObsSummary &summary, std::string TABs);
	/// write summary of statistics for a group of distance measurements (RESIDU MOYEN and LIMITES DE CONFIANC), (French version)
	void writeUnitlessResultsSummary(const TLGCObsSummary &summary, std::string TABs);

	/// write a histogram for the group of measurements
	void writeHisto(const TLGCObsSummary &summary, std::string description);

protected:
	/*!@name Protected member functions*/
	//@{
	/// write reliability header
	void writeReliabilityHeader(std::string name1, std::string name2, std::string name3, std::string Observation, std::string unit1, std::string unit2);

	/// write reliability
	void writeReliabilityCC(int index, const TLGCStatistic &stat);
	void writeReliabilityMM(int index, const TLGCStatistic &stat);

	//@}
private:
	/*!@name Private Function*/
	//@{
	/*! default Constructor*/
	TObservationWriter();
	//@}
};

/*@}*/

#endif // SU_TOBS_WRITER
