////////////////////////////////////////////////////////////////////
// TCovarFileWriter
/*!
Write all coariance matrices for points and frame in their tree position
*/
/////////////////////////////////////////////////////////////////////


#ifndef _COVARFILEWRITER_H_
#define _COVARFILEWRITER_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include <string>

#include "TAFileWriter.h"
#include "TAdjustableHelmertTransformation.h"

#include "Global.h"
#include "LGCAdjustablePoint.h"


class TLGCData;
class TAStreamFormatter;


using namespace std;
// typedefs
////////////////////////////////////////////////////////////////

/*!
\ingroup Writers
\brief Write a output file for COVAR.
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
	void	writeFile(const string error);
	/// write the input file for DEFORM
	void	writeFile(TLGCData &project);

private:

	/*!default constructor*/
	TCovarFileWriter();

	void writeTitle(TLGCData &project);
	void writePoints(LGCAdjustablePoint&);
	void writeFrames(TTreeEntry*);
	void writeFrameDefinition(TAdjustableHelmertTransformation&);
	void writeFrameUpperTriangularCovarianceMatrix(TAdjustableHelmertTransformation&);
	void writePointUpperTriangularCovarianceMatrix(LGCAdjustablePoint&);

};
#endif