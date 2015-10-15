////////////////////////////////////////////////////////////////////
// TSimFileWriter
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
class	TLGCDataSet;
class	LSCalcDataSet;
class	TAStreamFormatter;
class	TLGCProject;
#include "TAFileWriter.h"
#include <string>
using namespace std;
// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGC

@{*/

//Class definition
class  TDefAFileWriter : public TAFileWriter  
{
public:

	using TAFileWriter::writeFile;

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TDefAFileWriter(TAStreamFormatter* stream, const TLGCProject* project);

			//!Destructor
			virtual  ~TDefAFileWriter();
		//@}


			void	writeFile(const string error);
			void	writeFile(TLGCProject &project);

private:

	/*!default constructor*/
	TDefAFileWriter();

	void writeTitle(TLGCProject &project);
	void writePoints(TLGCProject&);
	void write3DAnalysis(TLGCProject&);
	void write3DPoint(const TLSCalcPosVectorParam&);
	void write2DAnalysis(TLGCProject&);
	void write2DPoint(const TLSCalcPosVectorParam&);
	void writeCALA(TLGCProject&);
	void writeUpperTriangularCovarianceMatrix(TLGCProject&);
	

private:

};

/*@}*/

#endif // SU_DEFA_WRITER
