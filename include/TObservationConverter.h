////////////////////////////////////////////////////////////////////
// TObservationConverter
/*!
Base Class for converters writing Observations for an output LGC file

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_TOBS_CONVERTER
#define SU_TOBS_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
class	TAMeasurement;
class	TLGCProject;

#include <list>
#include "TSpatialPointName.h"
#include "TALGCConverter.h"

// typedefs
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
		//void	writeKeyWord();

		void	readPtName(TSpatialPointName &tgName);
	

protected:


	/*!@name Protected member functions*/
	//@{
		/*!write reliability header*/
		void	writeReliabilityHeader(	string name,
										string Observation,
										string unit1,
										string unit2);

	//@}

			
		void				setComments(TAMeasurement* meas, string headCommentLine, string dbCom);
		void				setId(TAMeasurement* meas, int ID);

		void	 			readDBComments(	char lineNum[9],
											string& iLine,
											string& line,
											int& i,
											int& id,
											string& comdb,
											string& EOLComments,
											TLGCProject* projet);

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
