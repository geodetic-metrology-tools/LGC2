////////////////////////////////////////////////////////////////////
// TAFileWriter
/*!
	Abstract base class for LGC output file writers.
*/
#ifndef SU_TA_FILE_WRITER
#define SU_TA_FILE_WRITER
////////////////////////////////////////////////////////////////
// Forward declarations
#include	<string>
class		TAStreamFormatter;
class		TLGCData;
#include	"LGCConverters\TALGCConverter.h"



//Class definition
class  TAFileWriter : public TALGCConverter
{
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TAFileWriter(TAStreamFormatter* stream, const TLGCData* project);

			//!Destructor
			virtual  ~TAFileWriter();
		//@}


	/*!@name Public member functions*/
		//@{
			//!write the lgc file used when there's error in the project
			virtual void	writeFile(const string error);
	
			/*!\return the writer's error as a string*/
			virtual string	getError() const;
			//! write error
			virtual void	writeError();
			//! set the error string
			virtual void	setError(const string error);
	
		    //!Returns project data.*/
			const TLGCData*		getDataSet() const {return fProjectData;}
		//@}

protected:
		
	/*!default constructor*/
	TAFileWriter();

	/*! Pointer to project data, including all the information necessary (configuration options, datasets, observations,...)*/
	const TLGCData* fProjectData;

	//!Error
	string							fError;
};

/*@}*/

#endif // SU_TA_FILE_WRITER
