////////////////////////////////////////////////////////////////////
// TRadFileWriter
/*!
Write an LGC RAD file - a summary of the radial offset constraints
Creates a file from the calculation results and sends the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////////////////

#ifndef SU_RAD_FILE_WRITER
#define SU_RAD_FILE_WRITER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
class	TLGCDataSet;
class	TAStreamFormatter;
class	LSCalcDataSet;
class	TLGCProject;
#include	"TAFileWriter.h"
// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup ????

@{*/

//Class definition
class  TRadFileWriter : public TAFileWriter  
{
public:

	using TAFileWriter::writeFile;

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TRadFileWriter(TAStreamFormatter* stream, const TLGCProject* project);

			//!Destructor
			virtual  ~TRadFileWriter();
		//@}


	/*!@name Public member functions*/
		//@{
			//!write the lgc file used when there's no error in the project
			virtual void	writeFile(TLGCDataSet*, LSCalcDataSet*);
		//@}

private:

	/*!default constructor*/
	TRadFileWriter();

	/*!@name Private member functions*/
		//@{
			void	writeRadOffCnstr();
		//@}
	

/*@}*/
};

#endif // SU_RAD_FILE_WRITER
