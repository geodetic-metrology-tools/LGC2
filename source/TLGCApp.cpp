#include "TLGCApp.h"
#include "Readers/TReader.h"
#include "TLGCCalculation.h"

#include  "TStreamFormatterFactory.h"

#include "Writers\TResultsFileWriter.h"
#include "Version.h"


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
const string TLGCApp::fCopyright = "Copyright 2015, CERN SU. All rights reserved.";


/*! 
	\throws Throws/propagates runtime_error if a NAM file was not found or if it is not readable.
*/
TLGCApp::TLGCApp(const std::string& namfile):
	fNamFile(namfile), fStream(nullptr)
{
	readFilesParams();
}


TLGCApp::TLGCApp(const std::string& infileLocation, const std::string& outfileBasename, const std::string& outfileExtension):
finfileLoc(infileLocation)
{
	foutfileLoc = outfileBasename + "." + outfileExtension;
	floggerfileLoc = outfileBasename + "Log" + ".log";
}

//fStream is a Raw pointer because Stream Factory returns a raw pointer, can modify the factory to return smart pointer
TLGCApp::~TLGCApp()
{
	if(fStream !=0)
	{
		delete fStream;
	}
}

/*! 
		\throws Throws a runtime_error if the input file was not found or is not readable or if errors found in the input file.
*/
int TLGCApp::exec()
{
	bool result = false;

	std::ifstream inputFileStream (finfileLoc, std::ifstream::in);
	std::unique_ptr<TLGCData> fProjectData(new TLGCData);
	//auto sp1 = make_shared<TLGCData>();


	//If input file exists and is readable.
	if (inputFileStream.good())
	{
		fProjectData->getFileLogger().setOutputfileLocation(floggerfileLoc);
		fProjectData->getFileLogger().writeReportHeader("LGC output file");

		//Read the input file, write all errors into an ouptut file and in a case that errors were found throw an exception.
		TReader r(fProjectData.get());
		if (!r.read(inputFileStream))
			throw runtime_error("Errors found in the input file, check output file: " + foutfileLoc + " for more details.");
	}
	else{
		throw runtime_error("Input file \"" + finfileLoc + "\" was not found or is not readable!");
	}
			
	//Run the calculation and get the results
	TLGCCalculation lgcCalculation(fProjectData.get());

	result = lgcCalculation.computeLSResults();

	// save the results if computed
	if(result)
		this->saveLSResults(fProjectData.get());

	return result;
}

void TLGCApp::saveLSResults(TLGCData *dat)
{
	// write the results file
	writeStdResultsFile(dat);   /////////////////////////HERE WRITING .output FILE/////////////////////////////////////////////


	//Other file to come...
}


void	TLGCApp::writeStdResultsFile(TLGCData *dat){
#if 0
project->getDataSet()->setFileParams(project->getResultsFileParams());
	TLGCDataSet* data = project->getDataSet();
	makeSaveStream(data, data->getResultsSeparator(), data->getResultsFileFormat());
	TResultsFileWriter* file = new TResultsFileWriter (fStream, project);

	// if there is still no error the calculation has converged so save the results
	if (project->getError() == "")
	{//le calcul converge
		file->writeFile(*project);
	}
	else
	{//erreur ou le calcul ne converge pas		
		file->writeFile(data, project->getError());		
	}

	delete file;
	return;
#endif

	TFileParameters resultFileParam;
	resultFileParam.setFileName(foutfileLoc);   //foutfileLoc is the actual location of the output file, not just the file+extension (e.g. not output.out but C://Temp//output.out)
	dat->setFileParams(resultFileParam);
		

	//Some option in the input file responsible for it, for now just setting here one of them
	TAStreamFormatter::ETextFormat resultsFileFormat = TAStreamFormatter::ETextFormat::kColumnFormat;


	makeSaveStream(dat, "   " /* separator */, resultsFileFormat);
	TResultsFileWriter* file = new TResultsFileWriter (fStream, dat);



	// if there is still no error the calculation has converged so save the results
	//?? Probably not exactly what is in the description
	if (!dat->getFileLogger().hasErrors())
		file->writeFile();
	else
		file->writeFile("Error has occured, see the LGC log file.");		//Add error info from the file writer or whatever.....

	delete file;
	return;

}


void TLGCApp::makeSaveStream(TLGCData* dat, string separator, TAStreamFormatter::ETextFormat textFormat)
{
	if(fStream)
		delete fStream;

	TStreamFormatterFactory* formatterFactory = TStreamFormatterFactory::instance();
	fStream = formatterFactory->getFormatter(dat, textFormat, separator);

	/*  THIS IF ELSE IS REALLY WEIRD, DOES NOT TO WHAT EXPECTED, BECAUSE IN GETDATAPARAMS IS JUST SET DURING CONSTRUCTION AND THEN NEVER RESET*/
	/* POSSIBLE PROBLEMS, BUT WHY DO WE CARE IF TLGCData is Part of the Stream??????????????!!!!!!!!!!!!!!!!!*/
	if (dat->getDataParams().getRefFrameEnumerator() == TRefSystemFactory::kLocalRefFrame)
	{
		//points are defined in a local system
		fStream->setReferenceFrame(dat->getDataParams().getRefFrame());
	}
	else
	{
		//points are in a CERN sytem and transform in CCS
		fStream->setReferenceFrame(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
		fStream->setCoordSys(TCoordSysFactory::k3DCartesian);
	}
}



/*! 
		\throws Throws a runtime_error if NAM file was not found or is not readable.
*/
void  TLGCApp::readFilesParams()
{
	std::ifstream fNamFileStream(fNamFile);

	//If file exists and is readable.
	if (fNamFileStream.good())
	{
		string inputFileName;
		std::getline(fNamFileStream, inputFileName);
		string inputFileExt;
		std::getline(fNamFileStream, inputFileExt);

		// store the full path and filename
		finfileLoc =  inputFileName + "." + inputFileExt;

		string outputFileName;
		std::getline(fNamFileStream, outputFileName);
		string outputFileExt;
		std::getline(fNamFileStream, outputFileExt);	
		foutfileLoc = outputFileName + "." + outputFileExt;
	    floggerfileLoc = outputFileName + "Log" + ".log";
	}
	else{
		throw(std::runtime_error (fNamFile + "was not found or is not readable!"));
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////General information stuff
///////////////////////////////////////////////////////////////////////////////////////////////
// return the software version id string
const string TLGCApp::getProgId()
{
	std::stringstream id;
	id << "LGC2 " << getLGCVersion() << ", version was not compiled against SurveyLib," << " compiled on " <<   __DATE__ ;
	return id.str();
}


// return the software copyright string
const string TLGCApp::getCopyright()
{
	return fCopyright;
}

