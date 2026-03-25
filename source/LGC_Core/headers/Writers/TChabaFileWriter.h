/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _CHABAFILEWRITER_H_
#define _CHABAFILEWRITER_H_
#if _MSC_VER >= 1000
#	pragma once
#endif // _MSC_VER >= 1000

// STL
#include <string>
// SURVEYLIB
#include <TAdjustableHelmertTransformation.h>
#include <TPointConverter.h>
// LGC
#include <Global.h>
#include <LGCAdjustablePoint.h>
#include <MeasDef.h>
#include <TAFileWriter.h>

class TLGCData;
class TAStreamFormatter;
class TAStreamFormatter;
class TAdjustableHelmertTransformation;

/*!
Class for writing a result file in CHABA format

Other (private) functions are used inside the functions described below to write part of the files like title, a list of points, the helmert parameters...
*/

class TChabaFileWriter : public TAFileWriter
{
public:
	struct delta
	{
		TLength dx, dy, dz;
	};

	/*!@name Constructors and Destructors*/
	//@{
	//! constructor
	TChabaFileWriter(TAStreamFormatter *stream, const TLGCData *project);

	//! Destructor
	virtual ~TChabaFileWriter();
	//@}

	/// write .chabaout file for a bestfit calculation
	void writeFile(TAStreamFormatter *stream);

private:
	/*!default constructor*/
	TChabaFileWriter();

	/// This function write the transformation details
	void writeHelmertTransformationDetails(const TAdjustableHelmertTransformation &helmert, const std::vector<int> &ID);

	/// write transformed points
	void writeTransformedPoints(const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> &pairs, bool writeDeltas, TDataTreeIterator itTree);

	/// Write all transformed secondary passive points
	void writeTransformedSecondaryPoints(const std::vector<LGCAdjustablePoint> &secondary);

	/// Write active and passive x,y and z coordinates  before transformation
	void writeInputPoints(const std::vector<TOBSXYZ> &inputdata);

	/// Write points list
	void writePUNPoint(const LGCAdjustablePoint &p, TFreeVector sigma, delta d, bool writeDeltas);

	/// Write header in out file
	void writeTitle();

	/// create pair list between the OBSXYZ in the require node and the adjustable points to write results (coordinates, sigmas and differences)
	const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> createPair(TDataTreeIterator itTree);

	/// keep the OBSXYZ list corresponding to the required node to write the input data used
	const std::vector<TOBSXYZ> keepOBSXYZ(TDataTreeIterator itTree);

	/// create secondary points list
	const std::vector<LGCAdjustablePoint> createSecPoint(TDataTreeIterator itTree);

	/// Transform the sigma of the OBSXYZ observations in the root frame
	TFreeVector transformSigmaInRoot(const TPositionVector ptInSF, TDataTreeIterator itTree, TLength sx, TLength sy, TLength sz);
};

#endif
