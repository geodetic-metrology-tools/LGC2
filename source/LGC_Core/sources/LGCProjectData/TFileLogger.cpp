// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TFileLogger.h"


TFileLogger::TFileLogger (const std::string& fileLocation, const std::string& reportHeader):numWarnings (0U),numErrors (0U)
{
	
    outputFile.open (fileLocation, std::ios::app);

    if (outputFile.is_open()) {
        outputFile << reportHeader << std::endl << std::endl;
    } 

}

void TFileLogger::setOutputfileLocation(const std::string& fileLocation)
{
	outputFileLocation = fileLocation;
	outputFile.open(fileLocation);
}

void TFileLogger::writeReportHeader(const std::string& reportHeader)
{
	if (outputFile.is_open()) {
        outputFile << reportHeader << std::endl << std::endl;
    } 
}

void TFileLogger::writeNumberOfLogs()
{
	if (outputFile.is_open()) {
        outputFile << std::endl ;
        // Report number of errors and warnings
        outputFile << numWarnings << " warnings" << std::endl;
        outputFile << numErrors << " errors" << std::endl << std::endl;
    } 
}

TFileLogger::~TFileLogger () 
{
    if (outputFile.is_open()) {
        outputFile << std::endl ;

        // Report number of errors and warnings
        outputFile << numWarnings << " warnings" << std::endl;
        outputFile << numErrors << " errors" << std::endl << std::endl;


		//link to the user guide
		outputFile << "User guide: "
				   << "https://confluence.cern.ch/display/SUS/LGC2+User+Guide" << std::endl
				   << std::endl;
		outputFile << "For problematic cases consider checking the initial residuals (*ALLFIXED option or fixing variables)" << std::endl
				   << "or use the *CONSI option to check the geometric consistency of the configuration." << std::endl;
		outputFile << "Please report issues via Jira: "
				   << "https://cern.ch/lgc2-bug" << std::endl;


        outputFile.close();
    } 
}

TFileLogger &operator<<(TFileLogger &logger, const TFileLogger::e_logType l_type){
    switch (l_type) {
        case TFileLogger::e_logType::LOG_ERROR:
            logger.outputFile << "[ERROR]: ";
            ++logger.numErrors;
            break;

        case TFileLogger::e_logType::LOG_WARNING:
            logger.outputFile << "[WARNING]: ";
            ++logger.numWarnings;
            break;

        default:
            logger.outputFile << "[INFO]: ";
            break;
    } 
    return logger;
}

TFileLogger &operator<<(TFileLogger &logger, const std::string& text) {
                logger.outputFile << text << std::endl;
                return logger;
            }


 bool TFileLogger::hasErrors() const {
	if(numErrors > 0)
		return true;	
	else
		return false;
}