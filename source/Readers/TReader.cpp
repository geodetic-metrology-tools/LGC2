#include "AdjObjectsReader.h"
#include "InstrumentReaders.h"
#include "MeasurementReaders.h"
#include "MeasurementReader_lgc1.h"
#include "OptionReaders.h"
#include "TReader.h"


namespace {
	static inline bool isDelim(const char c, const char* delims, int ndelims) {
		for (int i = 0; i < ndelims; i++) {
			if (c == delims[i]) return true;
		}
		return false;
	}

	static inline void skipBOM(std::istream& stream) {
	static const int NUM_BOMs(3);
	static const int BOM_LEN(3);

	// all widespread byte order marks
	static const int boms[NUM_BOMs][BOM_LEN] = {
		{0xEF, 0xBB, 0xBF}, // UTF8
		{0xFE, 0xFF, 0x00}, // UTF16 (LE)
		{0xFF, 0xFE, 0x00}, // UTF16 (LE)
	};
	
	char y;
	for (int b=0; b < NUM_BOMs; b++) {
		for (int c=0; c < BOM_LEN; c++) {
			int x = stream.peek();
			// if the current character is a BOM character: remove it.
			if (x == boms[b][c])
				stream.get(y);
		}	
	}
}
}

// tokenizes a line by removing delimiters and stores non-delimiter words as separate strings.
// The asterisk is considered to be a separate token and is thus stored in a different field as the keyword itself.
// Parsing stops on a comment sign, the comment is then stored as a single token including the comment character.
std::vector<std::string> const TReader::tokenizeLGCfileString(const std::string& str) {
		using namespace std;
		enum {
			STATE_TOKEN,
			STATE_DELIM
		};
		
		size_t delimlen(strlen(INPUT_SEPERATOR_CHARS));
		size_t commentslen(strlen(INPUT_COMMENT_CHARS));
		vector<string> result(0);
	
		bool inString(false);
		size_t start(0);
		size_t end(0);
		size_t length(str.length());
		int state(STATE_DELIM);

		do {
			if (state == STATE_DELIM) {
				if (! isDelim(str[end], INPUT_SEPERATOR_CHARS, (int)delimlen+1)) {
					start = end;
					state = STATE_TOKEN;
				}
			}
			if (state == STATE_TOKEN) {
				if (!inString&& str[end] == '\"') 
					inString = true;
				else if (inString&& str[end] == '\"') 
					inString = false;
				// the nul-character is a delimiter: reason for delimlen+1
            if(!inString && isDelim(str[end], INPUT_SEPERATOR_CHARS, (int)delimlen + 1)) {
					result.push_back(std::move(str.substr(start, end-start)));
					state = STATE_DELIM;
				}
				// this is the beginning of a keyword, keep it as an extra token
				if (str[end] == '*') {
					result.push_back("*");
					start = end+1;
				}
			}
			if (state == STATE_TOKEN || state == STATE_DELIM) {
				// check for comment
            if(isDelim(str[end], INPUT_COMMENT_CHARS, (int)commentslen)) {
					start = end;
					end = length;
					result.push_back(std::move(str.substr(start, end-start)));
				}
			}
			end++;
		} while (str[end-1] != 0);

		return result;
}

TReader::TReader(std::shared_ptr<TLGCData> proj):
	project(*proj.get()){
	typedef std::unique_ptr<TAKeyWord> UPK; // Unique pointer (to) Keyword object
	
	// Options Section

	// reference frames
	finterpreters.emplace_back(UPK(new TKeyOLOC(project)));
	finterpreters.emplace_back(UPK(new TKeyRS2K(project)));
	finterpreters.emplace_back(UPK(new TKeyLEP(project)));
	finterpreters.emplace_back(UPK(new TKeySPHE(project)));

	finterpreters_lgc1.emplace_back(UPK(new TKeyOLOC(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyRS2K(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyLEP(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeySPHE(project)));

	// calculation objects
	finterpreters.emplace_back(UPK(new TKeyALLFIXED(project)));
	finterpreters.emplace_back(UPK(new TKeyLIBR(project)));
	finterpreters.emplace_back(UPK(new TKeyNODUP(project)));
	finterpreters.emplace_back(UPK(new TKeyPDOR(project)));
	finterpreters.emplace_back(UPK(new TKeySIMU(project)));

	finterpreters_lgc1.emplace_back(UPK(new TKeyALLFIXED(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyLIBR(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyNODUP(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyPDOR(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeySIMU(project)));

	// output options
	finterpreters.emplace_back(UPK(new TKeyAPRI(project)));
	finterpreters.emplace_back(UPK(new TKeyEREL(project)));
	finterpreters.emplace_back(UPK(new TKeyFMTO(project)));
	finterpreters.emplace_back(UPK(new TKeyFMTP(project)));
	finterpreters.emplace_back(UPK(new TKeyHIST(project)));
	finterpreters.emplace_back(UPK(new TKeyPREC(project)));
	finterpreters.emplace_back(UPK(new TKeyPRES(project)));

	finterpreters_lgc1.emplace_back(UPK(new TKeyAPRI(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyEREL(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyFMTO(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyFMTP(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyHIST(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyPREC(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyPRES(project)));

	// additional output files
	finterpreters.emplace_back(UPK(new TKeyDEFA(project)));
	finterpreters.emplace_back(UPK(new TKeyFAUT(project)));
	finterpreters.emplace_back(UPK(new TKeyPUNC(project)));
	finterpreters.emplace_back(UPK(new TKeyPLOT(project)));
	finterpreters.emplace_back(UPK(new TKeySOBS(project)));

	finterpreters_lgc1.emplace_back(UPK(new TKeyDEFA(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyFAUT(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyPUNC(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyPLOT(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeySOBS(project)));


	// Adjusted Objects (points) Section
	finterpreters.emplace_back(UPK(new TKeyCALA(project)));
	finterpreters.emplace_back(UPK(new TKeyPOIN(project)));
	finterpreters.emplace_back(UPK(new TKeyVXY(project)));
	finterpreters.emplace_back(UPK(new TKeyVXZ(project)));
	finterpreters.emplace_back(UPK(new TKeyVYZ(project)));
	finterpreters.emplace_back(UPK(new TKeyVZ(project)));

	finterpreters_lgc1.emplace_back(UPK(new TKeyCALA(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyPOIN(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyVXY(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyVXZ(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyVYZ(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyVZ(project)));

		
	// Instruments Section
	//no instrument define in lgc1
	finterpreters.emplace_back(UPK(new TKeyINSTR(project)));
	finterpreters.emplace_back(UPK(new TKeyPOLAR(project)));
	finterpreters.emplace_back(UPK(new TKeyCAMD(project)));
	finterpreters.emplace_back(UPK(new TKeyEDM(project)));
	finterpreters.emplace_back(UPK(new TKeyLEVEL(project)));
	finterpreters.emplace_back(UPK(new TKeySCALE(project)));


	// Observations Section	
	/*TSTN*/
	TAKeyWord* tstn  = new TKeyTSTN(project);
	TAKeyWord* v0    = new TKeyV0(project);
	TAKeyWord* angl  = new TKeyANGL(project);
	TAKeyWord* zend  = new TKeyZEND(project);
	TAKeyWord* dist  = new TKeyDIST(project);
	TAKeyWord* ecth  = new TKeyECTH(project);
	TAKeyWord* dhor  = new TKeyDHOR(project);
	TAKeyWord* plr3d = new TKeyPLR3D(project);

	/*add keywords to interpreters*/
	finterpreters.emplace_back(UPK(tstn));
	finterpreters.emplace_back(UPK(v0));
	finterpreters.emplace_back(UPK(angl));
	finterpreters.emplace_back(UPK(zend));
	finterpreters.emplace_back(UPK(dist));
	finterpreters.emplace_back(UPK(ecth));
	finterpreters.emplace_back(UPK(dhor));
	finterpreters.emplace_back(UPK(plr3d));

	finterpreters_lgc1.emplace_back(UPK(new TKeyANGL_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyZENI_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyZENH_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyDTHE_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyECTH_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyDHOR_lgc1(project)));


	/*CAMERA*/
	TAKeyWord* cam  = new TKeyCAM(project);
	TAKeyWord* uvec2dCam = new TKeyUVEC(project);
	TAKeyWord* uvd3dCam = new TKeyUVD(project);

	/*set hierarchical structure*/
	cam->addChild(uvec2dCam);
	cam->addChild(uvd3dCam);
	uvec2dCam->setParent(cam);
	uvd3dCam->setParent(cam);
	/*add keywords to interpreters*/
	finterpreters.emplace_back(UPK(cam));
	finterpreters.emplace_back(UPK(uvec2dCam));
	finterpreters.emplace_back(UPK(uvd3dCam));

	/*Other observations*/
	finterpreters.emplace_back(UPK(new TKeyDSPT(project)));
	finterpreters.emplace_back(UPK(new TKeyDVER(project)));
	finterpreters.emplace_back(UPK(new TKeyDLEV(project)));
	finterpreters.emplace_back(UPK(new TKeyECHO(project)));
	finterpreters.emplace_back(UPK(new TKeyECSP(project)));
	finterpreters.emplace_back(UPK(new TKeyECVE(project)));
	finterpreters.emplace_back(UPK(new TKeyORIE(project)));
	finterpreters.emplace_back(UPK(new TKeyRADI(project)));

	finterpreters_lgc1.emplace_back(UPK(new TKeyDMES_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyDVER_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyDLEV_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyECHO_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyECSP_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyECVE_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyORIE_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyRADI_lgc1(project)));
	
	finterpreters.emplace_back(UPK(new TKeyFRAME(project)));
	finterpreters.emplace_back(UPK(new TKeyENDFRAME(project)));
	finterpreters.emplace_back(UPK(new TKeyTITR(project)));

	finterpreters_lgc1.emplace_back(UPK(new TKeyTITR(project)));

}


// This function handles all three line endings ("\r", "\n" and "\r\n") and avoid problems when input files are created in a plateform and used in another:
std::istream& safeGetline(std::istream& is, std::string& t)
{
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return is;
        case '\r':
            if(sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
        case EOF:
            // Also handle the case when the last line has no line ending
            if(t.empty())
                is.setstate(std::ios::eofbit);
            return is;
        default:

			// Avoid double * case
			if( !(c==42 && t=="*"))
				t += (char)c;
        }
    }
}

//fOutFilename is the output file used for writing results, possibly can be used for writing errors, might be better to create separate log file for errors and warnings
bool TReader::read(std::istream& lgcStream) {
	using namespace std;
	string line;
	int nline(1);
	bool isReferenceSystemDefined = false;

	auto& outputMessages(project.getFileLogger());
	outputMessages.writeReportHeader("Reading input file:");

	// be sure to omit the byte order mark if there is one
	skipBOM(lgcStream);

	// read the first line of the file
	safeGetline(lgcStream, line);
	const auto& titlrline(tokenizeLGCfileString(line));
	// It must start with *TITR
	// Write error message into an ouput file instead of throwing exception
	if (titlrline.size() != 2)
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "An LGC input file must start with *TITR. The actual title must start on the next line.";
	else if (titlrline[1] != "TITR")
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "An LGC input file must start with *TITR. The actual title must start on the next line.";
			
	// read until the next keyword
	safeGetline(lgcStream, line/*, '*'*/);
	// store the read title in the config
	project.getConfig().title = line;
	// restore the asterisk that was gobbled up by safeGetline
	lgcStream.putback('*');

	// add the newline characters in the title to the linecount
	nline += (int)count(line.cbegin(), line.cend(), '\n');
	nline++;
 
	// read the rest of the file
	auto lasthandler(finterpreters.back().get());
	for (auto currenthandler(lasthandler);
		lgcStream.good() && safeGetline(lgcStream, line) && (line != "*END" && line != "*FIN");
		++nline) 
	{
		// Prepare the error message for this line
		const string nlinestr("Line " + to_string(nline) + ": ");
		// tokenize the current line
		auto tokLine(tokenizeLGCfileString(line));

		// skip empty lines
		if (tokLine.empty()) continue;
		// % means comment line, i.e. to be ignored
		if (tokLine[0][0] == *"%"){
			continue;
		}

		// This means that it is the last keyword, which actually ends the reading process.
		if (tokLine[0] == "*" && (tokLine[1] == "END" || tokLine[1] == "FIN"))
			break;

		// If the line starts with a keyword
		if (tokLine[0] == "*")  {
			const auto& currentkey(tokLine[1]);
			lasthandler = currenthandler;
			currenthandler = nullptr;

			// look for an appropriate handler
			for (auto& handler : finterpreters) {
				// if the handler matches the keyword: store it as the current handler
				if (handler.get()->key == currentkey) {
					currenthandler = handler.get();
					break;
				}
			}

			if (currentkey == OLOC || currentkey == RS2K || currentkey == LEP || currentkey == SPHE)
				isReferenceSystemDefined = true;

			try {

				// abort if there is no valid handler
				if (!currenthandler){
					outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + "Cannot handle keyword \"" + currentkey + ", at line " + line;
					continue;
				}

				// If the keyword in not allowed after the last handler
				if (!lasthandler->isKeyWordAllowed(currenthandler->getKey()))
					throw std::runtime_error("The keyword \"" + currentkey + "\" is not allowed here.");
				else // Particular case : TSTN authorized after ENDFRAME if all frames are closed
					if (currenthandler->getKey() == TSTN && (TKeyFRAME::getNumberOfOpenedFrames() != TKeyENDFRAME::getNumberOfClosedFrames()))
						throw std::runtime_error("TSTN keyword is not allowed in a frame ");
			}
			catch (std::exception const& excp) {
				outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + excp.what();
			}
		}


		try{ //Handler was found, try to parse
			currenthandler->parse(tokLine, nline);
		}
		catch (std::exception const & excp) {  // Catch exceptions which can emerge during parsing
			outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + excp.what();
		}
	}

	

	if(!isReferenceSystemDefined) {
		// Define OLOC as default
		project.getConfig().referential = TRefSystemFactory::ERefFrame::kLocalRefFrame;

		outputMessages << TFileLogger::e_logType::LOG_WARNING << "Reference System hasn't been provided between OLOC, RS2K, LEP & SPHE. It will be OLOC by default";
	}

	if(TKeyFRAME::getNumberOfOpenedFrames() != TKeyENDFRAME::getNumberOfClosedFrames())
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "The number of opened frames (*FRAME) and closed frames (*ENDFRAME) must be equal!";

	return !outputMessages.hasErrors();

}


bool TReader::readLgc1File(std::istream& lgcStream)
{
	using namespace std;
	string line;
	int nline(1);
	bool isReferenceSystemDefined = false;
	bool isLgc1 = true;

	auto& outputMessages(project.getFileLogger());
	outputMessages.writeReportHeader("Reading input file:");

	// be sure to omit the byte order mark if there is one
	skipBOM(lgcStream);

	// read the first line of the file
	safeGetline(lgcStream, line);
	const auto& titlrline(tokenizeLGCfileString(line));
	// It must start with *TITR
	// Write error message into an ouput file instead of throwing exception
	if (titlrline.size() != 2)
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "An LGC input file must start with *TITR. The actual title must start on the next line.";
	else if (titlrline[1] != "TITR")
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "An LGC input file must start with *TITR. The actual title must start on the next line.";

	// read until the next keyword
	safeGetline(lgcStream, line/*, '*'*/);
	// store the read title in the config
	project.getConfig().title = line;
	// restore the asterisk that was gobbled up by safeGetline
	lgcStream.putback('*');

	// add the newline characters in the title to the linecount
	nline += (int)count(line.cbegin(), line.cend(), '\n');
	nline++;

	// read the rest of the file
	auto lasthandler(finterpreters_lgc1.back().get());
	for (auto currenthandler(lasthandler);
		lgcStream.good() && safeGetline(lgcStream, line) && (line != "*END" && line != "*FIN");
		++nline)
	{
		// Prepare the error message for this line
		const string nlinestr("Line " + to_string(nline) + ": ");
		// tokenize the current line
		auto tokLine(tokenizeLGCfileString(line));

		// skip empty lines
		if (tokLine.empty()) continue;
		// % means comment line, i.e. to be ignored
		if (tokLine[0][0] == *"%"){
			continue;
		}

		// This means that it is the last keyword, which actually ends the reading process.
		if (tokLine[0] == "*" && (tokLine[1] == "END" || tokLine[1] == "FIN"))
			break;

		// If the line starts with a keyword
		if (tokLine[0] == "*")  
		{
			const auto& currentkey(tokLine[1]);
			lasthandler = currenthandler;
			currenthandler = nullptr;

			// look for an appropriate handler
			for (auto& handler : finterpreters_lgc1) {
				// if the handler matches the keyword: store it as the current handler
				if (handler.get()->key == currentkey) {
					currenthandler = handler.get();
					break;
				}
			}

			if (currentkey == OLOC || currentkey == RS2K || currentkey == LEP || currentkey == SPHE)
				isReferenceSystemDefined = true;

			try {
				// abort if there is no valid handler
				if (!currenthandler){
					outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + "Cannot handle keyword \"" + currentkey + ", at line " + line;
					continue;
				}

			}
			catch (std::exception const& excp)
			{
				outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + excp.what();
			}
		}


		try{ //Handler was found, try to parse
			currenthandler->parse(tokLine, nline);
		}
		catch (std::exception const & excp) 
		{  // Catch exceptions which can emerge during parsing
			outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + excp.what();
		}
	}

	if (!isReferenceSystemDefined)
		outputMessages << TFileLogger::e_logType::LOG_WARNING << "Reference System hasn't been provided between OLOC, RS2K, LEP & SPHE. It will be OLOC by default";

	return !outputMessages.hasErrors();
}

bool TReader::isLgc2File(std::istream& lgcStream)
{
	string line;
	int nline(1);

	// be sure to omit the byte order mark if there is one
	skipBOM(lgcStream);

	// read the first line of the file
	safeGetline(lgcStream, line);
	const auto& titlrline(tokenizeLGCfileString(line));

	// read until the next keyword
	safeGetline(lgcStream, line/*, '*'*/);

	// restore the asterisk that was gobbled up by safeGetline
	lgcStream.putback('*');

	// add the newline characters in the title to the linecount
	nline += (int)count(line.cbegin(), line.cend(), '\n');
	nline++;


	auto lasthandler(finterpreters.back().get());
	for (auto currenthandler(lasthandler);
		lgcStream.good() && safeGetline(lgcStream, line) && (line != "*END" && line != "*FIN");
		++nline) 
	{
		// tokenize the current line
		auto tokLine(tokenizeLGCfileString(line));

		// skip empty lines
		if (tokLine.empty()) continue;

		// % means comment line, i.e. to be ignored
		if (tokLine[0][0] == *"%") continue;

		// If the line starts with a keyword
		if (tokLine[0] == "*")
		{
			const auto& currentkey(tokLine[1]);

			if (currentkey == INSTR)
				return true;
		}

	}
	return false;
}
