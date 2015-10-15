#ifndef _LGCAPP_H_
#define _LGCAPP_H_

#include "TLGCData.h"
#include "TAStreamFormatter.h"

//! Class for a LGC application : file reading, analyzing data, running LS calculation and results writing
class TLGCApp {
	public:
		/*!
			Creates a new TLGCApp instance for a given nam file

			@param namfile[in] Location of the nam file to be read. 

		*/
		TLGCApp(const std::string& namfile = "c:/temp/file.nam");
		
		/*!
			Creates a new TLGCApp instance with a given individual parameters 
			which would otherwise be entries in the nam file.
			
			@param infileLocation[in] Location of the LGC file including the extension, e.g. C:/temp/net.lgc
			@param outfileBasename[in] Location of the LGC result files WITHOUT the extension, e.g. C:/temp/net
			@param outfileExtension[in] Extension of the main LGC result file WITHOUT a dot. The default ist 'out'

		*/
		TLGCApp(const std::string& infileLocation, 
				const std::string& outfileBasename, 
				const std::string& outfileExtension = "out");

		~TLGCApp();

		/*! return the software id.*/
		static const string getProgId();

		/*! return the software copyright*/
		static const string getCopyright();

		///Executes the TLGCApp
		int exec();
	private:
		static const string		fCopyright;

		// Neither copyable nor assignable
		TLGCApp(const TLGCApp& x);
		TLGCApp& operator=(const TLGCApp& x);

		std::string finfileLoc;
		std::string foutfileLoc; 
		std::string floggerfileLoc; 

		std::string fNamFile;

		//TLGCData fProjectData;

		void readFilesParams();

		void saveLSResults(TLGCData *dat);

		void	writeStdResultsFile(TLGCData *dat);

		void makeSaveStream(TLGCData* dat, string separator, TAStreamFormatter::ETextFormat textFormat);

		TAStreamFormatter*		fStream;
};

#endif