/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _CHABAFILEWRITER_H_
#define _CHABAFILEWRITER_H_
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//STL
#include <string>
//SURVEYLIB
#include <TAdjustableHelmertTransformation.h>
#include <TPointConverter.h>
//LGC
#include <Global.h>
#include <TAFileWriter.h>
#include <LGCAdjustablePoint.h>
#include <MeasDef.h>

class TLGCData;
class TAStreamFormatter;
class TAStreamFormatter;
class TAdjustableHelmertTransformation;
using namespace std;

/*!
Class for writing all files: 
- writeBestfitResultFile and writeTransformationResultFile write the main results: new coordinates, parameter's values and additionnal information like sigmas, deviations...
- writeCooFile and writePunchFile write list of transformed points in different format
- writeInputFilePoints write points used for the calculation (.act and .pas)
- writeProjectFile write the project file

Other (private) functions are used inside the functions described below to write part of the files like title, a list of points, the helmert parameters...
*/

class TChabaFileWriter : public TAFileWriter
{
public:
	struct delta { TLength dx, dy, dz; };

	/*!@name Constructors and Destructors*/
	//@{
	//!constructor
	TChabaFileWriter(TAStreamFormatter* stream, const TLGCData* project);

	//!Destructor
	virtual  ~TChabaFileWriter();
	//@}

	/// write .chabaout file for a bestfit calculation
	void writeFile(TAStreamFormatter* stream, const TLGCData & proj);


private:
	/*!default constructor*/
	TChabaFileWriter();

	/// This function write the transformation details
	void writeHelmertTransformationDetails(const TAdjustableHelmertTransformation & helmert);

	///Control the writing of transformed passive points which fall within the tolerance
	void writeTransformedPoints(const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> & pairs, const std::vector<LGCAdjustablePoint> & secondary, bool writeDeltas);

	///Write all transformed secondary passive points
	void writeTransformedSecondaryPoints(const std::vector<LGCAdjustablePoint> & secondary);

	///Write active and passive x,y and z coordinates of point pairs before transformation
	void writePointPairs(const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> & pairs);

	///Write points list after transformation
	void writePUNPoint(const LGCAdjustablePoint & p, delta d, bool writeDeltas);

	///Write header in out file
	void writeTitle(const TLGCData &project);

	///create pair list
	const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> createPair(const TLGCData &project, TDataTreeIterator itTree);
	///creat secondary points list
	const std::vector<LGCAdjustablePoint> createSecPoint(const TLGCData &project, TDataTreeIterator itTree);
};

#endif
