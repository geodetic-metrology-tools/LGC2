#include "TLGCApp.h"
#include "TReader.h"
#include "TLGCCalculation.h"
#include "TStreamFormatterFactory.h"
#include "Version.h"
#include "TSimulationOutputFileWriter.h"
#include "TSimFileWriter.h"
//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
const string TLGCApp::fCopyright = "Copyright 2015, CERN SU. All rights reserved.";


TLGCApp::TLGCApp(const std::string& infileLocation, const std::string& outfileLocation):
	fInputFileLoc(infileLocation),
	fOutputFileLoc(outfileLocation),
	fStream(nullptr)
{
	fLoggerFileLoc = fOutputFileLoc.substr(0, fOutputFileLoc.length() - 4) + "Log" + ".log";  //Cut the extension + the dot(.)
}

TLGCApp::~TLGCApp()
{ }

/*! 
		\throws Throws a runtime_error if the input file was not found or is not readable or if errors found in the input file or if errors occured in the file.
*/
bool TLGCApp::exec()
{
	bool result = false;

	std::ifstream inputFileStream (fInputFileLoc, std::ifstream::in);
	std::ifstream cp_inputFileStream(fInputFileLoc, std::ifstream::in);
	std::shared_ptr<TLGCData> projectData(new TLGCData);

	//The input file exists, already test in main.cpp.
	projectData->getFileLogger().setOutputfileLocation(fLoggerFileLoc);
	projectData->getFileLogger().writeReportHeader("LGC output file");

	//Read the input file. If error occured during the reading proces output them into an LOG file and throw an exception.
	TReader r(projectData);
	if (r.isLgc2File(cp_inputFileStream))
	{
		if (!r.read(inputFileStream))
			throw runtime_error("Errors found in the input file, check the output file: " + fLoggerFileLoc + " for more details.");
	}
	else
	{
		if (!r.readLgc1File(inputFileStream))
			throw runtime_error("Errors found in the input file, check the output file: " + fLoggerFileLoc + " for more details.");
	}
	
	//Initialize the writer into the output file.
	initializeStream(projectData);

	//Run the calculation, results are obtained in the 'projectData', 
	TLGCCalculation lgcCalculation(projectData);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(new TSimulationOutputFileWriter(fStream.get(), projectData.get()));
	
	result = lgcCalculation.computeResults(fileWriter);

	//Write input file with simulated observation (SOBS) if SIMU and SOBS are used
	if (projectData->getConfig().sim.writeLGCFile && projectData->getConfig().sim.isActive())
		writeSimFile(projectData.get());

	// Save the final results if SIMU is not used. SIMU output is writen during the calculation after each iteration.
	if(result && !projectData->getConfig().sim.isActive())
		this->saveResults(projectData.get());

	// Write deform file here to have acces to lgcCalculation
	if (result && projectData->getConfig().writeDefa.isActive())
		writeDefaFile(projectData.get(), lgcCalculation.getResultMtr());

	return result;
}

void TLGCApp::initializeStream(std::shared_ptr<TLGCData> dat){
	
	TFileParameters resultFileParam;
	resultFileParam.setFileName(fOutputFileLoc);  

	//Some keywords(options) in the input file responsible for this, for now just setting here one of them (column), but can be semi-colon, dash etc.
	TAStreamFormatter::ETextFormat resultsFileFormat = TAStreamFormatter::ETextFormat::kColumnFormat;

	TStreamFormatterFactory* formatterFactory = TStreamFormatterFactory::instance();
	TDataParameters dataParam;
	dataParam.setRefFrame(TRefSystemFactory::ERefFrame::kCCS); //default param because not redefine
	dataParam.setPrecision( dat->getConfig().outPrecision.digits);

	TADataSet tads(resultFileParam, dataParam);
	fStream.reset(formatterFactory->getFormatter(&tads, resultsFileFormat, "   " /* separator */));
	fStream->setReferenceFrame(dataParam.getRefFrame());  //default param because not redefine
	fStream->setCoordSys(TCoordSysFactory::k3DCartesian);
}

void TLGCApp::saveResults(TLGCData *dat)
{
	// Write the standard output file
	writeStdResultsFile(dat);
	
	// Write punch files if needed
	if(dat->getConfig().writePunch.isActive())
		writePunchFile(dat);

	//Write error file (FAUT)
	if(dat->getConfig().faut.isActive()==true)
		writeFautFile(dat);
	
}

void TLGCApp::writeStdResultsFile(TLGCData *dat)
{	
	// change stream name
	fStream->resetStreamName(fOutputFileLoc);
	TResultsFileWriter resultsFileWriter(fStream.get(), dat);
	
	if (!dat->getFileLogger().hasErrors())
		resultsFileWriter.writeFile();
	else
		resultsFileWriter.writeFile("Error has occured, see the LGC log file.");
}

void TLGCApp::writePunchFile(TLGCData* dat)
{
	// change stream name
	std::size_t found = fOutputFileLoc.find_last_of(".");
	fOutputFileLoc=fOutputFileLoc.substr(0, found);

	auto writefile = [&](TPunchFileWriter punchFileWriter) {
		if (!dat->getFileLogger().hasErrors())
		 punchFileWriter.writeFile();
	   else
		punchFileWriter.writeFile("Error has occured, see the LGC log file."); 
	};

	if (dat->getConfig().writePunch.kOUT1)
	{
		fStream->resetStreamName(fOutputFileLoc + ".coo");
		TPunchFileWriter punchFileWriter(fStream.get(), dat);
		writefile(punchFileWriter);
	}
	else if (dat->getConfig().writePunch.kOUT3)
	{
		fStream->resetStreamName(fOutputFileLoc + ".mes");
		TPunchFileWriter punchFileWriter(fStream.get(), dat);
		writefile(punchFileWriter);
	}
	else
	{
		fStream->resetStreamName(fOutputFileLoc + ".pun");
		TPunchFileWriter punchFileWriter(fStream.get(), dat);
		writefile(punchFileWriter);
	}

	
}

void TLGCApp::writeFautFile(TLGCData* dat)
{
	// change stream name
	std::size_t found = fOutputFileLoc.find_last_of(".");
	fOutputFileLoc=fOutputFileLoc.substr(0, found);

	fStream->resetStreamName(fOutputFileLoc + ".err");
	TFautFileWriter fautFileWriter(fStream.get(), dat);

	if (!dat->getFileLogger().hasErrors())
		fautFileWriter.writeFile(dat);
	else
		fautFileWriter.writeFile("Error has occured, see the LGC log file.");
}

void TLGCApp::writeDefaFile(TLGCData* dat, TLSResultsMatrices &fResMtrx)
{
	// change stream name
	std::size_t found = fOutputFileLoc.find_last_of(".");
	fOutputFileLoc = fOutputFileLoc.substr(0, found);

	fStream->resetStreamName(fOutputFileLoc + ".def");
	TDefaFileWriter defaFileWriter(fStream.get(), dat);

	if (!dat->getFileLogger().hasErrors())
		defaFileWriter.writeFile(*dat, fResMtrx);
	else
		defaFileWriter.writeFile("Error has occured, see the LGC log file.");
}

void TLGCApp::writeSimFile(TLGCData* dat)
{
	// change stream name
	std::size_t found = fOutputFileLoc.find_last_of(".");
	fOutputFileLoc = fOutputFileLoc.substr(0, found);

	fStream->resetStreamName(fOutputFileLoc + ".sim");
	TSimFileWriter simFileWriter(fStream.get(), dat);

	if (!dat->getFileLogger().hasErrors())
		simFileWriter.writeFile();
	else
		simFileWriter.writeFile("Error has occured, see the LGC log file.");
}

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////General information stuff
///////////////////////////////////////////////////////////////////////////////////////////////
// return the software version id string
const string TLGCApp::getProgId()
{
	std::stringstream id;
	id << "LGC2 " << getLGCVersion() << ", version was not compiled against SurveyLib, compiled on " <<   __DATE__ ;
	return id.str();
}


// return the software copyright string
const string TLGCApp::getCopyright()
{
	return fCopyright;
}

