////////////////////////////////////////////////////////////////////
// TDefAFileWriter
/*!
Write a deformation analysis input file
 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////////////////

#ifndef SU_DEFA_WRITER
#define SU_DEFA_WRITER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
class	TLGCData;
class	TAStreamFormatter;
class TLSResultsMatrices;
#include "TAFileWriter.h"
#include <string>
using namespace std;
// typedefs
////////////////////////////////////////////////////////////////

/*!
\ingroup Writers
\brief Write a output file for DEFORM.
*/


class  TDefaFileWriter : public TAFileWriter  
{
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TDefaFileWriter(TAStreamFormatter* stream, const TLGCData* project);

			//!Destructor
			virtual  ~TDefaFileWriter();
		//@}

			/// write errors
			void	writeFile(const string error);
			/// write the input file for DEFORM
			void	writeFile(TLGCData &project, TLSResultsMatrices& rm);

private:

	/*!default constructor*/
	TDefaFileWriter();

	void writeTitle(TLGCData &project);
	void writePoints(TLGCData&);
	void write3DAnalysis(TLGCData&);
	void writeXYAnalysis(TLGCData&);
	void writeXZAnalysis(TLGCData&);
	void writeYZAnalysis(TLGCData&);
	void writeXAnalysis(TLGCData&);
	void writeYAnalysis(TLGCData&);
	void writeZAnalysis(TLGCData&);
	void writeCALA(TLGCData&);
	void write3DPoint(string name, TReal X, TReal Y, TReal Z);
	void writeUpperTriangularCovarianceMatrix(TLGCData& project, TLSResultsMatrices& rm);
	

private:

};

/*@}*/

#endif // SU_DEFA_WRITER