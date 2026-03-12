#include "SagConstraintReader.h"

#include <string>
#include <sstream>
#include <TLGCData.h>
#include <TLGCSagConstraintPair.h>


void TASagConstraintPairKey::parse(const std::vector<std::string> &tokens, bool activeLine, int line)
{
//	std::cout << "Parsing a sag association" << std::endl;
	// 1. token is the *
	// 2. keyword
	// 3. ref point
	// 4. associated point
	// 5. responsible sag adjustable object name

	// check if its the keyword line or a pair-definition line
	int nTokens = tokens.size();
	// TODO:
	// several cases:
	// 1. 1 token => sag pair connected to provisional value
	// 2. 2 token => two points that will be connected
	// 3. >8 tokens => keywordline with definition of adjustable sag element
	// 4. 3 token => keywordline with only a name reference to the adjustabel sag element (maybe to be removed)
	if (nTokens >= 8)
	{
		TOptionHelper opts(tokens.cbegin() + 2, tokens.cend());
		std::string sagName = tokens[2];
		std::string frameName = tokens[3];
		TLength vertSag(std::stor(tokens[4]));
		TLength vertCurv(std::stor(tokens[5]));
		TLength radSag(std::stor(tokens[6]));
		TLength radCurv(std::stor(tokens[7]));
		std::bitset<5> fixedStates("11110");
		if (opts.has("VS"))
			fixedStates[1] = 0;
		if (opts.has("VC"))
			fixedStates[2] = 0;
		if (opts.has("RS"))
			fixedStates[3] = 0;
		if (opts.has("RC"))
			fixedStates[4] = 0;
		
		LGCAdjustableSag sagObject(sagName, frameName, vertSag, vertCurv, radSag, radCurv, fixedStates);
		// remember the line of the element definition
		sagObject.getLine() = line;
		if (fSagAccess.doesObjectExist(sagName))
			throw std::runtime_error("Sag adjustable element " + sagName + " is already defined.");
		else
			fSagAccess.addObject(sagObject);

		// setting the name of the adjustable sag element
		std::string sagElementName = sagName;
		fSagElementName = sagElementName;
	}
	else if (nTokens == 3)
	{
		// setting the name of the adjustable sag element
		std::string sagElementName = tokens[2];
		fSagElementName = sagElementName;
	}
	else if (nTokens == 2)
	{
		// defining a sag-association
		std::string refPointName = tokens[0];
		std::string assocPointName = tokens[1];
		std::string sagElementName = fSagElementName;
		if (proj.getSags().doesObjectExist(sagElementName))
		{
			LGCAdjustableSag &sagObject = proj.getSags().getObject(sagElementName);
			TLGCSagConstraintPair newPair(refPointName, assocPointName, sagObject);
			proj.getSagPointPairs().push_back(newPair);
			//fSagElementName = sagElementName;
		}
		else
		{
			throw std::runtime_error("Sag adjustable element " + sagElementName + " needs to be defined before it can be used.");
		}
	}
	else if (nTokens == 1)
	{
		throw std::runtime_error("*SAGCONNECT with a single point name is no longer supported. "
								 "Use *DEFORM or the DEFORM tag on a point definition to create "
								 "sag constraint pairs, or provide both reference and associated "
								 "point names (2 names per line).");
	}
	else
	{
		throw std::runtime_error("Sag association parser expects either a sag adjustable object name (with or without definition) in the keyword line, a pair of pointnames or a single pointname in subsequent lines");
	}
}
