#ifndef SU_TOBS_WRITER
#define SU_TOBS_WRITER

#include <list>
#include <TSpatialPointName.h>
#include "TALGCObjectWriter.h"
#include "TLGCStatistic.h"

class TLGCData;
class TLGCObsSummary;
////////////////////////////////////////////////////////////////

/*!
	\ingroup LGCObjectWriters
	Base Class for writing observations for an output LGC file.
@{*/
class  TObservationWriter : public TALGCObjectWriter
{

public:

	/*!@name Constructors and Destructors*/
	//@{
		/*! 
		\brief Constructor

		\param TAStreamFormatter* stream used to write the data */
		TObservationWriter(TAStreamFormatter& stream);

		/// Destructor 
		virtual  ~TObservationWriter();
	//@}

		/// get the descriptive text in French for an observation type
	    static string	getObsDescriptionFR(TALGCObjectWriter::ELGCObservations key);
						
		/// get the descriptive text in English for an observation type
		static string	getObsDescriptionEN(TALGCObjectWriter::ELGCObservations key);

		/// write a title of an observation, including description and number of obs.
		void writeObsTitle(const string& description, const int numObs);

		/// write summary of statistics for a group of angle measurements (RESIDU MOYEN and LIMITES DE CONFIANC), (French version)
		void writeAngleResultsSummary(TLGCObsSummary summary, std::string TABs);
		/// write summary of statistics for a group of distance measurements (RESIDU MOYEN and LIMITES DE CONFIANC), (French version)
		void writeDistanceResultsSummary(TLGCObsSummary summary, std::string TABs);
		/// write summary of statistics for a group of distance measurements (RESIDU MOYEN and LIMITES DE CONFIANC), (French version)
		void writeUnitlessResultsSummary(TLGCObsSummary summary, std::string TABs);
protected:


	/*!@name Protected member functions*/
	//@{
		/// write reliability header
		void	writeReliabilityHeader(	string name1,
										string name2,
										string name3,
										string Observation,
										string unit1,
										string unit2);
		
		/// write reliability 
		void writeReliability(int index, const TLGCStatistic& stat);

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