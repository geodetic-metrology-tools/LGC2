/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_DEFA_WRITER
#define SU_DEFA_WRITER
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//STL
#include <string>
//LGC
#include <TAFileWriter.h>

class	TLGCData;
class	TAStreamFormatter;
class TLSResultsMatrices;

using namespace std;


/*!
\ingroup Writers
\brief Write a output file for DEFORM. Write a deformation analysis input file
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
			void	writeFile(const TLGCData &project, TLSResultsMatrices& rm);

private:

	/*!default constructor*/
	TDefaFileWriter();

    void writeTitle(const TLGCData &project);
    void writePoints(const TLGCData&);
    void write3DAnalysis(const TLGCData&);
    void writeXYAnalysis(const TLGCData&);
    void writeXZAnalysis(const TLGCData&);
    void writeYZAnalysis(const TLGCData&);
    void writeXAnalysis(const TLGCData&);
    void writeYAnalysis(const TLGCData&);
    void writeZAnalysis(const TLGCData&);
    void writeCALA(const TLGCData&);
	void write3DPoint(string name, TReal X, TReal Y, TReal Z);
    void writeUpperTriangularCovarianceMatrix(const TLGCData& project, TLSResultsMatrices& rm);
	

private:

};

/*@}*/

#endif // SU_DEFA_WRITER