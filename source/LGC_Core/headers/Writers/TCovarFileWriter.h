/*
 * SPDX-FileCopyrightText: 2025 CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _COVARFILEWRITER_H_
#define _COVARFILEWRITER_H_
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//STL
#include <string>
//SURVEYLIB
#include <TAdjustableHelmertTransformation.h>
//LGC
#include <Global.h>
#include <TAFileWriter.h>
#include <LGCAdjustablePoint.h>

class TLGCData;
class TAStreamFormatter;



/*!
\ingroup Writers
\brief Write all coariance matrices for points and frame in their tree position
*/
class TCovarFileWriter : public TAFileWriter  
{
public:
	/*!@name Constructors and Destructors*/
	//@{
	//!constructor
	TCovarFileWriter(TAStreamFormatter* stream, const TLGCData* project);

	//!Destructor
	virtual  ~TCovarFileWriter();
	//@}

	/// write errors
	void	writeFile(const std::string error);
	/// write the input file for DEFORM
    void	writeFile(const TLGCData &project);

private:

	/*!default constructor*/
	TCovarFileWriter();

    void writeTitle(const TLGCData &project);
	void writePoints(const LGCAdjustablePoint&);
	void writeFrames(TTreeEntry*);
	void writeFrameDefinition(TAdjustableHelmertTransformation&);
	void writeFrameUpperTriangularCovarianceMatrix(TAdjustableHelmertTransformation&);
	void writePointUpperTriangularCovarianceMatrix(const LGCAdjustablePoint&);

};
#endif