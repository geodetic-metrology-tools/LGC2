/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
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