#ifndef _READERS_READER_H_
#define _READERS_READER_H_

#include "ReaderBase.h"
#include "TLGCData.h"

class TAKeyWord;

class TReader {
	public:
		TReader(TLGCData* project);

		/**
			Reads a configuration and extracts options, instruments, 
			points, frames and measurements into the LGCData object that was passed in the constructor. Second parameter is a name of the ouput file into which are 
			errors recorded.

			\return TRUE if no error occurred during reading the file, FALSE otherwise

			If the keyword *FRAME is not used the tree will only consist of a root node.
		*/
		bool read(std::istream& lgcStream);

		/// Splits a line into its tokens, removing blank characters
		static std::vector<const std::string> const tokenizeLGCfileString(const std::string& str);

	private:
		// all keyword handler objects
		std::vector< std::unique_ptr<TAKeyWord> > finterpreters;

		// keep the project that is passed in the constructor
		TLGCData& project;
		
		// Not asignable
		TReader& operator=(const TReader&);
};

#endif
