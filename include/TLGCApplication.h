// TLGCApplication.h
//
/*! Class for a LGC application : 
file reading, interaction with LGCproject, results writing 

 Patterns:

 
 Copyright 2000-2008 M.Jones CERN TS/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_TLGCApplication
#define SU_TLGCApplication


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <iostream>
using namespace std;
//
//class  **classname**;
#include	"TDataParameters.h"
#include	"TLGCProject.h"
#include	"TFileParameters.h"

#include	"TAStreamFormatter.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////



//! Class for a LGC application : file reading, interaction with LGCproject, results writing
class  TLGCApplication //: public TObject  
{
public:
	//constants

	/*!@name Constructors and Destructors */
	//@{
		//! Constructor taking command line paramaters
		TLGCApplication(int argc, char** argv);

		//! Destructor
		virtual  ~TLGCApplication();
	//@}


	/*!@name member function */
	//@{
		//! execute the application
		int exec();

		/*! return the software id.*/
		static const string getProgId();

		/*! return the software copyright*/
		static const string getCopyright();

		// test function : to be deleted
		TLGCProject getProject() const { return *fProject; }
	//@}




private:

	/*!@name private member functions */
	//@{
		//!Copy constructor : not implemented
		TLGCApplication( const TLGCApplication& );

		//!Copy assignment operator : not implemented
		TLGCApplication& operator=( const TLGCApplication& );

		//! extract project file names file parameters from the command line parameters
		TFileParameters  getNameFileParams();

		/*! extract input file parameters from the name file*/
		TFileParameters  getInputFileParams();

		/*! extract result file parameters from the name file*/
		TFileParameters  getResultsFileParams();

		/*! extract Deform file parameters from the name file*/
		TFileParameters  getDeformFileParams();

		/*! read a line from a stream (for the name file)
		@param fileStream stream extract from a file
		@return a string*/
		string readLine(fstream* fileStream);

		/*! read the project file names file, store the file parameters in the project
		    and load the input file data into the project
		@param fileParams the parameters of the file to read*/
		int load(const TFileParameters& fileParams);

		/*! return a boolean to indicate if GUI is used or not : actually always false*/
		bool usingGUI();

		/*! check number and type of command line arguments
		@param currArg number of the current argument
		@param numParams parameters number*/
		bool argsChkOK(int currArg, int numParams);

		/*! get the arguments of the command line*/
		void getCommandLine();

		/*! run the application (without GUI)*/
		int runApp();

		/*! run the application (with GUI)*/
		int runGUI();

		void makeSaveStream(TLGCDataSet* ds, string separator, TAStreamFormatter::ETextFormat textFormat);

		/*! Save all the results files requested for a  standard LS computation
		@param project TLGCProject pointer completed by the calculation*/
		void	saveLSResults(TLGCProject *);

		/*! Save all the results files requested for a simulated LS computation
		@param project TLGCProject pointer completed by the calculation*/
		void	saveSimulatedLSResults(TLGCProject *);

		/*! write the standard results file for a project
		@param project TLGCProject pointer completed by the calculation*/
		void	writeStdResultsFile(TLGCProject *);

		/*! write and save the error analysis file
		@param project TLGCProject pointer completed by the calculation*/
		void	writeFautFile(TLGCProject*);

		/*! write and save the punch file
		@param project TLGCProject pointer completed by the calculation*/
		void	writePunchFile(TLGCProject*);

		/*! write and save the coo file
		@param project TLGCProject pointer completed by the calculation*/
		void	writeCooFile(TLGCProject*);

		/*! write and save the measurements file
		@param project TLGCProject pointer completed by the calculation*/
		void	writeMesFile(TLGCProject*);

		/*! write and save the radial offsets file
		@param project TLGCProject pointer completed by the calculation*/
		void	writeRadFile(TLGCProject*);

		/*! write and save the simulated input file
		@param project TLGCProject pointer completed by the calculation*/
		void	writeSimInputFile(TLGCProject*);

		// writes a deformation analysis input file
		void	writeDefAFile(TLGCProject*);
//@}


private:

	static const string		fCopyright;
	
	bool					fProgramError;
	bool					fUseGUI;

	int						fArgc;
	char**					fArgv;

	TLGCProject*			fProject;

	TAStreamFormatter*		fStream;

	//ClassDef(TLGCApplication, 1)
};


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////





#endif // SU_TLGCApplication
