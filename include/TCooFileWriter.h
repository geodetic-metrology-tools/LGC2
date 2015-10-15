////////////////////////////////////////////////////////////////////
// TCooFileWriter
/*!
Write a *.coo LGC output file
Creates the summary file from the data read and sends the appropriate messages

Patterns:
 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////////////////

#ifndef SU_TLGC_COO_FILE_WRITER
#define SU_TLGC_COO_FILE_WRITER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include	"TAFileWriter.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGC

@{*/

//Class definition
class  TCooFileWriter : public TAFileWriter  
{
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TCooFileWriter(TAStreamFormatter* stream, const TLGCProject* project);

			//!Destructor
			virtual  ~TCooFileWriter();
		//@}


	/*!@name Public menber functions*/
		//@{
			//!write the lgc file used when there's no error in the project
			virtual void	writeFile(TLGCDataSet*, LSCalcDataSet*);
		//@}

private:

	/*!default constructor*/
	TCooFileWriter();

	/*!@name Private member functions*/
		//@{
			void	writeTitle();

			void	writePointsHeader();

			void	writePoints();

			
			void	writeCooData(	LSPosVecConstIter posVecIterator,
								PointConstIter	pointIterator);

		//@}
	

/*@}*/
};

#endif // SU_TLGC_COO_FILE_WRITER