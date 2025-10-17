/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SU_INPUT_FILE_WRITER
#define SU_INPUT_FILE_WRITER
#if _MSC_VER >= 1000
#pragma once
#endif 

//LGC
#include <TSimFileWriter.h>

class	TLGCProject;


/*!
\ingroup Writers
\brief Write an LGC input file from the virtual project data.
*/
class  TInputFileWriter : public TSimFileWriter  
{
public:

	/*!@name Constructors and Destructors*/
		//@{
			/// constructor
			TInputFileWriter(TAStreamFormatter* stream, const TLGCData* project);

			/// Destructor
			virtual  ~TInputFileWriter();
		//@}


	/*!@name Public member functions*/
		//@{
			/// write the lgc file used when there's no error in the project
			virtual void	writeFile();

			/// write the lgc file  when there is an error in the project
			virtual void	writeFile(const std::string error);
		//@}

protected:

	/*!default constructor*/
	TInputFileWriter();

	/// write the title, the referential and the options
	virtual void writeHeader();

};

/*@}*/

#endif // SU_INPUT_FILE_WRITER
