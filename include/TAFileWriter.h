////////////////////////////////////////////////////////////////////
// TAFileWriter
/*!
Abstract base class for LGC output file writers

Patterns:
 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/

#ifndef SU_TA_FILE_WRITER
#define SU_TA_FILE_WRITER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include	<string>
using namespace std;
class	TLGCDataSet;
class	TAStreamFormatter;
class	LSCalcDataSet;
class	TLGCProject;
#include	"TALGCConverter.h"
#include	"TLGCOutputOptions.h"
#include	"TLGCCalcParams.h"
#include    "TRefSystemFactory.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup ????

@{*/



//Class definition
class  TAFileWriter : public TALGCConverter
{
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TAFileWriter(TAStreamFormatter* stream, const TLGCProject* project);

			//!Destructor
			virtual  ~TAFileWriter();
		//@}


	/*!@name Public menber functions*/
		//@{
			//!write the lgc file used when there's error in the project
			virtual void	writeFile(TLGCDataSet*, const string error);
	
			/*!\return the writer's error as a string*/
			virtual string	getError() const;
			//! write error
			virtual void	writeError();
			//! set the error string
			virtual void	setError(const string error);

			//! set the options for the project to be output
			void	setOutputOptions(TLGCOutputOptions* outOptions);

			//! set the calculation options for the project to be output
			void	setCalcParams(const TLGCCalcParams*);

			//! get the descriptive text in French for an observation type
			static string	getObsDescriptionFR(TALGCConverter::ELGCObservations key);
						
			//! get the descriptive text in English for an observation type
			static string	getObsDescriptionEN(TALGCConverter::ELGCObservations key);
		//@}

protected:

	/*!@name Protected member functions*/
		//@{
			//!initialise attributs
			virtual void	init(TLGCDataSet*, LSCalcDataSet*, const string error);
		//@}

	/*!@name Protected access functions*/
		//@{
			//!return a pointer to a TCSGEODataSet object
			TLGCDataSet*					getDataSet() const {return fDataSet;}
		
			//!return pointer to LSCalcDataSet object
			LSCalcDataSet*					getLSCalcDataSet() {return fLSCalcDataSet;}

			//!return pointer to LSCalcDataSet object
			LSCalcDataSet*					getLSCalcDataSet() const {return fLSCalcDataSet;}	


			//!Return Reference Frame used for build (Hg)
			TRefSystemFactory::ERefFrame	getRefFrameForH() const {return fRefFrame;}

			//!return the TLGCOutputOptions::EPunc indicating the type of the punch file to write
			TLGCOutputOptions::EPunc		getPuncOptions() const {return fPuncOptions;}
	
			bool							isSAPrioriUsed() const { return fUsingSAPriori; }
			TRefSystemFactory::ERefFrame		getRefSurface() const {return fRefSurface;}	
			bool							isHistOptionUsed() const { return fHistOption; }			
		//@}
		
	/*!default constructor*/
	TAFileWriter();


	//!pointer to a TCSGEODataSet object
	TLGCDataSet*					fDataSet;

private:

	/*!@name Private Attributes*/
	//@{
		//! descriptions in French for the output of different observations
		map< string, string >		fFRObsDescriptions;

		//!pointer to LSCalcDataSet object
		LSCalcDataSet*					fLSCalcDataSet;

		//!Reference Frame used for build (Hg)
		TRefSystemFactory::ERefFrame	fRefFrame;

		//!Error
		string							fError;

		/*!< Output options  information*/
		TLGCOutputOptions::EPunc		fPuncOptions;/*!<TLGCOutputOptions::EPunc indicating the type of the punch file to write*/

		/*!< Calculation parameters information*/
		bool							fUsingSAPriori; /*!<boolean indicating if observations' sigma are calculated with sigma a priori*/
		TRefSystemFactory::ERefFrame		fRefSurface; /*!< reference surface for the project*/


		bool							fHistOption;

	//@}
};

/*@}*/

#endif // SU_TA_FILE_WRITER
