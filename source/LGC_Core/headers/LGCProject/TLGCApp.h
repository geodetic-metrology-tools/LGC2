/*
 * SPDX-FileCopyrightText: 2025 CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGCAPP_H_
#define _LGCAPP_H_

#include "Defaults.h"

//STL
#include <memory>
#include <string>
//SURVEYLIB
#include "TStreamFormatterFactory.h"
#include "TLSResultsMatrices.h"

#include <Behavior.h>
#include <chrono>

class TLGCData;
class TLGCCalculation;

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
		TLGCApp(const std::string &infileLocation, const std::string &outfileBasename, const int maxIterations = MAX_ITERATIONS);

		/// Destructor
		~TLGCApp();

		/// Returns the software id
		static const std::string getProgId();

		/// Returns the software copyright
		static const std::string getCopyright();

		///Executes the application
		Behavior exec();

        /// Write the given data as an LGC input file into the given file location
        static bool writeLGCFile(std::shared_ptr<TLGCData> dat, const std::string &filePath);

        /// Initialise the given stream
        static void initializeStream(std::shared_ptr<TLGCData> dat, const std::string &filePath, std::shared_ptr<TAStreamFormatter> &stream);

        /// Save the results in files
        static void saveResults(TLGCData const * const dat, std::string outputFileLocation, const TLGCCalculation &calculation, std::shared_ptr<TAStreamFormatter> &stream);
        
        /// Write the standard results file
        static void writeStdResultsFile(TLGCData const * const dat, const std::string &outputFileLocation, std::shared_ptr<TAStreamFormatter> &stream);

        /// Write simulated files 
        static void writeSimFile(TLGCData const * const dat, const std::string &outputFileLocation, std::shared_ptr<TAStreamFormatter> &stream);

        /// Write punch files including coordinates of calculated points
        static void writePunchFile(TLGCData const * const dat, const std::string &outputFileLocation, std::shared_ptr<TAStreamFormatter> &stream);

        /// Write faut files 
        static void writeFautFile(TLGCData const * const dat, const std::string &outputFileLocation, std::shared_ptr<TAStreamFormatter> &stream);

        /// Write files for Deform
        static void writeDefaFile(TLGCData const * const dat, const std::string &outputFileLocation, const TLSResultsMatrices &fResMtrx, std::shared_ptr<TAStreamFormatter> &stream);

        /// Write files for covariances
        static void writeCovarFile(TLGCData const * const dat, const std::string &outputFileLocation, std::shared_ptr<TAStreamFormatter> &stream);

		/// Write files for bestfit analysis
		static void writeChabaFile(TLGCData const * const dat, const std::string &outputFileLocation, std::shared_ptr<TAStreamFormatter> &stream);

		/// Convert a timestamp to a string
		const std::string convertTimestampToString(const std::chrono::system_clock::time_point &timestamp, const char *format);
        
		/// Compute and return a time diference
		const static double computeProcessingElapsedSeconds(const std::chrono::system_clock::time_point &timeStart, const std::chrono::system_clock::time_point &timeEnd);
		
		/// Get the string of the processing timestamp
		static std::string getStartProcessingTimestamp() { return startProcessingTimestampOUT; };

		/// Get the processing elapsed time in seconds
		static double getProcessingElapsedSeconds() { return processingElapsedSeconds; };

#if USE_SERIALIZER
		/// Write serialization files
		static void writeJsonFiles(TLGCData const *const dat, const std::string &outputFileLocation, const TLSResultsMatrices &fResMtrx);
#endif // USE_SERIALIZER


	private:
		static const std::string		copyright;
		static const std::string		year;
		static const std::string		license;

		// Neither copyable nor assignable
		TLGCApp(const TLGCApp& x);
		TLGCApp& operator=(const TLGCApp& x);

		std::string fInputFileLoc; //Location of the input file
		std::string fOutputFileLoc; // Path to the final destination of .res file. During `saveResults` the extension is removed.
		std::string fLoggerFileLoc; //Location of the input file with extension, e.g. C://Temp//outputLog.log 
		std::string fNamFile; //Location of the 'name' file
		int			fMaxIterations; // Maximal number of iterations (fixed by default at 80)

		/// Shared pointer to global stream
		std::shared_ptr<TAStreamFormatter> fStream;

        std::chrono::system_clock::time_point startProcessingTime; // Start processing time to the fraction of the second
        static std::string startProcessingTimestampISO; // Start processing timestamp string, formatted according to ISO 8601
		static std::string startProcessingTimestampOUT; // Start processing timestamp string, formatted for the output files
        
		std::chrono::system_clock::time_point endProcessingTime; // End processing time to the fraction of the second
        
		static double processingElapsedSeconds; // Time difference in fractional seconds
};

#endif