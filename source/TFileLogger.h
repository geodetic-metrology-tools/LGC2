#ifndef FILELOGGER_H
#define FILELOGGER_H

#include <string>
#include <fstream> 

	/*!
		Class for writing into a file, provides a formatted output.
		Enable to specify Log type of message (ERROR, WARNING, INFO).
	*/
    class TFileLogger {

        public:


            /// Identification of Log type
            enum class e_logType { LOG_ERROR, LOG_WARNING, LOG_INFO };

			/*!@name Constructor / destructors*/
			//@{

			/*!
				Creates a new File logger instance.
				\param[in] fileLocation Location of the file to be written to, e.g.  "c:/temp/output.out"
				\param[in] reportHeader Header of this output file section, first message appended in the end of the file, e.g. "Reading input file: "
			*/
            explicit TFileLogger (const std::string& fileLocation, const std::string& reportHeader);


			explicit TFileLogger ():numWarnings (0U),numErrors (0U){}

			void setOutputfileLocation(const std::string& fileLocation);

			std::string getOutputFileLocation(){return outputFileLocation;}

			void writeReportHeader(const std::string& reportHeader);

			void writeNumberOfLogs();

            /*! 
				Destructor, writes number of errors and warnings and closes filestream.
			*/
            ~TFileLogger ();

			//@}
	
			/*!@name Public mehods, writing / querying*/
			//@{

            /// Overload << operator using log type
            friend TFileLogger &operator << (TFileLogger &logger, const e_logType l_type);
		
            /// Overload << operator using log type
            friend TFileLogger &operator << (TFileLogger &logger, const std::string& text);

			/// Returns true, if at least one error reported, FALSE otherwise
			 bool hasErrors();

			 //@}

        private:
			// Neither copyable nor assignable
			TFileLogger(const TFileLogger& x);
			TFileLogger& operator=(const TFileLogger& x);

			///Output file stream
            std::ofstream           outputFile;
			///Output file stream
            std::string				outputFileLocation;
			///Number of warnings
            unsigned int            numWarnings;
			///Number of errors
            unsigned int            numErrors;

    }; 

#endif // FILELOGGER_H