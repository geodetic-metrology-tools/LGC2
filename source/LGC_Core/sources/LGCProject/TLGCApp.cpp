#include "TLGCApp.h"
#include "TReader.h"
#include "TLGCCalculation.h"
#include "Version.h"
#include "TResultsFileWriter.h"
#include <TSimulationOutputFileWriter.h>
#include "TSimFileWriter.h"
#include "TInputFileWriter.h"
#include "TPunchFileWriter.h"
#include "TFautFileWriter.h"
#include "TDefaFileWriter.h"
#include "TCovarFileWriter.h"
#include "TChabaFileWriter.h"
#include <Logger.hpp>
#include <TLGCData.h>



//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
const std::string TLGCApp::fCopyright = "Copyright 2015, CERN SU. All rights reserved.";


TLGCApp::TLGCApp(const std::string& infileLocation, const std::string& outfileLocation, const int maxIterations) :
	fInputFileLoc(infileLocation),
	fOutputFileLoc(outfileLocation),
	fStream(nullptr),
	fMaxIterations(maxIterations)
{
	
	fLoggerFileLoc = fOutputFileLoc.substr(0, fOutputFileLoc.length() - 4) + ".log";  // fLoggerFileLoc will become OBSOLETE!
}

TLGCApp::~TLGCApp()
{ }

/*! 
		@return Returns false if the input file was not found or is not readable or if errors found in the input file or if errors occured in the file, otherwise true
*/
Behavior TLGCApp::exec()
{
	std::ifstream inputFileStream (fInputFileLoc, std::ifstream::in);
	std::ifstream cp_inputFileStream(fInputFileLoc, std::ifstream::in);
	std::shared_ptr<TLGCData> projectData(new TLGCData);

	//The input file exists, already test in main.cpp.
	projectData->getFileLogger().setOutputfileLocation(fLoggerFileLoc);  // will become OBSOLETE!
	projectData->getFileLogger().writeReportHeader("LGC output file");   // will become OBSOLETE!
	logInfo() << "Starting the adjustment calculations";

	// Reinitializes the log counters (warnings, critical errors,...)
	Logger::getLogger().clearCounters();

	//Initialise Behavior with an error during the read
	Behavior result(Behavior::BehaviorCode::ERR_readingContent, L"Errors found in the input file, check the log file for more details.");

	//Read the input file. If error occured during the reading proces output them into an LOG file and throw an exception.
	TReader r(projectData);
	if (r.isLgc2File(cp_inputFileStream))
	{
		if (!r.read(inputFileStream))
		{
			return result;
			//throw runtime_error("Errors found in the input file, check the output file: " + fLoggerFileLoc + " for more details.");
		}
	}
	else
	{
		if (!r.readLgc1File(inputFileStream))
		{
			return result;
			//throw runtime_error("Errors found in the input file, check the output file: " + fLoggerFileLoc + " for more details.");
		}
	}
	logInfo() << "Input data correctly read";

	//Initialize the writer into the output file.
	initializeStream(projectData, fOutputFileLoc, fStream);

	//Run the calculation, results are obtained in the 'projectData', 
	TLGCCalculation lgcCalculation(projectData, fMaxIterations);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(new TSimulationOutputFileWriter(fStream.get(), projectData.get()));
	
	//if we are here, the reading is well done, clean Behavior and launch calculation
	result.extract(Behavior::BehaviorCode::ERR_readingContent);
	result = lgcCalculation.computeResults(fileWriter);

	// Save the final results (SIMU output is written during the calculation after each iteration)
	if(result)
        this->saveResults(projectData.get(), fOutputFileLoc, lgcCalculation, fStream);

	logInfo() << "Calculation process ended.";
	return result;
}

bool TLGCApp::writeLGCFile(std::shared_ptr<TLGCData> dat, const std::string &filePath){
    // Create and initialise stream:
    std::shared_ptr<TAStreamFormatter> stream;
    initializeStream(dat, filePath, stream);

    // Create writer, write the file:
    TInputFileWriter infileWriter(stream.get(), dat.get());
    try{
        infileWriter.writeFile();
    } catch(...) {
        // There were some problems with the TLGCData, and the writer
        // tried to read faulty locations. Return false.
        return false;
    }
    return true;
}

void TLGCApp::initializeStream(std::shared_ptr<TLGCData> dat, const std::string &filePath, std::shared_ptr<TAStreamFormatter> &stream){
	
	TFileParameters resultFileParam;
    resultFileParam.setFileName(filePath);

	//Some keywords(options) in the input file responsible for this, for now just setting here one of them (column), but can be semi-colon, dash etc.
	TAStreamFormatter::ETextFormat resultsFileFormat = TAStreamFormatter::ETextFormat::kColumnFormat;

	TStreamFormatterFactory* formatterFactory = TStreamFormatterFactory::instance();
	TDataParameters dataParam;
	dataParam.setRefFrame(TRefSystemFactory::ERefFrame::kCCS); //default param because not redefine
	dataParam.setPrecision( dat->getConfig().outPrecision.digits);

	TADataSet tads(resultFileParam, dataParam);
    stream.reset(formatterFactory->getFormatter(&tads, resultsFileFormat, "   " /* separator */));
    stream->setReferenceFrame(dataParam.getRefFrame());  //default param because not redefine
    stream->setCoordSys(TCoordSysFactory::k3DCartesian);

    TPointFormat* pointFormat = stream->getPointFormat();
	//Set the name width to the max width of a name point +1 characters.
	if (dat->getConfig().pointNameWidth>pointFormat->getNameWidth())
	{
		pointFormat->setNameWidth(dat->getConfig().pointNameWidth+1);
        stream->setPointFormat(*pointFormat);
	}
	//As the name width is used also for frames. Set the name width to the max of of frame name +1 characters, if necessary.
	for (const auto& itTree : dat->getTree())
	{
		if (itTree.get()->frame.getName().length() >= pointFormat->getNameWidth()) {
			pointFormat->setNameWidth((int)itTree.get()->frame.getName().length() + 1);
			stream->setPointFormat(*pointFormat);
		}
	}

}

void TLGCApp::saveResults(TLGCData const * const dat, std::string outputFileLocation, const TLGCCalculation &calculation, std::shared_ptr<TAStreamFormatter> &stream)
{
    const auto &conf = dat->getConfig();

    // Write the standard output file if simulation mode is not used:
    if(!conf.sim.isActive())
        writeStdResultsFile(dat, outputFileLocation, stream);

    // Remove the output file location extension (each file writer will add a different extension):
    std::size_t found = outputFileLocation.find_last_of(".");
    outputFileLocation = outputFileLocation.substr(0, found);

    //Write input file with simulated observation (SOBS) if SIMU and SOBS are used
    if(conf.sim.isActive() && conf.sim.writeLGCFile)
        writeSimFile(dat, outputFileLocation, stream);

	// Write punch files if needed
	if(conf.writePunch.isActive())
        writePunchFile(dat, outputFileLocation, stream);

	//Write error file (FAUT)
	if(conf.faut.isActive())
        writeFautFile(dat, outputFileLocation, stream);

	// Write covariance matrices
	if (conf.covar.isActive())
        writeCovarFile(dat, outputFileLocation, stream);

	// Write best fit analysis
	if (conf.chaba.isActive())
		writeChabaFile(dat, outputFileLocation, stream);
	
    // Write deform file here to have acces to lgcCalculation
    if(conf.writeDefa.isActive())
        writeDefaFile(dat, outputFileLocation, calculation.getResultMtr(), stream);

}

void TLGCApp::writeStdResultsFile(TLGCData const * const dat, const std::string &outputFileLocation, std::shared_ptr<TAStreamFormatter> &stream)
{	
	// change stream name
    stream->resetStreamName(outputFileLocation);
    TResultsFileWriter resultsFileWriter(stream.get(), dat);
	
	if (!Logger::getLogger().hasErrors())
	{
		resultsFileWriter.writeFile();
		logInfo() << "Result file:" << stream->getFileName() << "has been written";
	}
	else
	{
		resultsFileWriter.writeFile("Error has occured, see the LGC log file.");
		logCritical() << "Result file:" << stream->getFileName() << "has NOT been written";
	}
}

void TLGCApp::writePunchFile(TLGCData const * const dat, const std::string &outputFileLocation, std::shared_ptr<TAStreamFormatter> &stream)
{
	auto writefile = [&](TPunchFileWriter punchFileWriter) 
	{
		if (!Logger::getLogger().hasErrors())
		{
			punchFileWriter.writeFile();
			logInfo() << "Punch file:" << stream->getFileName() << "has been written";
		}
		else
		{
			punchFileWriter.writeFile("Error has occured, see the LGC log file."); 
			logCritical() << "Punch file:" << stream->getFileName() << "has NOT been written";
		}
	};

	if (dat->getConfig().writePunch.kOUT1)
	{
        stream->resetStreamName(outputFileLocation + ".coo");
        TPunchFileWriter punchFileWriter(stream.get(), dat);
		writefile(punchFileWriter);
	}
	else if (dat->getConfig().writePunch.kOUT3)
	{
        stream->resetStreamName(outputFileLocation + ".mes");
        TPunchFileWriter punchFileWriter(stream.get(), dat);
		writefile(punchFileWriter);
	}
	else
	{
        stream->resetStreamName(outputFileLocation + ".pun");
        TPunchFileWriter punchFileWriter(stream.get(), dat);
		writefile(punchFileWriter);
	}
}

void TLGCApp::writeFautFile(TLGCData const * const dat, const std::string &outputFileLocation, std::shared_ptr<TAStreamFormatter> &stream)
{
    stream->resetStreamName(outputFileLocation + ".err");
    TFautFileWriter fautFileWriter(stream.get(), dat);

	if (!Logger::getLogger().hasErrors() && dat->fUEOIndices.UIndex != 0)
	{
		fautFileWriter.writeFile(dat);
		logInfo() << "Fault file:" << stream->getFileName() << "has been written";
	}
	else if (dat->fUEOIndices.UIndex != 0)
	{
		fautFileWriter.writeFile("No data because there s no unknowns.");
		logCritical() << "Fault file:" << stream->getFileName() << "has NOT been written";
	}
	else
	{
		fautFileWriter.writeFile("Error has occured, see the LGC log file.");
		logCritical() << "Fault file:" << stream->getFileName() << "has NOT been written";
	}
}

void TLGCApp::writeDefaFile(TLGCData const * const dat, const std::string &outputFileLocation, TLSResultsMatrices &fResMtrx, std::shared_ptr<TAStreamFormatter> &stream)
{
    stream->resetStreamName(outputFileLocation + ".def");
    TDefaFileWriter defaFileWriter(stream.get(), dat);

	if (!Logger::getLogger().hasErrors())
	{
		defaFileWriter.writeFile(*dat, fResMtrx);
		logInfo() << "Def file:" << stream->getFileName() << "has been written";
	}
	else
	{
		defaFileWriter.writeFile("Error has occured, see the LGC log file.");
		logCritical() << "Def file:" << stream->getFileName() << "has NOT been written";
	}
}

/// Write files for covariances
void TLGCApp::writeCovarFile(TLGCData const * const dat, const std::string &outputFileLocation, std::shared_ptr<TAStreamFormatter> &stream)
{
    stream->resetStreamName(outputFileLocation + ".cov");
    TCovarFileWriter covarFileWriter(stream.get(), dat);

	if (!Logger::getLogger().hasErrors())
	{
		covarFileWriter.writeFile(*dat);
		logInfo() << "Covariance file:" << stream->getFileName() << "has been written";
	}
	else
	{
		covarFileWriter.writeFile("Error has occured, see the LGC log file.");
		logCritical() << "Covariance file:" << stream->getFileName() << "has NOT been written";
	}
}

/// Write files for covariances
void TLGCApp::writeChabaFile(TLGCData const * const dat, const std::string &outputFileLocation, std::shared_ptr<TAStreamFormatter> &stream)
{
	stream->resetStreamName(outputFileLocation + ".chabaOut");
	TChabaFileWriter chabaFileWriter(stream.get(), dat);

	if (!Logger::getLogger().hasErrors())
	{
		chabaFileWriter.writeFile(stream.get());
		logInfo() << "Chaba output file:" << stream->getFileName() << "has been written";
	}
	else
		logCritical() << "Chaba output file:" << stream->getFileName() << "has NOT been written";

}

void TLGCApp::writeSimFile(TLGCData const * const dat, const std::string &outputFileLocation, std::shared_ptr<TAStreamFormatter> &stream)
{
    stream->resetStreamName(outputFileLocation + ".sim");
	TSimFileWriter simFileWriter(stream.get(), dat);

	if (!Logger::getLogger().hasErrors())
	{
		simFileWriter.writeFile();
		logInfo() << "Simulation file:" << stream->getFileName() << "has been written";
	}
	else
	{
		simFileWriter.writeFile("Error has occured, see the LGC log file.");
		logCritical() << "Simulation file:" << stream->getFileName() << "has NOT been written";
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////General information stuff
///////////////////////////////////////////////////////////////////////////////////////////////
// return the software version id string
const std::string TLGCApp::getProgId()
{
	std::stringstream id;
	id << "LGC2 " << getLGCVersion() << ", compiled on " <<   __DATE__ ;
	return id.str();
}


// return the software copyright string
const std::string TLGCApp::getCopyright()
{
	return fCopyright;
}

