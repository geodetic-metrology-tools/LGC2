// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TFileLogger.h"

TFileLogger::TFileLogger(const std::string &fileLocation, const std::string &reportHeader) : numWarnings(0U), numErrors(0U)
{
	outputFile.open(fileLocation, std::ios::app);

	if (outputFile.is_open())
	{
		outputFile << reportHeader << std::endl << std::endl;
	}
}

void TFileLogger::setOutputfileLocation(const std::string &fileLocation)
{
	outputFileLocation = fileLocation;
	outputFile.open(fileLocation);
}

void TFileLogger::writeReportHeader(const std::string &reportHeader)
{
	if (outputFile.is_open())
	{
		outputFile << reportHeader << std::endl << std::endl;
	}
}

void TFileLogger::writeNumberOfLogs()
{
	if (outputFile.is_open())
	{
		outputFile << std::endl;
		// Report number of errors and warnings
		outputFile << numWarnings << " warnings" << std::endl;
		outputFile << numErrors << " errors" << std::endl << std::endl;
	}
}

TFileLogger::~TFileLogger()
{
	if (outputFile.is_open())
	{
		outputFile << std::endl;

		// Report number of errors and warnings
		outputFile << numWarnings << " warnings"
				   << "\n";
		outputFile << numErrors << " errors"
				   << "\n"
				   << "\n";

		// link to the user guide
		outputFile << "User guide: "
				   << "https://lgc2.docs.cern.ch/"
				   << "\n"
				   << "\n";
		outputFile << "For problematic cases consider checking the initial residuals (*ALLFIXED option or fixing variables)"
				   << "\n"
				   << "or use the *CONSI option to check the geometric consistency of the configuration."
				   << "\n";
		outputFile << "Please report issues via Jira: "
				   << "https://cern.ch/lgc2-bug"
				   << "\n";
		outputFile << "Or via GitHub issues."
				   << "\n";

		outputFile.close();
	}
}

TFileLogger &operator<<(TFileLogger &logger, const TFileLogger::e_logType l_type)
{
	switch (l_type)
	{
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

TFileLogger &operator<<(TFileLogger &logger, const std::string &text)
{
	logger.outputFile << text << std::endl;
	return logger;
}

bool TFileLogger::hasErrors() const
{
	if (numErrors > 0)
		return true;
	else
		return false;
}
