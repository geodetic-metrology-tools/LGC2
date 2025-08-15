/*
 * SPDX-FileCopyrightText: 2025 CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SU_TA_FILE_WRITER
#define SU_TA_FILE_WRITER


//STL
#include	<string>
//LGC
#include	<TALGCObjectWriter.h>

class		TAStreamFormatter;
class		TLGCData;

/*!
	\ingroup Writers
	\brief Abstract base class for LGC output file writers.
*/
class  TAFileWriter : public TALGCObjectWriter
{
public:

	/*!@name Constructors and Destructors*/
		//@{
			/// Constructor
			TAFileWriter(TAStreamFormatter* stream, const TLGCData* project);

			///Destructor
			virtual  ~TAFileWriter();
		//@}


	/*!@name Public member functions*/
		//@{

			/// virtual function to write the file with the errors
			virtual void	writeFile(const std::string error);

			///write error to the LGC output file
			void	writeError(const std::string error);

		    /// returns project data.
			const TLGCData*		getDataSet() const {return fProjectData;}
		//@}

protected:
	///default constructor
	TAFileWriter();

	/// Pointer to project data, including all the information necessary (configuration options, datasets, observations,...)
	const TLGCData* fProjectData;
};

/*@}*/

#endif // SU_TA_FILE_WRITER
