// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <TALGCObjectWriter.h>

#include "TAStreamFormatter.h"

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// constructor / destructor
/////////////////////////////////////////////////////////////////////////////
// default constructor
TALGCObjectWriter::TALGCObjectWriter() : TAConverter()
{
}

TALGCObjectWriter::TALGCObjectWriter(TAStreamFormatter &stream) : TAConverter(stream)
{
}

TALGCObjectWriter::~TALGCObjectWriter()
{ // destructor
}

void TALGCObjectWriter::writeKeyWord(const std::string &keyword)
{
	TAStreamFormatter &stream = this->getStreamRef();

	// Ouput the keyword to the stream
	stream << keyword;
	stream << this->getSeparator();
	stream << endl;

	return;
}
