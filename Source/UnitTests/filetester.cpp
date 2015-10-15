#include <string>
#include <cstring>
#include <fstream>
#include <direct.h>
#include <process.h>
#include <windows.h>
#include <stdexcept>

#include <TAConverter.h>
#include <TLGCApplication.h>

#include <FileTester.h>

using namespace std;

namespace {
	struct TATestItem {
		string ffilename;
		string ftesterror;
		bool fcorrectSection;
		bool fcorrectLine;
		bool ftestPassed;
		bool ftestFinished;

		TATestItem(const string& fn) :
			ffilename(fn),
			ftesterror(""),
			fcorrectSection(false),
			fcorrectLine(false),
			ftestPassed(false),
			ftestFinished(false) {}

		virtual void testLine(const string& line) = 0;
	};
	
	struct TTestAtlRES_101210 : public TATestItem  {
		// call parent constructor
		TTestAtlRES_101210() : TATestItem("AtlRES_101210.out") {
			fcorrectSection = true;
		}

		virtual void testLine(const string& line) {
			static const int POINT_LEN(512);
			char pointID[POINT_LEN];
			float sx, sy, sz;

			//  Read the beginning of the result: PointID X Y Z SX SY SZ
			int argsRead = sscanf(line.c_str(), "%512s %*f %*f %*f %f %f %f", pointID, &sx, &sy, &sz);

			// Must have read all 4 arguments
			if (argsRead != 4) return;
			// must be one of the two points below
			if (strcmp(pointID, "TRUPSC") != 0 && strcmp(pointID, "TRUPSA") != 0) return;

			// all standard deviations must be gerater that 0.1
			if (sx > 0.1 && sy > 0.1 && sz > 0.1) {
				ftestPassed = true;
			}
			else {
				ftesterror = "Wrong standard deviations for point " + string(pointID);
			}

			ftestFinished = true;
		}
	};

	struct TTestDVERDLEV : public TATestItem  {
		// call parent constructor
		TTestDVERDLEV(const std::string& file) : TATestItem(file) {
			fcorrectSection = false;
		}

		virtual void testLine(const string& line) {
			static const int POINT_LEN(512);
			char pointID[POINT_LEN];
			float dz;

			// read point name of the current line
			int argsRead = sscanf(line.c_str(), "%512s", pointID);

			// This is the first variable point, start checking
			if (argsRead == 1 && strcmp(pointID, "LS4") == 0)
				fcorrectSection = true;
			// This is the last variable point, stop checking
			else if (argsRead == 1 && strcmp(pointID, "P220") == 0) {
				ftestFinished   = true;
				fcorrectSection = false;
			}


			if (fcorrectSection) {
				//  Read the result: PointID X Y Z H SZ DZ
				argsRead = sscanf(line.c_str(), "%*512s %*f %*f %*f %*f %*f %f", &dz);

				// Must have read dz
				if (argsRead != 1) return;

				// movement must be very close to zero
				if (fabsf(dz) < 1e-6) {
					ftestPassed = true;
				}
				else {
					ftesterror = "Point in leveling test moved, but shouldnt have: " + string(pointID);
				}
			}
		}
	};

	
	
	struct TTestAllMeasSim : public TATestItem  {
		// call parent constructor
		TTestAllMeasSim() : TATestItem("lgcin_AllMeas.err") {
			fcorrectSection = true;
		}
		
		virtual void testLine(const string& line) {
			TStrTokens tokens(TAConverter::tokenizeString(line, " \t"));

			if (tokens.size() > 0 && tokens[0] == "STATION")
			{
				fcorrectSection = true;
				return;
			}

			if (! fcorrectSection)
				return;

			switch (tokens.size())
			{
				// This is the expected case for a blunder line
				// Do a sanity check of the line
				case 13:
					if ( !( (tokens[6] == "!" || tokens[6] == "**") &&
						     fabsq(stod(tokens[7])) > 2.5 &&
						     tokens[8] == "**" ) )
					{ // The content seems wrong
						ftesterror == "Broken blunder detection.";
					}
					break;			
			}
		}
	};
}


void FileTester::runTestFiles() {
	static const int PATH_LEN(4096);
	static char projectroot[PATH_LEN];
	string bindir;
	vector<string> testfiles;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	// get the directory of the executable
	if (GetModuleFileName(0, projectroot, PATH_LEN) == 0)
		// NULL indicates an error
		throw runtime_error("Could not retrive path of the testing executable.");
	// steup up to root directory
	for (int i = 0; i < 4; i++) {
		*(strrchr(projectroot,'\\')) = 0;
		if (i == 2) bindir = projectroot;
	}
	
	// create the (empty) output directory
	string indir(string(projectroot) + string("\\testfiles"));
	string outdir(string(projectroot) + string("\\testoutfiles"));
	_unlink(outdir.c_str());
	if (_mkdir(outdir.c_str()) != 0) {
		if (errno == ENOENT)
			throw runtime_error("Output file path " + outdir  + "not found");
	}

	// location of lgc executable
#ifdef _DEBUG
	string exesubdir("\\lgc++\\Debug\\");
#else
	string exesubdir("\\lgc++\\Release\\");
#endif
	string lgcCommand(bindir + exesubdir + string("lgc.exe > NUL"));

	// create list of testfilenames 
	hFind = FindFirstFile((indir + "\\*.inp").c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// truncate the extension .inp
			*(strrchr(FindFileData.cFileName, '.')) = 0;
			testfiles.push_back(FindFileData.cFileName);
		} 
		while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
	else {
		throw runtime_error("No Test Files in" + indir);
	}
#if 0
	int rc;
	for (size_t i = 0; i < testfiles.size(); i++) {
		// create LGC's configuration file
		ofstream namfile("C:/TEMP/file.nam");
		namfile << projectroot << "\\testfiles\\"  << testfiles[i] << endl << "inp" << endl;
		namfile << projectroot << "\\testoutfiles\\" << testfiles[i] << endl << "out" << endl;
		namfile << projectroot << "\\testoutfiles\\" << testfiles[i] << endl;

		// launch lgc with the current nam-file
		rc = system(lgcCommand.c_str());
		
		if (rc != 0) {
			throw runtime_error("!! Error in testfiles\\" + testfiles[i] + ".inp !!\n");
		}
		else {
			cout << "Completed testfiles\\" << testfiles[i] << ".inp" << endl;
		}
	}
	
#endif
	// initialize test objects
	//
	// ADD instances of new classes HERE
	vector <TATestItem*> tests;
	tests.push_back(new TTestAtlRES_101210());
	tests.push_back(new TTestDVERDLEV("LevStnAll_2_XYHs_SPHE_DVER_90_Precise7DecPl.out"));
	tests.push_back(new TTestDVERDLEV("LevStnAll_2_XYHs_SPHE_DLEV_90.out"));
	tests.push_back(new TTestAllMeasSim());
	
	// read created output files
	testfiles.clear();
	hFind = FindFirstFile((outdir + "\\*.*").c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			testfiles.push_back(FindFileData.cFileName);
		} 
		while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
	else {
		throw runtime_error("No Test Files in" + indir);
	}
	// loop over result files and match against test objects
	cout << "\n\nAnalyzing output files...\n";
	for (size_t fileidx = 0; fileidx < testfiles.size(); fileidx++) {
		for (size_t testidx = 0; testidx < tests.size(); testidx++) {
			TATestItem* curTest(tests[testidx]);
			if (curTest->ffilename == testfiles[fileidx]) {
				// This testobject matches the current file
				string outfilestr(outdir + "\\" + testfiles[fileidx]);
				cout << outfilestr << "\n";
				ifstream output(outfilestr);
				if (! output.is_open())
					 throw runtime_error("Output file " + outfilestr + " not found");
				// step through the lines of the output file
				for (string line; getline(output, line); ) {
					curTest->testLine(line);
					// abort if the test is finished
					if (curTest->ftestFinished || !curTest->ftestPassed)
						break;
				}
				if (!curTest->ftestPassed)
					throw logic_error("Error in file " + testfiles[fileidx] + ": " + curTest->ftesterror);
			}
		}
	}
		
	cout << "\n!!\nTests completed successfully\n!!\n";

}