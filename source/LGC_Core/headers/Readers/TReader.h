#ifndef _READERS_READER_H_
#define _READERS_READER_H_

#include "ReaderBase.h"
#include "TLGCData.h"

class TAKeyWord;

/*! 
	\ingroup InputFileReader
	\brief Class responsible for the reading process.
*/
class TReader {
	public:
		TReader(std::shared_ptr<TLGCData> project);

		/**
			\brief Reads the input file and extracts options, instruments, points, frames and measurements into the TLGCData class 
			that was passed in the constructor.

			\return TRUE if no error occurred during reading the file, FALSE otherwise.

			If the keyword *FRAME is not used the tree will only consist of a root node.
		*/
		bool read(std::istream& lgcStream, std::istream& cp_lgcStream);
		bool readLgc1File(std::istream& lgcStream);
		bool isLgc2File(std::istream& lgcStream);
		bool requiredAdjustableVo(std::istream& lgcStream, int nline);


	private:
		/// All lgc2 keyword handler objects.
		std::vector< std::unique_ptr<TAKeyWord> > finterpreters;

		/// All lgc1 keyword handler objects.
		std::vector< std::unique_ptr<TAKeyWord> > finterpreters_lgc1;

		/// The reference to the project passed in the constructor is stored
		TLGCData& project;
		
		/// Not asignable, contains reference
		TReader& operator=(const TReader&);

		
};
#endif
