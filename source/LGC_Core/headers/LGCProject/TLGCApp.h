#ifndef _LGCAPP_H_
#define _LGCAPP_H_

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

//STL
#include <memory>
#include <string>
//SURVEYLIB
#include "TStreamFormatterFactory.h"
#include "TLSResultsMatrices.h"

class TLGCData;


/*!
	\ingroup LGCProject
	\brief Class responsible for execution of the program : reading input file, launching the calculation and writing the results.
*/
class TLGCApp {
	public:
		/*!
			\brief Creates a new TLGCApp instance with a given individual parameters which would otherwise be entries in the 'nam' file.
			
			\param[in] infileLocation[in] Location of the LGC file including the extension, e.g. C:/temp/net.lgc
			\param[in] outfileLocation[in] Location of the LGC result files including the extension, e.g. C:/temp/net.out
		*/
		TLGCApp(const std::string& infileLocation, const std::string& outfileBasename);

		/// Destructor
		~TLGCApp();

		/// Returns the software id
		static const string getProgId();

		/// Returns the software copyright
		static const string getCopyright();

		///Executes the application
		bool exec();

	private:
		static const string		fCopyright;

		// Neither copyable nor assignable
		TLGCApp(const TLGCApp& x);
		TLGCApp& operator=(const TLGCApp& x);

		std::string fInputFileLoc; //Location of the input file
		std::string fOutputFileLoc; // Output file extension
		std::string fLoggerFileLoc; //Location of the input file with extension, e.g. C://Temp//outputLog.log 
		std::string fNamFile; //Location of the 'name' file

		void initializeStream(std::shared_ptr<TLGCData> dat);
		void saveResults(TLGCData *dat);
		void writeStdResultsFile(TLGCData *dat);

		/// Write punch files including coordinates of calculated points
		void writePunchFile(TLGCData *dat);

		/// Write faut files 
		void writeFautFile(TLGCData *dat);

		/// Write files for Deform
		void writeDefaFile(TLGCData *dat, TLSResultsMatrices &fResMtrx);

		/// Write files for covariances
		void writeCovarFile(TLGCData *dat);

		/// Write simulated files 
		void writeSimFile(TLGCData *dat);

		/// Shared pointer to global stream
		std::shared_ptr<TAStreamFormatter> fStream;
};

#endif