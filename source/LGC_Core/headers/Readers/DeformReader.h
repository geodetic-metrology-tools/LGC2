/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _READERS_DEFORMREADER_H_
#define _READERS_DEFORMREADER_H_

// LGC
#include <Global.h>
#include <ReaderBase.h>
#include <KeywordRights.h>

#include <string>
#include <vector>

/*!
 *  \addtogroup InputFileReader
 *  @{
 */

/*!
	\brief Keyword handler for *DEFORM.

	*DEFORM applies to the current frame position in the tree. Syntax: *DEFORM sagElementName
	Stores the sag element name directly on the current frame node (TTreeEntry).
	The actual expansion (renaming points, creating free points, creating sag constraint pairs)
	happens as a post-processing step after the entire file is read.
*/
class TADeformKey : public TAKeyWord
{
public:
	TADeformKey(TLGCData &project, int nb_allowed_keywords = nb_allowed_deform, const char **keywords = allowed_DEFORM)
		: TAKeyWord(DEFORM, project)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	TADeformKey &operator=(const TADeformKey &);
};

/*! @} */
#endif
