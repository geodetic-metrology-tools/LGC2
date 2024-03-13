/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef FILELOGGER_H
#define FILELOGGER_H


//STL
#include <string>
#include <fstream> 

/*!
	\ingroup LGCProjectData

	Class for writing into a log file of the TLGCData class, providing formatted output.
	Enable to specify Log type of message (ERROR, WARNING, INFO).
*/
class TFileLogger {

    public:
    /// Identification of Log type
    enum class e_logType { LOG_ERROR, LOG_WARNING, LOG_INFO };

	/*!@name Constructor / destructors*/
	//@{

	/*!
		\brief Creates a new File logger instance.

		\param[in] fileLocation Location of the file to be written to, e.g.  "c:/temp/output.out"
		\param[in] reportHeader Header of this output file section, first message appended in the end of the file, e.g. "Reading input file: "
	*/
    explicit TFileLogger (const std::string& fileLocation, const std::string& reportHeader);


	explicit TFileLogger ():numWarnings (0U),numErrors (0U){}

	///	Destructor, writes number of errors and warnings and closes filestream.
    ~TFileLogger ();

	//@}

	/// Sets the location of the output file.
	void setOutputfileLocation(const std::string& fileLocation);

	/*!@name Public mehods, writing / querying*/
	//@{

	/// Returns the path of the output file.
	std::string getOutputFileLocation(){return outputFileLocation;}

	/// Writes the header of the file
	void writeReportHeader(const std::string& reportHeader);

	/// Writes number of errors and warnings to the file
	void writeNumberOfLogs();

    /// Overload << operator using log type
    friend TFileLogger &operator << (TFileLogger &logger, const e_logType l_type);
	
    /// Overload << operator using string text
    friend TFileLogger &operator << (TFileLogger &logger, const std::string& text);

	/// Returns TRUE, if at least one error has occured, FALSE otherwise.
	 bool hasErrors() const;

	 // reset number of errors to 0
	 void cleanErrors() { numErrors = 0; };

	 //@}

    private:
	// Neither copyable nor assignable
	TFileLogger(const TFileLogger& x);
	TFileLogger& operator=(const TFileLogger& x);

	///Output file stream
    std::ofstream           outputFile;
	///Output file location
    std::string				outputFileLocation;
	///Number of warnings
    unsigned int            numWarnings;
	///Number of errors
    unsigned int            numErrors;

}; 

#endif // FILELOGGER_H