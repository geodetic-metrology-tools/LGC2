////////////////////////////////////////////////////////////////////
// TMesFileWriter
/*!
Writes an LGC measurements file

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////////////////

#ifndef SU_MES_FILE_WRITER
#define SU_MES_FILE_WRITER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
class	TLGCProject;
#include	"TAFileWriter.h"
#include	"lsalgo/TALSCalcParameter.h"
#include	"TAngle.h"
#include	"TLength.h"
// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGC

@{*/

//Class definition
class  TMesFileWriter : public TAFileWriter  
{
public:

	using TAFileWriter::writeFile;

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TMesFileWriter(TAStreamFormatter* stream, const TLGCProject* project);

			//!Destructor
			virtual  ~TMesFileWriter();
		//@}


	/*!@name Public menber functions*/
		//@{
			//!write the lgc file used when there's no error in the project
			virtual void	writeFile(TLGCDataSet*, LSCalcDataSet*);
		//@}

private:

	/*!default constructor*/
	TMesFileWriter();


	/*!@name Private member functions*/
	//@{
		//! write ANGL measurements
		void	writeHorAng();
		//! write ZENH/ZENI measurements
		void	writeZenDist(TALSCalcParameter::ELSStatus);
		//! write DMES/DTHE measurements
		void	writeSpaDist(TALSCalcParameter::ELSStatus);
		//! write ORIE measurements
		void	writeGyroOrie();
		//! write DVER measurements
		void	writeVertDist();
		//! write DHOR measurements
		void	writeHorDist();
		//! write ECHO measurements
		void	writeOffsetToVerPlane();
		//! write ECSP measurements
		void	writeOffsetToSpaLine();
		//! write ECTH measurements
		void	writeOffsetToTheoPlane();
		//! write ECVE measurements
		void	writeOffsetToVerLine();
		//! write RADI measurements
		void	writeRadOffCnstr();

		void	formatAngleLine(const long int id, const string namept1, const string namept2, const string namept3,  const TAngle obs, const TAngle calc, const TAngle res, const TAngle sig, const TAngle v0, const string com);
		void	formatLengthLine(const long int id, const string namept1, const string namept2, const string namept3, const TAngle gis, const TLength obs, const TLength calc, const TLength res, const TLength sig, const TAngle v0, const string com);
		void	formatConstraintLine(const long int id, const string namept1, const string namept2, const string namept3, const TAngle bear, const TLength res, const TLength sig, const string com);
	//@}


/*@}*/
};

#endif // SU_MES_FILE_WRITER
