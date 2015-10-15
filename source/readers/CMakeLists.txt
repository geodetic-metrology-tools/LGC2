include_directories(${CMAKE_SOURCE_DIR})

set(READER_HEADERS
	AdjObjectsReader.h
	InstrumentReaders.h
	TReader.h
	MeasurementReaders.h
	OptionReaders.h
	ReaderBase.h
)

set(READER_SOURCES
	TReader.cpp
	ReaderInit.cpp
	AdjObjectsReader.cpp
	InstrumentReaders.cpp
	MeasurementReaders.cpp
	OptionReaders.cpp
)

add_library(Readers STATIC ${READER_HEADERS} ${READER_SOURCES})
