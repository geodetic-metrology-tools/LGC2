# Create a variable containing all .cpp files:
set(ReaderAndWriter_SOURCES
	TAFileWriter.cpp
	TCooFileWriter.cpp
	TDefAFileWriter.cpp
	TFautFileWriter.cpp
	TLGCFileReader.cpp
	TMesFileWriter.cpp
	TPunchFileWriter.cpp
	TRadFileWriter.cpp
	TResSimFileWriter.cpp
	TResultsFileWriter.cpp
	TSimFileWriter.cpp
)

# Create a variable containing all .h files:
set(ReaderAndWriter_HEADERS 	
	${LGC_INCLUDE_PATH}/TAFileWriter.h
	${LGC_INCLUDE_PATH}/TCooFileWriter.h
	${LGC_INCLUDE_PATH}/TDefAFileWriter.h
	${LGC_INCLUDE_PATH}/TFautFileWriter.h
	${LGC_INCLUDE_PATH}/TLGCFileReader.h
	${LGC_INCLUDE_PATH}/TMesFileWriter.h
	${LGC_INCLUDE_PATH}/TPunchFileWriter.h
	${LGC_INCLUDE_PATH}/TRadFileWriter.h
	${LGC_INCLUDE_PATH}/TResSimFileWriter.h
	${LGC_INCLUDE_PATH}/TResultsFileWriter.h
	${LGC_INCLUDE_PATH}/TSimFileWriter.h
)

# Create a static library from sources:
add_library(ReaderAndWriter STATIC ${ReaderAndWriter_SOURCES} ${ReaderAndWriter_HEADERS})