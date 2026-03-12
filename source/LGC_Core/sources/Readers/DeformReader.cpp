// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "DeformReader.h"

#include <TLGCData.h>
#include <stdexcept>
#include <string>

void TADeformKey::parse(const std::vector<std::string> &tokens, bool activeLine, int line)
{
	// Expected format: * DEFORM sagElementName
	// Tokens: ["*", "DEFORM", "sagElementName"]

	if (tokens.size() < 3)
		throw std::runtime_error("Key *DEFORM requires 1 argument: sag element name. "
								 "Syntax: *DEFORM sagElementName");

	auto &node = proj.getCurrentNode();
	if (!node.deformSagElementName.empty())
		throw std::runtime_error("Line " + std::to_string(line) + ": Frame already has a *DEFORM directive (line " + std::to_string(node.deformLine) + ").");

	node.deformSagElementName = tokens[2];
	node.deformLine = line;
}
