#ifndef SU_FAUT_WRITER
#define SU_FAUT_WRITER

class	TLGCData;
class	TAStreamFormatter;
#include "TAFileWriter.h"
#include <string>
using namespace std;

/*!
	\ingroup Writers
	\brief Write an additional LGC output file for FAUT detection.
*/
class TFautFileWriter : TAFileWriter
{

public:
	/*@name Constructor / Destructor */
	//@{
		/// Default constructor 
		TFautFileWriter(TAStreamFormatter* stream, const TLGCData* project);


		/// Destructor 
		virtual ~TFautFileWriter();
	//@}


	/*!@name Public member functions*/
	//@{
		/// write the point coordinate file corresponding to the given project
		virtual void writeFile(TLGCData*);

		///write the lgc file  when there is an error in the project
		virtual void writeFile(const string error);
	//@}

private:
	
	///default constructor 
	TFautFileWriter();

	/*!@name Private functions for header part of the file */
	//@{
		/// write title and date 
		void	writeTitle();
		/// write data summary (sigma zero a posteriori, alpha, beta, and others) 
		void	writeDataSummary();

	//@}

	/// Total reliability if possible 
		void	writeOverallReliability(TLGCData* project);

	/*!@name Private Attribute*/
	//@{
		TReal	fAlpha;	/*! level of significance, [0;1], default value : 1% */
		TReal	fBeta; /*! risk of having false values, [0;1], default value : 10% */
	//@}

};

/*@}*/

#endif //SU_FAUT_WRITER