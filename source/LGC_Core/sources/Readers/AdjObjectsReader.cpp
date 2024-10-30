#include "AdjObjectsReader.h"

///////////////////////////////////////////////////////
// TKeyFRAME
///////////////////////////////////////////////////////
TKeyFRAME::TKeyFRAME(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAKeyWord(FRAME, project)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyFRAME::parse(const std::vector<std::string> &tokens, bool /*activeLine*/, int line)
{
	using namespace LGC;
	auto numTokens = tokens.size();

	// The asterisk and the keyword itself are already two tokens
	if (numTokens < 10)
		throw std::runtime_error("Key *FRAME takes at least 8 arguments: Name, tx, ty, tz, rx, ry, rz and scale.");

	// set translations, rotations and scale in the bitset to be fixed by default
	std::bitset<3> translations(std::string("111"));
	std::bitset<3> rotations(std::string("111"));
	std::bitset<1> scale(std::string("1"));

	TOptionHelper opts(tokens.cbegin() + 2, tokens.cend());

	// If flag is used, change state of appropriate bit set and make the element of transl. rotation or scale variable (set 0)
	if (opts.has("TX"))
		translations.set(0, 0);
	if (opts.has("TY"))
		translations.set(1, 0);
	if (opts.has("TZ"))
		translations.set(2, 0);
	if (opts.has("RX"))
		rotations.set(0, 0);
	if (opts.has("RY"))
		rotations.set(1, 0);
	if (opts.has("RZ"))
		rotations.set(2, 0);
	if (opts.has("SCL"))
		scale.set(0, 0);

	const auto gon(TAngle::kGons);
	TransformParameters transfParam;
	transfParam.tX = TLength(std::stor(tokens[3])); // Translation along X

	std::string trafoName = tokens[2];
	// Create adjustable helmert transformation
	TAdjustableHelmertTransformation adjTrafo = TAdjustableHelmertTransformation( // Wrapper containing adjustable related information
		translations, // Bits telling which of the translations are fixed
		rotations, // Bits telling which of the rotations around an axis are fixed
		scale, // Bit telling whether scale is fixed
		trafoName // Transformation name
	);

	adjTrafo.setParam( // The transformation itself
		TLength(std::stor(tokens[3])), // Translation along X
		TLength(std::stor(tokens[4])), // Translation along Y
		TLength(std::stor(tokens[5])), // Translation along Z
		TAngle(std::stor(tokens[6]), gon), // Rotation around X
		TAngle(std::stor(tokens[7]), gon), // Rotation around Y
		TAngle(std::stor(tokens[8]), gon), // Rotation around Z
		std::stor(tokens[9]) // SCALE factor
	);

	// remember the line of the frame definition
	adjTrafo.getLine() = line;

	// Create a new level in the tree using the current transformation definition.
	proj.addChild(&adjTrafo);

	// check for "Slave" frames
	if (opts.has("SLAVE"))
	{
		std::string groupName = opts.getParam("SLAVE");
		// does this slave group already exist?
		bool groupAlreadyExists{false};
		for (TLGCFrameConstraintGroup &group : proj.getSlaveGroups())
		{
			if (group.getGroupName() == groupName)
			{
				groupAlreadyExists = true;
				group.addFrameToGroup(adjTrafo.getName(), proj);
			}
		}
		if (!groupAlreadyExists)
		{
			TLGCFrameConstraintGroup newGroup;
			newGroup.setGroupName(groupName);
			newGroup.addFrameToGroup(adjTrafo.getName(), proj);
			proj.getSlaveGroups().push_back(newGroup);
		}
	}
}

///////////////////////////////////////////////////////
// TKeyENDFRAME
///////////////////////////////////////////////////////
TKeyENDFRAME::TKeyENDFRAME(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAKeyWord(ENDFRAME, project)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyENDFRAME::parse(const std::vector<std::string> &tokens, bool, int)
{
	if (tokens.size() != 2 && tokens[2].at(0) != '%') // More then 2 and at the same time the third is not comment
		throw std::runtime_error("Key *ENDFRAME expects no arguments.");

	// Throw an error if the *ENDFRAME keyword is in the root.
	if (proj.getCurrentNode().ID.size() == 1)
	{
		throw std::runtime_error("key *ENDFRAME is not allowed in ROOT Frame");
	}

	proj.moveUp();
}

///////////////////////////////////////////////////////
// TAPointKey
///////////////////////////////////////////////////////

TAPointKey::TAPointKey(TLGCData &project, const std::string &word) :
	TAKeyWord(word, project), defaultErrmsg("Point entries need 4 parameters: ID and 3 coordinates (X,Y,H|Z)."), fconfig(proj.getConfig()), fpointAccess(proj.getPoints())
{
}

size_t TAPointKey::findComment(const std::string &s)
{
	for (size_t i = 0; i < strlen(INPUT_SEPERATOR_CHARS); i++)
	{
		size_t pos = s.find(INPUT_SEPERATOR_CHARS[i]);
		if (pos != std::string::npos)
			return pos;
	}
	return std::string::npos;
}

void TAPointKey::parse(const std::vector<std::string> &tokens, bool activeLine, int line)
{
	using namespace LGC;

	auto numTokens = tokens.size();

	if (numTokens == 0 || tokens.at(0) == "*")
		return;

	size_t commentpos = findComment(tokens.at(0));

	if (commentpos != std::string::npos)
	{
		// this line is a comment, stpointore it and be done
		hdrcomment += std::string(tokens.at(0) + "\n");
		return;
	}

	if (numTokens < 4)
		throw std::runtime_error(defaultErrmsg);

	if (fpointAccess.doesObjectExist(tokens.at(0)))
		throw std::runtime_error("Point " + tokens.at(0) + " already exists.");

	// the conversion method stor throws on error
	auto &pt = insertPoint(tokens.at(0), std::stor(tokens.at(1)), std::stor(tokens.at(2)), std::stor(tokens.at(3)));
	pt.line = line;
	pt.setActive(activeLine);

	if (tokens.at(0).size() > proj.getConfig().pointNameWidth)
		proj.getConfig().pointNameWidth = (int)tokens.at(0).size();

	TOptionHelper opts(tokens.cbegin(), tokens.cend());

	// check if this point comes with weights and modify the pt object accordingly
	pointSigmaData &ptSigma = pt.getPointSigmaData();

	// check for values of bearing, roll, slope, deflection (bear,roll,slope,half defl)
	for (int j = 0; j < ptSigma.fAngles.size(); j++)
		ptSigma.fAngles[j].setGonsValue(opts.getParamR(ptSigma.fAngleNames[j]));

	ptSigma.fHasAngle = (!isZero(ptSigma.fAngles[0]) || !isZero(ptSigma.fAngles[1]) || !isZero(ptSigma.fAngles[2]) || !isZero(ptSigma.fAngles[3]));

	// create the rotation matrix in case some angles are defined, if no angle is defined the rotation matrix is the identity
	if (ptSigma.fHasAngle)
	{
		ptSigma.calcAndSetRotMat();
	}

	// values are given in mm
	bool hasSigmas = false;
	std::vector<std::string> stdDevNames({"SX", "SY", "SZ"});
	for (int idx = 0; idx < stdDevNames.size(); ++idx)
	{
		if (opts.has(stdDevNames[idx]))
		{
			ptSigma.fSigmas[idx] = MM2M * opts.getParamR(stdDevNames[idx]);
			hasSigmas = true;
		}
	}
	if ((ptSigma.fSigmas.array() < 0).any())
	{
		throw std::runtime_error("Standard deviation cannot be negative.");
	}
	if ((ptSigma.fSigmas.array() < nullLimit && ptSigma.fSigmas.array() > 0.0).any())
	{
		throw std::runtime_error("Specified standard deviation is too small, consider to set it to 0 to fix the variable.");
	}

	// check if a apriori covariance matrix is given
	if (opts.has("APRICOV"))
	{
		if (hasSigmas)
			throw std::runtime_error("Either a point apriori covariance is provided OR sigmas are provided.");
		if (ptSigma.fHasAngle)
			// in theory we could apply the covariance matrix to the rotated system but to avoid confusion we do not allow it
			throw std::runtime_error("A-priori covariance can't be combined with rotation angles.");
		std::string test = opts.getParam("APRICOV");
		Eigen::Matrix3d apriCovMat = opts.commaSeparatedStringToMat(test, 3);
		if (!apriCovMat.isApprox(apriCovMat.transpose()))
			throw std::runtime_error("a-priori covariance matrix must be symmetric");
		if (!TSparseUtils::isPositiveDefinite(apriCovMat))
			throw std::runtime_error("a-priori covariance matrix must be positive definite");

		ptSigma.fApriCovMat = apriCovMat;
		ptSigma.fHasApriCovMat = true;
	}

	// check if apriori information is attached to point
	if (hasSigmas || ptSigma.fHasApriCovMat)
	{
		// for now only allow point with sigma for points defined in a *POIN section
		// this prevents ambiguous lines like
		// *VZ
		// A 1 2 3 SZ 1
		// which would be overriden to a point free in x,y,z with a weight on z
		// this will become obsolete when *VZ will be replaced by A 1 2 3 SX 0 SY 0 and so on..
		if (pt.getNumUnkn() != 3)
		{
			throw std::runtime_error("Attaching a precision in a point definition is only possible in *POIN section. Point " + pt.getName());
		}
		pt.activatePointSigma();
	}

	// store the matrix in the point
	if (ptSigma.fHasApriCovMat)
		pt.setAprioriCovarianceMatrix(ptSigma.fApriCovMat);
	else
	{
		// if matrix was not set by user, calculate it from the sigmas
		// if any sigma will be not defined (point free in this axis) and theres a rotation, the apriori covariance matrix will remain NAN, which is ok.
		Eigen::Matrix3d apriCovMat;
		apriCovMat.setZero();
		apriCovMat.diagonal() << pow2(ptSigma.fSigmas[0]), pow2(ptSigma.fSigmas[1]), pow2(ptSigma.fSigmas[2]);
		if (ptSigma.fHasAngle)
		{
			Eigen::Matrix3d rotMat = ptSigma.fRotMat;
			pt.setAprioriCovarianceMatrix(rotMat.transpose() * apriCovMat * rotMat);
		}
		else
			pt.setAprioriCovarianceMatrix(apriCovMat);
	}

	// check if one of the weights was set to zero
	// if no angle was used we block the corresponding freedoms
	// if rotations are used, a constraint has to be introduced (see TDataAnalyzer)
	if (hasSigmas)
	{
		if (!ptSigma.fHasAngle)
		{
			// if no angles are involved we can block the degrees of freedom directly
			pt.updateFixedState(isZero(ptSigma.fSigmas[0]), isZero(ptSigma.fSigmas[1]), isZero(ptSigma.fSigmas[2]));
		}
	}

	// If last token starts with a comment chararcter, store it
	const char fOfLastToken = tokens.back().at(0);
	if (fOfLastToken == '$' || fOfLastToken == '%')
		pt.eolcomment = tokens.back();

	// remove the newline of the last comment line
	auto lastchar(hdrcomment.length() - 1);
	if (hdrcomment.length() > 0 && hdrcomment[lastchar] == '\n')
		hdrcomment[lastchar] = 0;
	// store the comment lines above the point
	pt.hdrcomment = hdrcomment;

	// reset the headercomment storage
	hdrcomment = "";
}

///////////////////////////////////////////////////////
// Specific points definitions
///////////////////////////////////////////////////////
TKeyCALA::TKeyCALA(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAPointKey(project, CALA)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

LGCAdjustablePoint &TKeyCALA::insertPoint(const std::string &pointName, TReal x, TReal y, TReal z)
{
	// TRUE, if it is a ROOT node
	if (proj.getCurrentNode().isROOTNode())
	{
		if (fconfig.referential == TRefSystemFactory::ERefFrame::kLocalRefFrame)
			return fpointAccess.addObject(LGCAdjustablePoint(
				TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), true, true, true, pointName, fconfig.referential, proj.getCurrentPosition()));
		else
			return fpointAccess.addObject(LGCAdjustablePoint(
				TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k2DPlusH), true, true, true, pointName, fconfig.referential, proj.getCurrentPosition()));
	}
	else
		// If it is defined in a sub-frame the provisional values are given in XYZ coordinates relative to the subframe in which was the point defined
		return fpointAccess.addObject(LGCAdjustablePoint(
			TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), true, true, true, pointName, fconfig.referential, proj.getCurrentPosition()));
}

TKeyPOIN::TKeyPOIN(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAPointKey(project, POIN)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

LGCAdjustablePoint &TKeyPOIN::insertPoint(const std::string &pointName, TReal x, TReal y, TReal z)
{
	if (proj.getCurrentNode().isROOTNode())
	{
		if (fconfig.referential == TRefSystemFactory::ERefFrame::kLocalRefFrame)
			return fpointAccess.addObject(LGCAdjustablePoint(
				TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), false, false, false, pointName, fconfig.referential, proj.getCurrentPosition()));
		else
			return fpointAccess.addObject(LGCAdjustablePoint(
				TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k2DPlusH), false, false, false, pointName, fconfig.referential, proj.getCurrentPosition()));
	}
	else
		return fpointAccess.addObject(LGCAdjustablePoint(
			TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), false, false, false, pointName, fconfig.referential, proj.getCurrentPosition()));
}

TKeyVXY::TKeyVXY(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAPointKey(project, VXY)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

LGCAdjustablePoint &TKeyVXY::insertPoint(const std::string &pointName, TReal x, TReal y, TReal z)
{
	if (proj.getCurrentNode().isROOTNode())
	{
		if (fconfig.referential == TRefSystemFactory::ERefFrame::kLocalRefFrame)
			return fpointAccess.addObject(LGCAdjustablePoint(
				TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), false, false, true, pointName, fconfig.referential, proj.getCurrentPosition()));
		else
			return fpointAccess.addObject(LGCAdjustablePoint(
				TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k2DPlusH), false, false, true, pointName, fconfig.referential, proj.getCurrentPosition()));
	}
	else
		return fpointAccess.addObject(LGCAdjustablePoint(
			TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), false, false, true, pointName, fconfig.referential, proj.getCurrentPosition()));
}

TKeyVXZ::TKeyVXZ(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAPointKey(project, VXZ)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

LGCAdjustablePoint &TKeyVXZ::insertPoint(const std::string &pointName, TReal x, TReal y, TReal z)
{
	if (proj.getCurrentNode().isROOTNode())
	{
		if (fconfig.referential == TRefSystemFactory::ERefFrame::kLocalRefFrame)
			return fpointAccess.addObject(LGCAdjustablePoint(
				TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), false, true, false, pointName, fconfig.referential, proj.getCurrentPosition()));
		else
			return fpointAccess.addObject(LGCAdjustablePoint(
				TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k2DPlusH), false, true, false, pointName, fconfig.referential, proj.getCurrentPosition()));
	}
	else
		return fpointAccess.addObject(LGCAdjustablePoint(
			TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), false, true, false, pointName, fconfig.referential, proj.getCurrentPosition()));
}

TKeyVYZ::TKeyVYZ(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAPointKey(project, VYZ)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

LGCAdjustablePoint &TKeyVYZ::insertPoint(const std::string &pointName, TReal x, TReal y, TReal z)
{
	if (proj.getCurrentNode().isROOTNode())
	{
		if (fconfig.referential == TRefSystemFactory::ERefFrame::kLocalRefFrame)
			return fpointAccess.addObject(LGCAdjustablePoint(
				TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), true, false, false, pointName, fconfig.referential, proj.getCurrentPosition()));
		else
			return fpointAccess.addObject(LGCAdjustablePoint(
				TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k2DPlusH), true, false, false, pointName, fconfig.referential, proj.getCurrentPosition()));
	}
	else
		return fpointAccess.addObject(LGCAdjustablePoint(
			TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), true, false, false, pointName, fconfig.referential, proj.getCurrentPosition()));
}

TKeyVZ::TKeyVZ(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAPointKey(project, VZ)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

LGCAdjustablePoint &TKeyVZ::insertPoint(const std::string &pointName, TReal x, TReal y, TReal z)
{
	if (proj.getCurrentNode().isROOTNode())
	{
		if (fconfig.referential == TRefSystemFactory::ERefFrame::kLocalRefFrame)
			return fpointAccess.addObject(LGCAdjustablePoint(
				TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), true, true, false, pointName, fconfig.referential, proj.getCurrentPosition()));
		else
			return fpointAccess.addObject(LGCAdjustablePoint(
				TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k2DPlusH), true, true, false, pointName, fconfig.referential, proj.getCurrentPosition()));
	}
	else
		return fpointAccess.addObject(LGCAdjustablePoint(
			TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), true, true, false, pointName, fconfig.referential, proj.getCurrentPosition()));
}