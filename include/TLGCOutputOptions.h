// TLGCOutputOptions.h
//
/*! The parameters of a LGC project. Identifies the specific options to a LGC output file:

  		naming conventions
		punch files options
*/
// Patterns:
//
// 
// Copyright 2000-2008 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_LGC_OUT_OPTIONS
#define SU_LGC_OUT_OPTIONS


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <string>
//using namespace std;
#include  "TSpatialPointName.h"
//
// typedefs
//
//
////////////////////////////////////////////////////////////////

//Class definition
class  TLGCOutputOptions //: public TObject 

{
public:

	/*!@name Enumerators for *PUNC options */
	//@{
		/*! Enumeration of the possible punc options */
		enum	EPunc {kNo, kDefault, kE, kEE, kH, kZ, kZH, kHN, kZHN, kT, kCOO, kMES}; 
	//@}

	/*!@name Constructors and Destructors */
	//@{
		//! default constructor
		TLGCOutputOptions();

		//! copy constructor
		TLGCOutputOptions(const TLGCOutputOptions&);

		//! destructor
		virtual  ~TLGCOutputOptions();

	//@}


	/*!@name member functions */
	//@{
		//! copy assignment operator
		TLGCOutputOptions& operator=(const TLGCOutputOptions& );

		//! equivalence operator
		bool	operator==(const TLGCOutputOptions& ) const;

		/*! turn on the option for the use of GEODE naming convention*/
		void	setUsingGEODEOption();
		
		/*! indicate if the "_" must be replace by space in output file*/
		bool	usingGeodeFiles() const;

		/*! turn on the option for simulated input file creation*/
		void	simInputToCreate() {fSaveInpSimFile = true; return;}

		/*! indicate if simulated input file have to be created or not*/
		bool	isSimInputToBeSaved() const {return fSaveInpSimFile;}

		bool	isDeformationAnalysisToBeCreated() { return fDeformationAnalysisOption; }
		std::string getDeformationAnalysisTitle() { return deformationAnalysisTitle; }

		/*! set the punch file selection */
		void	setPuncOption(const TLGCOutputOptions::EPunc ops) {fPuncOptions = ops;return;}
		/*! set the request for the database summary file */
		void	setCooOption(){fCooOptions = kCOO;return;}
		/*! set the request for the measurement summary file */
		void	setMesOption(){fMesOptions = kMES;return;}

		/*! get the punch file selection */
		TLGCOutputOptions::EPunc	isPunchfileToBeSaved() const {return fPuncOptions;}
		/*! get the request status for the database summary file */
		TLGCOutputOptions::EPunc	isCoofileToBeSaved() const {return fCooOptions;}
		/*! get the request status for the measurement summary file */
		TLGCOutputOptions::EPunc	isMesfileToBeSaved() const {return fMesOptions;}

		/*! set the request for the output of histograms of the observation residuals */
		void						usedHistOption() {fHistOption = true;return;}

		/*! get the request status for the output of histograms of the observation residuals */
		bool						isHistOptionUsed() const {return fHistOption;}

		void						setDeformationAnalysisOption() { fDeformationAnalysisOption = true; }
		void	setDeformationAnalysisTitle(std::string newTitle) { deformationAnalysisTitle = newTitle; }
	//@}


private:
	
	TSpatialPointName::ENameType		fUsingGEODE; /*!< enumerator indicating if the GEODE naming convention is used*/
	bool								fSaveInpSimFile; /*!< boolean indicating if, after simulation computation, an 'input' file with simulated observations' values is required */
	TLGCOutputOptions::EPunc			fPuncOptions; /*!< TLGCOutputOptions::EPunc indicating if, there's a punch file to write*/
	TLGCOutputOptions::EPunc			fCooOptions; /*!< TLGCOutputOptions::EPunc indicating if, there's a coo file to write*/
	TLGCOutputOptions::EPunc			fMesOptions; /*!< TLGCOutputOptions::EPunc indicating if, there's a mes file to write*/
	bool								fHistOption;
	bool								fDeformationAnalysisOption;
	std::string								deformationAnalysisTitle;
	//ClassDef(TLGCOutputOptions, 1)
};


#endif // SU_LGC_OUT_OPTIONS
