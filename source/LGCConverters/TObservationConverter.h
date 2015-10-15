////////////////////////////////////////////////////////////////////
// TObservationConverter
/*!
Base Class for converters writing Observations for an output LGC file.

Copyright 2003-2015 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////
#ifndef SU_TOBS_CONVERTER
#define SU_TOBS_CONVERTER

////////////////////////////////////////////////////////////////
// Forward declarations
#include <list>
#include <TSpatialPointName.h>
#include "TALGCConverter.h"

class TLGCObsSummary;
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/
//Class definition
class  TObservationConverter : public TALGCConverter
{

public:

	/*!@name Constructors and Destructors*/
	//@{
		/*! Constructor
		\param TAStreamFormatter* stream used to convert data */
		TObservationConverter(TAStreamFormatter& stream);

		/*! Destructor */
		virtual  ~TObservationConverter();
	//@}

		//! get the descriptive text in French for an observation type
	    static string	getObsDescriptionFR(TALGCConverter::ELGCObservations key);
						
		//! get the descriptive text in English for an observation type
		static string	getObsDescriptionEN(TALGCConverter::ELGCObservations key);

		//! write a title of an observation, including description and number of obs.
		void writeObsTitle(const string& description, const int numObs);

		//! write summary of statistics for a group of angle measurements (RESIDU MOYEN and LIMITES DE CONFIANC), (French version)
		void writeAngleResultsSummary(TLGCObsSummary summary, std::string TABs);
		//! write summary of statistics for a group of distance measurements (RESIDU MOYEN and LIMITES DE CONFIANC), (French version)
		void writeDistanceResultsSummary(TLGCObsSummary summary, std::string TABs);
protected:


	/*!@name Protected member functions*/
	//@{
		/*!write reliability header*/
		void	writeReliabilityHeader(	string name1,
										string name2,
										string name3,
										string Observation,
										string unit1,
										string unit2);

	//@}

			
		//void				setComments(TAMeasurement* meas, string headCommentLine, string dbCom);
		//void				setId(TAMeasurement* meas, int ID, int& numOfMeas);

		//void	 			readDBComments(	char lineNum[9],
		//									string& iLine,
		//									string& line,
		//									int& i,
		//									int& id,
		//									string& comdb,
		//									string& EOLComments,
		//									TLGCProject* projet);

		void				insertKeyChar(int keyChar) {fKeyCharList.push_back(keyChar); return;}

		list<int>::iterator			getKeyCharListBegin()  {return fKeyCharList.begin();}
		list<int>::iterator			getKeyCharListEnd()  {return fKeyCharList.end();}

private:

	/*!@name Private Function*/
	//@{
		/*! default Constructor*/
		TObservationConverter();
	//@}
	
	/*!@name Private Attribute*/
	//@{
		list<int>		fKeyCharList;
	//@}
};

/*@}*/

#endif // SU_TOBS_CONVERTER