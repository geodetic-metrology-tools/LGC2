// TLGCProject.h
//
/*! A LGC project  (manages the data storing and the calculation) 

 Patterns:

  
 Copyright 2000-2008 M. Jones, CERN TS/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TLGCProject
#define SU_TLGCProject


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <**classname**>
//using namespace std;
//
class TLGCApplication;
#include  "TFileParameters.h"
#include  "TDataParameters.h"
#include  "TLGCDataSet.h"
#include  "TPtrWrapper.h"
#include  "ObservationSet.h"
#include  "LSCalcDataSet.h"
#include  "TLGCOutputOptions.h"
#include  "TLGCCalcParams.h"
#include  "TLSCalculation.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////



//Class definition
class  TLGCProject //: public TObject  
{
public:

	/*!@name Constructors and Destructors */
	//@{
		/*! Default Constructor*/
		TLGCProject();

		/*! Copy constructor : non-const argument because of TDataParameters construction*/
		TLGCProject(TLGCProject&);

		//! Destructor
		virtual  ~TLGCProject();
	//@}

	/*!@name Operators */
	//@{
		//! Copy assignment operator : non-const argument because of TDataParameters construction
		TLGCProject& operator=(TLGCProject&);
	//@}


	/*!@name Access methods*/
	//@{

		/*! gets the project title*/
		static string getTitle();
		/*! gets the project data set*/
		TLGCDataSet*  getDataSet();
		/*! gets the project data set*/
		const TLGCDataSet*  getDataSet() const;
		/*! gets the project file name*/
		TFileParameters  getNameFile() const;
		/*! gets the results file parameters*/
		TFileParameters  getInputFileParams() const;
		/*! gets the results file parameters*/
		TFileParameters  getResultsFileParams() const;
		/*! gets the error message*/
		string getError() const { return fError; }
		/*! gets any warning messages */
		string getWarning() const;
		/*! gets the project output options*/
		TLGCOutputOptions*  getOutputOptions() const;
		/*! gets the project calculation parameters*/
		TLGCCalcParams*  getCalcParams() const;	
		


		/*!@return a pointer to the project's LSCalcDataSet*/
		LSCalcDataSet*	getLSCalcDataSet();
	//@}


	/*!@name Set methods*/
	//@{

		/*! sets the title*/
		void setTitle(const string title);
		/*! sets the error message*/
		void setError( const string error) { fError = error; return; }
		/*! adds another warning message */
		void addWarning(const string &warning);
		/*! set the name file parameters*/
		bool setNameFileParams(const TFileParameters& nameFile);
		/*! set the results file parameters*/
		bool setResultsFileParams(const TFileParameters& resFile);
		/*! set the deformation file parameters*/
		bool setDeformFileParams(const TFileParameters& defFile);
		/*! turn on the simulation option */
		void setSimulation();

	//@}


	/*!@name related to computation program steps*/
	//@{
		//! loads new data, overwrites existing data
		int  load( const TFileParameters& );

		//! calculate the project least squares results for the input measurements 
		void computeLSResults();

		/*!check if the computation is made with simulated values or not*/
		bool isSimulation();

		//! check if the data processing options are compatible
		bool processOptionsCompatible();

	//@}

	
private:

	static string					fTitle;

	TPtrWrapper<TLGCDataSet>		fData;
	TPtrWrapper<ObservationSet>		fObs;
	TPtrWrapper<LSCalcDataSet>		fCalcData;

	string							fError;
	string							fWarning;

	TFileParameters					fNameFile;
	TFileParameters					fInputFile;
	TFileParameters					fResultsFile;
	TFileParameters					fDeformFile;

	TLGCOutputOptions*				fOutOptions;
	TLGCCalcParams*					fParams;


	//ClassDef(TLGCProject, 1)
};


#endif // SU_TLGCProject
