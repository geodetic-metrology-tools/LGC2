// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TReader.h"

#include <unordered_map>
#include <StringManager.h>

#include "AdjObjectsReader.h"
#include "InstrumentReaders.h"
#include "MeasurementReader_lgc1.h"
#include "MeasurementReaders.h"
#include "OptionReaders.h"
#include "TReader.h"
#include "SagConstraintReader.h"

namespace
{
static inline void skipBOM(std::istream &stream)
{
	static const int NUM_BOMs(3);
	static const int BOM_LEN(3);

	// all widespread byte order marks
	static const int boms[NUM_BOMs][BOM_LEN] = {
		{0xEF, 0xBB, 0xBF}, // UTF8
		{0xFE, 0xFF, 0x00}, // UTF16 (LE)
		{0xFF, 0xFE, 0x00}, // UTF16 (LE)
	};

	char y;
	for (int b = 0; b < NUM_BOMs; b++)
	{
		for (int c = 0; c < BOM_LEN; c++)
		{
			int x = stream.peek();
			// if the current character is a BOM character: remove it.
			if (x == boms[b][c])
				stream.get(y);
		}
	}
}
} // namespace

TReader::TReader(std::shared_ptr<TLGCData> proj) : project(*proj.get())
{
	typedef std::unique_ptr<TAKeyWord> UPK; // Unique pointer (to) Keyword object

	// Options Section

	// reference frames
	finterpreters.emplace_back(UPK(new TKeyOLOC(project)));
	finterpreters.emplace_back(UPK(new TKeyRS2K(project)));
	finterpreters.emplace_back(UPK(new TKeyLEP(project)));
	finterpreters.emplace_back(UPK(new TKeySPHE(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyOLOC(project, nb_allowed_oloc_lgc1, allowed_OLOC_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyRS2K(project, nb_allowed_rs2k_lgc1, allowed_RS2K_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyLEP(project, nb_allowed_lep_lgc1, allowed_LEP_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeySPHE(project, nb_allowed_sphe_lgc1, allowed_SPHE_lgc1)));

	// calculation objects
	finterpreters.emplace_back(UPK(new TKeyALLFIXED(project)));
	finterpreters.emplace_back(UPK(new TKeyLIBR(project)));
	finterpreters.emplace_back(UPK(new TKeyNODUP(project)));
	finterpreters.emplace_back(UPK(new TKeyPDOR(project)));
	finterpreters.emplace_back(UPK(new TKeySIMU(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyALLFIXED(project, nb_allowed_allfixed_lgc1, allowed_ALLFIXED_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyLIBR(project, nb_allowed_libr_lgc1, allowed_LIBR_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyNODUP(project, nb_allowed_nodup_lgc1, allowed_NODUP_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyPDOR(project, nb_allowed_pdor_lgc1, allowed_PDOR_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeySIMU(project, nb_allowed_simu_lgc1, allowed_SIMU_lgc1)));

	// output options
	finterpreters.emplace_back(UPK(new TKeyAPRI(project)));
	finterpreters.emplace_back(UPK(new TKeyEREL(project)));
	finterpreters.emplace_back(UPK(new TKeyERELFRAME(project)));
	finterpreters.emplace_back(UPK(new TKeyFMTO(project)));
	finterpreters.emplace_back(UPK(new TKeyFMTP(project)));
	finterpreters.emplace_back(UPK(new TKeyHIST(project)));
	finterpreters.emplace_back(UPK(new TKeyPREC(project)));
	finterpreters.emplace_back(UPK(new TKeyPRES(project)));
	finterpreters.emplace_back(UPK(new TKeyCOVAR(project)));
	finterpreters.emplace_back(UPK(new TKeyCHABA(project)));
	finterpreters.emplace_back(UPK(new TKeyCONSI(project)));
	finterpreters.emplace_back(UPK(new TKeyLM(project)));
	finterpreters.emplace_back(UPK(new TKeyJSON(project)));

	finterpreters_lgc1.emplace_back(UPK(new TKeyAPRI(project, nb_allowed_apri_lgc1, allowed_APRI_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyEREL(project, nb_allowed_erel_lgc1, allowed_EREL_lgc1)));
	// finterpreters_lgc1.emplace_back(UPK(new TKeyFMTO(project, nb_allowed_fmto_lgc1, allowed_FMTO_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyFMTP(project, nb_allowed_fmtp_lgc1, allowed_FMTP_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyHIST(project, nb_allowed_hist_lgc1, allowed_HIST_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyPREC(project, nb_allowed_prec_lgc1, allowed_PREC_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyMICR(project, nb_allowed_micr_lgc1, allowed_MICR_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyCLIC(project, nb_allowed_clic_lgc1, allowed_CLIC_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyNOSPC(project, nb_allowed_nospc_lgc1, allowed_NOSPC_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyPRES(project, nb_allowed_pres_lgc1, allowed_PRES_lgc1)));

	// additional output files
	finterpreters.emplace_back(UPK(new TKeyDEFA(project)));
	finterpreters.emplace_back(UPK(new TKeyFAUT(project)));
	finterpreters.emplace_back(UPK(new TKeyPUNC(project)));
	finterpreters.emplace_back(UPK(new TKeyPLOT(project)));
	finterpreters.emplace_back(UPK(new TKeySOBS(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyDEFA(project, nb_allowed_defa_lgc1, allowed_DEFA_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyFAUT(project, nb_allowed_faut_lgc1, allowed_FAUT_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyPUNC(project, nb_allowed_punc_lgc1, allowed_PUNC_lgc1)));
	// finterpreters_lgc1.emplace_back(UPK(new TKeyPLOT(project, nb_allowed_plot_lgc1, allowed_PLOT_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeySOBS(project, nb_allowed_sobs_lgc1, allowed_SOBS_lgc1)));

	// Adjusted Objects (points) Section
	finterpreters.emplace_back(UPK(new TKeyCALA(project)));
	finterpreters.emplace_back(UPK(new TKeyPOIN(project)));
	finterpreters.emplace_back(UPK(new TKeyVXY(project)));
	finterpreters.emplace_back(UPK(new TKeyVXZ(project)));
	finterpreters.emplace_back(UPK(new TKeyVYZ(project)));
	finterpreters.emplace_back(UPK(new TKeyVZ(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyCALA(project, nb_allowed_cala_lgc1, allowed_CALA_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyPOIN(project, nb_allowed_poin_lgc1, allowed_POIN_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyVXY(project, nb_allowed_vxy_lgc1, allowed_VXY_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyVXZ(project, nb_allowed_vxz_lgc1, allowed_VXZ_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyVYZ(project, nb_allowed_vyz_lgc1, allowed_VYZ_lgc1)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyVZ(project, nb_allowed_vz_lgc1, allowed_VZ_lgc1)));

	// Instruments Section
	// no instrument define in lgc1
	finterpreters.emplace_back(UPK(new TKeyINSTR(project)));
	finterpreters.emplace_back(UPK(new TKeyPOLAR(project)));
	finterpreters.emplace_back(UPK(new TKeyCAMD(project)));
	finterpreters.emplace_back(UPK(new TKeyEDM(project)));
	finterpreters.emplace_back(UPK(new TKeyLEVEL(project)));
	finterpreters.emplace_back(UPK(new TKeySCALE(project)));
	finterpreters.emplace_back(UPK(new TKeyINCL(project)));
	finterpreters.emplace_back(UPK(new TKeyHLSR(project)));
	finterpreters.emplace_back(UPK(new TKeyWPSR(project)));

	finterpreters.emplace_back(UPK(new TKeySAGELEMENT(project)));
	finterpreters.emplace_back(UPK(new TASagConstraintPairKey(project)));

	// Observations Section
	/*TSTN*/
	TAKeyWord *tstn = new TKeyTSTN(project);
	TAKeyWord *v0 = new TKeyV0(project);
	TAKeyWord *angl = new TKeyANGL(project);
	TAKeyWord *zend = new TKeyZEND(project);
	TAKeyWord *dist = new TKeyDIST(project);
	TAKeyWord *ecth = new TKeyECTH(project);
	TAKeyWord *ecdir = new TKeyECDIR(project);
	TAKeyWord *dhor = new TKeyDHOR(project);
	TAKeyWord *plr3d = new TKeyPLR3D(project);

	/*add keywords to interpreters*/
	finterpreters.emplace_back(UPK(tstn));
	finterpreters.emplace_back(UPK(v0));
	finterpreters.emplace_back(UPK(angl));
	finterpreters.emplace_back(UPK(zend));
	finterpreters.emplace_back(UPK(dist));
	finterpreters.emplace_back(UPK(ecth));
	finterpreters.emplace_back(UPK(ecdir));
	finterpreters.emplace_back(UPK(dhor));
	finterpreters.emplace_back(UPK(plr3d));
	finterpreters_lgc1.emplace_back(UPK(new TKeyANGL_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyZENI_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyZENH_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyDTHE_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyECTH_lgc1(project)));
	finterpreters_lgc1.emplace_back(UPK(new TKeyDHOR_lgc1(project)));

	/*CAMERA*/
	TAKeyWord *cam = new TKeyCAM(project);
	TAKeyWord *uvec2dCam = new TKeyUVEC(project);
	TAKeyWord *uvd3dCam = new TKeyUVD(project);

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
	finterpreters.emplace_back(UPK(new TKeyOBSXYZ(project)));
	finterpreters.emplace_back(UPK(new TKeyINCLY(project)));
	finterpreters.emplace_back(UPK(new TKeyROLLY(project)));
	finterpreters.emplace_back(UPK(new TKeyECWS(project)));
	finterpreters.emplace_back(UPK(new TKeyECWI(project)));
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
std::istream &safeGetline(std::istream &is, std::string &t)
{
	t.clear();

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

	std::istream::sentry se(is, true);
	std::streambuf *sb = is.rdbuf();

	for (;;)
	{
		int c = sb->sbumpc();
		switch (c)
		{
		case '\n':
			return is;
		case '\r':
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case EOF:
			// Also handle the case when the last line has no line ending
			if (t.empty())
				is.setstate(std::ios::eofbit);
			return is;
		default:

			// Avoid double * case
			if (!(c == '*' && t == "*"))
				t += (char)c;
		}
	}
}

// fOutFilename is the output file used for writing results, possibly can be used for writing errors, might be better to create separate log file for errors and warnings
bool TReader::read(std::istream &lgcStream)
{
	using namespace std;
	string line;
	int nline(0);
	bool isReferenceSystemDefined = false;

	auto &outputMessages(project.getFileLogger());
	outputMessages.writeReportHeader("Reading input file:");

	// be sure to omit the byte order mark if there is one
	skipBOM(lgcStream);

	// read the first line of the file
	safeGetline(lgcStream, line);
	nline++;
	const auto &titlrline(tokenizefileString(line));
	// It must start with *TITR
	// Write error message into an ouput file instead of throwing exception
	if (titlrline.size() != 2)
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "Input file is not in the correct LGC format. Issue: Missing *TITR at the start of the file.";
	else if (titlrline[1] != "TITR")
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "Input file is not in the correct LGC format. Issue: Missing *TITR at the start of the file.";

	// read until the next keyword
	safeGetline(lgcStream, line);
	nline++;
	bool titleProvided = false;
	while (line.compare(0, 1, "*"))
	{
		// store the read title in the config
		project.getConfig().title += line + " ";
		titleProvided = true;
		safeGetline(lgcStream, line);
		nline++;
	}
	// check if the title is proper before removing the last added whitespace
	if (titleProvided)
	{
		// Remove the last added whitespace:
		project.getConfig().title.pop_back();
	}
	else
	{ // set a default title
		project.getConfig().title = "Default title";
	}

	// read the rest of the file
	auto lasthandler(finterpreters.back().get());
	for (auto currenthandler(lasthandler); lgcStream.good() && (line != "*END" && line != "*FIN"); ++nline)
	{
		// Prepare the error message for this line
		const string nlinestr("Line " + to_string(nline) + ": ");
		// tokenize the current line
		auto tokLine(tokenizefileString(line));

		// skip empty lines
		if (tokLine.empty())
		{
			safeGetline(lgcStream, line);
			continue;
		}
		// % means comment line, i.e. to be ignored
		if (tokLine[0][0] == *"%")
		{
			project.pushComment(std::pair<int, std::string>(nline, line));
			safeGetline(lgcStream, line);
			continue;
		}

		// Check if the line begins with the deactivation characer, store the activation status
		bool activeLine = true;
		if (tokLine[0][0] == *DEACTIVATION_CHAR)
		{
			activeLine = false;
			// Remove the deactivation character from the beginning of the string:
			tokLine[0].erase(tokLine[0].begin());
		}

		// This means that it is the last keyword, which actually ends the reading process.
		if (tokLine[0] == "*" && (tokLine[1] == "END" || tokLine[1] == "FIN"))
			break;

		// If the line starts with a keyword
		if (tokLine[0] == "*")
		{
			const auto &currentkey(tokLine[1]);
			lasthandler = currenthandler;
			currenthandler = nullptr;

			// look for an appropriate handler
			for (auto &handler : finterpreters)
			{
				// if the handler matches the keyword: store it as the current handler
				if (handler.get()->key == currentkey)
				{
					currenthandler = handler.get();
					break;
				}
			}

			if (currentkey == OLOC || currentkey == RS2K || currentkey == LEP || currentkey == SPHE)
				isReferenceSystemDefined = true;

			try
			{
				// abort if there is no valid handler
				if (!currenthandler)
				{
					outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + "Cannot handle keyword \"" + currentkey + "\"";
					return !outputMessages.hasErrors();
				}

				// If the keyword in not allowed after the last handler
				if (!lasthandler->isKeyWordAllowed(currenthandler->getKey()))
				{
					outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + "The keyword \"" + currentkey + "\" is not allowed here.";
					return !outputMessages.hasErrors();
				}
				// else // Particular case : TSTN authorized after ENDFRAME if all frames are closed
				//	if (currenthandler->getKey() == TSTN && (TKeyFRAME::getNumberOfOpenedFrames() != TKeyENDFRAME::getNumberOfClosedFrames()))
				//	{
				//		outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + "TSTN keyword is not allowed in a frame ";
				//		return !outputMessages.hasErrors();
				//	}
			}
			catch (std::exception const &excp)
			{
				outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + excp.what();
				return !outputMessages.hasErrors();
			}
		}

		try
		{ // Handler was found, try to parse

			currenthandler->parse(tokLine, activeLine, nline);
			safeGetline(lgcStream, line);
		}
		catch (std::exception const &excp)
		{ // Catch exceptions which can emerge during parsing
			outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + excp.what();
			// need return error during reading since the fisrt failure. because if no instrument are defined after *INSTR. When the first instr ID is not found, the software crashs.
			return !outputMessages.hasErrors();
		}
	}

	if (!isReferenceSystemDefined)
	{
		// Define OLOC as default
		project.getConfig().referential = TRefSystemFactory::ERefFrame::kLocalRefFrame;

		outputMessages << TFileLogger::e_logType::LOG_WARNING << "Reference System hasn't been provided between OLOC, RS2K, LEP & SPHE. It will be OLOC by default";
	}

	if (project.getCurrentNode().ID.size() != 1)
	{
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "The number of opened frames (*FRAME) and closed frames (*ENDFRAME) must be equal!";
	}

	// CHECK FRAMES WITH THE NAME "ROOT", DUPLICATE FRAME NAMES AND OBSERVATION ID

	std::vector<std::string> frameNames; // Declare a vector of strings for the names of the frames
	std::vector<int> frameLines; // Declare a vector of integers for the lines of the *FRAME keywords
	std::unordered_map<std::string, int> globalObsIdMap; // Map to track observation IDs across all frames

	// Iterate the frames
	for (TDataTreeIterator itTree = project.getTree().begin(); itTree != project.getTree().end(); itTree++)
	{
		// In each iteration append the frame name and the *FRAME line
		frameNames.push_back(itTree->get()->frame.getName());
		frameLines.push_back(itTree->get()->frame.getLine());

		if (frameNames.size() > 1 && frameNames.back().compare("ROOT") == 0)
		{
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "The frame name \"ROOT\" at line " + to_string(frameLines.back()) + " cannot be used.";
		}

		// Iterate the vector starting from the second item, i.e., the second frame name.
		for (std::size_t i = 2; i < frameNames.size(); ++i)
		{
			if (frameNames.back().compare(frameNames[i - 1]) == 0)
			{
				outputMessages
					<< TFileLogger::e_logType::LOG_ERROR
					<< "Frames at lines " + to_string(frameLines[i - 1]) + " and " + to_string(frameLines.back()) + " have the same name: \"" + frameNames.back() + "\".";
			}
		}

		// Register observation IDs and check for duplicates across all frames
		if (checkAndRegisterObsId(itTree, globalObsIdMap, outputMessages))
			break;
	}

	// Calculate obsIDwidth from all registered observation IDs
	for (const auto& obsIdPair : globalObsIdMap)
	{
		if (obsIdPair.first.size() > project.getConfig().obsIDwidth)
			project.getConfig().obsIDwidth = obsIdPair.first.size();
	}

	// Expand DEFORM directives (must happen after all frames and points are read)
	expandDeformDirectives();

	project.setLGCv1(false);

	return !outputMessages.hasErrors();
}

bool TReader::readLgc1File(std::istream &lgcStream)
{
	using namespace std;
	string line;
	int nline(0);
	bool isReferenceSystemDefined = false;
	// bool isLgc1 = true;

	auto &outputMessages(project.getFileLogger());
	outputMessages.writeReportHeader("Reading input file:");

	// be sure to omit the byte order mark if there is one
	skipBOM(lgcStream);

	// read the first line of the file
	safeGetline(lgcStream, line);
	nline++;
	const auto &titlrline(tokenizefileString(line));
	// It must start with *TITR
	// Write error message into an ouput file instead of throwing exception
	if (titlrline.size() != 2)
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "Input file is not in the correct LGC format. Issue: Missing *TITR at the start of the file.";
	else if (titlrline[1] != "TITR")
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "Input file is not in the correct LGC format. Issue: Missing *TITR at the start of the file.";

	// read until the next keyword
	safeGetline(lgcStream, line);
	nline++;
	while (line.compare(0, 1, "*"))
	{
		// store the read title in the config
		project.getConfig().title += line + ' ';
		safeGetline(lgcStream, line);
		nline++;

		if (lgcStream.peek() == EOF)
			break; // End of file
	}
	// Remove the last added whitespace:
	project.getConfig().title.pop_back();

	// read the rest of the file
	auto lasthandler(finterpreters_lgc1.back().get());
	for (auto currenthandler(lasthandler); lgcStream.good() && (line != "*END" && line != "*FIN"); ++nline)
	{
		// Prepare the error message for this line
		const string nlinestr("Line " + to_string(nline) + ": ");
		// tokenize the current line
		auto tokLine(tokenizefileString(line));

		// skip empty lines
		if (tokLine.empty())
		{
			safeGetline(lgcStream, line);
			continue;
		}
		// % means comment line, i.e. to be ignored
		// The deactivation character behaves as commented line in LGC1 mode
		if (tokLine[0][0] == *"%" || tokLine[0][0] == *DEACTIVATION_CHAR)
		{
			project.getComments()[nline] = line;
			safeGetline(lgcStream, line);
			continue;
		}

		// This means that it is the last keyword, which actually ends the reading process.
		if (tokLine[0] == "*" && (tokLine[1] == "END" || tokLine[1] == "FIN"))
			break;

		// If the line starts with a keyword
		if (tokLine[0] == "*")
		{
			const auto &currentkey(tokLine[1]);
			lasthandler = currenthandler;
			currenthandler = nullptr;

			// look for an appropriate handler
			for (auto &handler : finterpreters_lgc1)
			{
				// if the handler matches the keyword: store it as the current handler
				if (handler.get()->key == currentkey)
				{
					currenthandler = handler.get();
					break;
				}
			}

			if (currentkey == OLOC || currentkey == RS2K || currentkey == LEP || currentkey == SPHE)
				isReferenceSystemDefined = true;

			try
			{
				// abort if there is no valid handler
				if (!currenthandler)
				{
					outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + "Cannot handle keyword \"" + currentkey + "\"";
					// allow to get to the next line, avoid infinite loop
					safeGetline(lgcStream, line);
					continue;
				}
			}
			catch (std::exception const &excp)
			{
				outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + excp.what();
				// allow to get to the next line, avoid infinite loop
				safeGetline(lgcStream, line);
			}
		}

		try
		{ // Handler was found, try to parse
			currenthandler->parse(tokLine, true, nline);
			safeGetline(lgcStream, line);
		}
		catch (std::exception const &excp)
		{ // Catch exceptions which can emerge during parsing
			outputMessages << TFileLogger::e_logType::LOG_ERROR << nlinestr + excp.what();
			// allow to get to the next line, avoid infinite loop
			safeGetline(lgcStream, line);
		}
	}

	if (!isReferenceSystemDefined)
		outputMessages << TFileLogger::e_logType::LOG_WARNING << "Reference System hasn't been provided between OLOC, RS2K, LEP & SPHE. It will be OLOC by default";

	project.setLGCv1(true);

	if (outputMessages.hasErrors())
		return false;

	std::shared_ptr<TInstrumentData::TPOLAR::TTarget> polarDefTgt = project.getInstruments().fPOLAR.empty()
		? nullptr
		: project.getInstruments().fPOLAR.begin()->second->targets.begin()->second;
	std::shared_ptr<TInstrumentData::TEDM::TTarget> edmDefTgt = project.getInstruments().fEDM.empty()
		? nullptr
		: project.getInstruments().fEDM.begin()->second->targets.at(project.getInstruments().fEDM.begin()->second->defTarget);
	std::shared_ptr<TInstrumentData::TEDM::TTarget> edmAdjTgt = project.getInstruments().fEDM.size() == 2
		? project.getInstruments().fEDM.begin()->second->targets.at("EDMAdjTgt")
		: nullptr;
	std::shared_ptr<TInstrumentData::TLEVEL::TTarget> defStaff = project.getInstruments().fLEVEL.empty()
		? nullptr
		: project.getInstruments().fLEVEL.begin()->second->targets.begin()->second;

	// Update the targets in instruments stored in stations for data consistency

	if (polarDefTgt || edmDefTgt || defStaff)
	{
		for (auto &node : project.getTree())
		{
			for (auto &tstn : node->measurements.fTSTN)
				*tstn->instrument.targets.begin()->second = *polarDefTgt;

			for (auto &edm : node->measurements.fEDM)
			{
				*edm.instrument.targets.at(edmDefTgt->ID) = *edmDefTgt;

				// Copy the adjustable target to the station if it exists:
				if (edmAdjTgt)
					*edm.instrument.targets[edmAdjTgt->ID] = *edmAdjTgt;
			}

			for (auto &level : node->measurements.fLEVEL)
				*level.instrument.targets.begin()->second = *defStaff;

			for (auto &orierom : node->measurements.fORIE)
				*orierom.instrument.targets.begin()->second = *polarDefTgt;
		}
	}

	return true;
}

// Check if the given file is in LGC2 format (i.e., it contains the *INSTR keyword)
bool TReader::isLgc2File(std::istream &lgcStream)
{
	std::istream::pos_type originalPos = lgcStream.tellg(); // Save position
	// be sure to omit the byte order mark if there is one
	skipBOM(lgcStream);

	for (std::string line; lgcStream.good() && safeGetline(lgcStream, line);)
	{
		// tokenize the current line
		auto tokLine(tokenizefileString(line));

		// skip empty lines
		if (tokLine.empty())
			continue;

		// If the line starts with a keyword
		if (tokLine[0] == "*" && (tokLine[1] == INSTR || tokLine[1] == CHABA))
		{
			lgcStream.seekg(originalPos); // Reset position
			return true;
		}
	}
	lgcStream.seekg(originalPos); // Reset position
	return false;
}

bool TReader::expandDeformDirectives()
{
	auto &outputMessages(project.getFileLogger());

	// Phase A: Collect frame-level directives and validate no nesting
	struct FrameDeform
	{
		TDataTreeIterator position;
		std::string sagElementName;
		int line;
	};

	// Helper: check if 'node' is in the subtree rooted at 'root' (inclusive of root itself)
	// by walking up the parent chain from node
	auto isInSubtreeOf = [&](const TDataTreeIterator &node, const TDataTreeIterator &root) -> bool
	{
		if (node == root)
			return true;
		auto current = node;
		while (current.node != nullptr && current.node->parent != nullptr)
		{
			current = project.getTree().parent(current);
			if (current == root)
				return true;
		}
		return false;
	};

	std::vector<FrameDeform> frameDeforms;
	for (auto it = project.getTree().begin(); it != project.getTree().end(); ++it)
	{
		if (!it->get()->deformSagElementName.empty())
			frameDeforms.push_back({it, it->get()->deformSagElementName, it->get()->deformLine});
	}

	for (size_t i = 0; i < frameDeforms.size(); ++i)
	{
		for (size_t j = i + 1; j < frameDeforms.size(); ++j)
		{
			if (isInSubtreeOf(frameDeforms[i].position, frameDeforms[j].position) ||
				isInSubtreeOf(frameDeforms[j].position, frameDeforms[i].position))
			{
				std::string nameI = frameDeforms[i].position.node->data.get()->frame.getName();
				std::string nameJ = frameDeforms[j].position.node->data.get()->frame.getName();
				outputMessages << TFileLogger::e_logType::LOG_ERROR
							   << "Line " + std::to_string(frameDeforms[j].line) + ": DEFORM in frame '" + nameJ + "' overlaps with DEFORM in frame '" + nameI + "' (nested deforms are not allowed).";
				return false;
			}
		}
	}

	// Phase B: Validate no overlap between frame-level and point-level deforms
	for (const auto &point : project.getPoints())
	{
		if (point.getDeformSagElement().empty())
			continue;
		for (const auto &fd : frameDeforms)
		{
			if (isInSubtreeOf(point.getFrameTreePosition(), fd.position))
			{
				outputMessages << TFileLogger::e_logType::LOG_ERROR
							   << "Point '" + point.getName() + "' has a DEFORM tag but is also inside frame-level DEFORM in frame '" + fd.position.node->data.get()->frame.getName() + "' (line " + std::to_string(fd.line) + "). Use one or the other, not both.";
				return false;
			}
		}
	}

	// Helper lambda: expand a single point into a _ref + sagged pair.
	// The original point object is kept in place (preserving any observation pointers)
	// and converted to a free sagged point. A new _ref point is created with the original's fixed state.
	auto expandPoint = [&](const std::string &originalName, const std::string &sagElementName, int lineNum) -> bool {
		std::string refName = originalName + "_ref";

		if (project.getPoints().doesObjectExist(refName))
		{
			outputMessages << TFileLogger::e_logType::LOG_ERROR
						   << "Line " + std::to_string(lineNum) + ": DEFORM cannot create reference point '" + refName + "' because that name already exists.";
			return false;
		}

		if (!project.getSags().doesObjectExist(sagElementName))
		{
			outputMessages << TFileLogger::e_logType::LOG_ERROR
						   << "Line " + std::to_string(lineNum) + ": DEFORM references undefined sag element '" + sagElementName + "'.";
			return false;
		}

		// Get the original point (observations already point to this object)
		LGCAdjustablePoint &origPoint = project.getPoints().getObject(originalName);
		TPositionVector provValue = origPoint.getProvisionalValue();
		TDataTreeIterator framePos = origPoint.getFrameTreePosition();
		TRefSystemFactory::ERefFrame refFrame = origPoint.getReferenceFrame();
		bool fx = origPoint.isCoordinateFixed(0), fy = origPoint.isCoordinateFixed(1), fz = origPoint.isCoordinateFixed(2);

		// Create the _ref point with the same fixed state as the original (preserving coordinates, frame position)
		LGCAdjustablePoint refPoint(provValue, fx, fy, fz, refName, refFrame, framePos);
		refPoint.setAutoGenerated(true);
		project.getPoints().addObject(refPoint);

		// Convert the original point in-place to a free sagged point (3 DOF).
		// This preserves all existing observation pointers to this object.
		origPoint.updateFixedState(false, false, false);

		// Create sag constraint pair: refName (reference) -> originalName (sagged)
		LGCAdjustableSag &sagObj = project.getSags().getObject(sagElementName);
		TLGCSagConstraintPair newPair(refName, originalName, sagObj);
		project.getSagPointPairs().push_back(newPair);

		return true;
	};

	// Phase C: Expand frame-level deforms
	for (size_t fdIdx = 0; fdIdx < frameDeforms.size(); ++fdIdx)
	{
		const auto &fd = frameDeforms[fdIdx];
		std::vector<std::string> pointNames;
		for (const auto &point : project.getPoints())
		{
			if (isInSubtreeOf(point.getFrameTreePosition(), fd.position))
				pointNames.push_back(point.getName());
		}

		std::string frameName = fd.position.node->data.get()->frame.getName();

		if (pointNames.empty())
		{
			outputMessages << TFileLogger::e_logType::LOG_WARNING
						   << "Line " + std::to_string(fd.line) + ": DEFORM in frame '" + frameName + "' found no points in subtree.";
			continue;
		}

		for (size_t pi = 0; pi < pointNames.size(); ++pi)
		{
			if (!expandPoint(pointNames[pi], fd.sagElementName, fd.line))
				return false;
		}
	}

	// Phase D: Expand point-level deforms
	{
		// Collect names first since expansion modifies the collection
		std::vector<std::pair<std::string, std::string>> pointDeforms; // (pointName, sagElementName)
		for (const auto &point : project.getPoints())
		{
			if (!point.getDeformSagElement().empty())
				pointDeforms.emplace_back(point.getName(), point.getDeformSagElement());
		}

		for (const auto &[pointName, sagName] : pointDeforms)
		{
			int lineNum = project.getPoints().getObject(pointName).line;
			if (!expandPoint(pointName, sagName, lineNum))
				return false;
		}
	}

	// Phase E: Validate unique sag pair membership (across DEFORM-generated and manual SAGCONNECT pairs)
	{
		std::unordered_map<std::string, int> assocPointCount;
		std::unordered_map<std::string, int> refPointCount;
		for (const auto &pair : project.getSagPointPairs())
		{
			assocPointCount[pair.assocPoint]++;
			refPointCount[pair.refPoint]++;
		}
		for (const auto &[name, count] : assocPointCount)
		{
			if (count > 1)
			{
				outputMessages << TFileLogger::e_logType::LOG_ERROR
							   << "Point '" + name + "' appears as associated point in " + std::to_string(count) + " sag constraint pairs. A point may only appear in one sag constraint pair.";
				return false;
			}
		}
		for (const auto &[name, count] : refPointCount)
		{
			if (count > 1)
			{
				outputMessages << TFileLogger::e_logType::LOG_ERROR
							   << "Point '" + name + "' appears as reference point in " + std::to_string(count) + " sag constraint pairs. A point may only appear in one sag constraint pair.";
				return false;
			}
		}
	}

	return true;
}

/// Helper function to iterate through all measurements and apply a callback
/// This ensures a single point of maintenance when measurement types change
template<typename Callback>
static void iterateAllMeasurements(TDataTreeIterator itTree, Callback callback)
{
	// Iterate through all measurement types
	for (auto const &i : itTree->get()->measurements.fTSTN)
	{
		for (auto const &j : i.get()->roms)
		{
			std::for_each(j.get()->measANGL.begin(), j.get()->measANGL.end(), callback);
			std::for_each(j.get()->measZEND.begin(), j.get()->measZEND.end(), callback);
			std::for_each(j.get()->measDIST.begin(), j.get()->measDIST.end(), callback);
			std::for_each(j.get()->measDHOR.begin(), j.get()->measDHOR.end(), callback);
			std::for_each(j.get()->measECTH.begin(), j.get()->measECTH.end(), callback);
			std::for_each(j.get()->measECDIR.begin(), j.get()->measECDIR.end(), callback);
			std::for_each(j.get()->measPLR3D.begin(), j.get()->measPLR3D.end(), callback);
		}
	}
	for (auto const &i : itTree->get()->measurements.fEDM)
		std::for_each(i.measDSPT.begin(), i.measDSPT.end(), callback);
	for (auto const &i : itTree->get()->measurements.fLEVEL)
		std::for_each(i.measDLEV.begin(), i.measDLEV.end(), callback);
	for (auto const &i : itTree->get()->measurements.fCAM)
	{
		std::for_each(i.measUVD.begin(), i.measUVD.end(), callback);
		std::for_each(i.measUVEC.begin(), i.measUVEC.end(), callback);
	}
	for (auto const &i : itTree->get()->measurements.fDVER)
		callback(i);
	for (auto const &i : itTree->get()->measurements.fORIE)
		std::for_each(i.measORIE.begin(), i.measORIE.end(), callback);
	for (auto const &i : itTree->get()->measurements.fECHO)
		std::for_each(i.measECHO.begin(), i.measECHO.end(), callback);
	for (auto const &i : itTree->get()->measurements.fECVE)
		std::for_each(i.measECVE.begin(), i.measECVE.end(), callback);
	for (auto const &i : itTree->get()->measurements.fECSP)
		std::for_each(i.measECSP.begin(), i.measECSP.end(), callback);
	callback(itTree->get()->measurements.fPDOR);
	for (auto const &i : itTree->get()->measurements.fRADI)
		callback(i);
	for (auto const &i : itTree->get()->measurements.fOBSXYZ)
		callback(i);
	for (auto const &i : itTree->get()->measurements.fINCLY)
		std::for_each(i.measINCLY.begin(), i.measINCLY.end(), callback);
	for (auto const &i : itTree->get()->measurements.fROLLY)
		std::for_each(i.measROLLY.begin(), i.measROLLY.end(), callback);
	for (auto const &i : itTree->get()->measurements.fECWS)
		std::for_each(i.measECWS.begin(), i.measECWS.end(), callback);
	for (auto const &i : itTree->get()->measurements.fECWI)
		std::for_each(i.measECWI.begin(), i.measECWI.end(), callback);
}

/// Register observation IDs from current frame and check for duplicates across all frames
bool TReader::checkAndRegisterObsId(TDataTreeIterator itTree, std::unordered_map<std::string, int> &globalObsIdMap, TFileLogger &outputMessages)
{
	std::string duplicateId;
	int duplicateLine = -1;
	int firstOccurrenceLine = -1;
	
	// Use the shared iteration helper to check for duplicates across all frames
	iterateAllMeasurements(itTree, [&globalObsIdMap, &duplicateId, &duplicateLine, &firstOccurrenceLine](auto const &meas) {
		if (!meas.obsID.empty() && duplicateId.empty()) {  // Only record first duplicate
			auto result = globalObsIdMap.insert({meas.obsID, meas.line});
			if (!result.second) {  // Insertion failed - duplicate found across frames!
				duplicateId = meas.obsID;
				duplicateLine = meas.line;
				firstOccurrenceLine = result.first->second;
			}
		}
	});
	
	// If duplicate was found, report it with both line numbers
	if (!duplicateId.empty())
	{
		const std::string lineStr = "Line " + std::to_string(duplicateLine) + ": ";
		outputMessages << TFileLogger::e_logType::LOG_ERROR << lineStr + "Observation ID \"" + duplicateId + "\" is duplicated (first occurrence at line " + std::to_string(firstOccurrenceLine) + ").";
		return true;
	}

	return false;
}
