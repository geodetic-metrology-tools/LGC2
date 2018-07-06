/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
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
