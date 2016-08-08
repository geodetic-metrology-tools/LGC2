include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/LGCAdjustableObjects
					${CMAKE_SOURCE_DIR}/LGCProject
					${CMAKE_SOURCE_DIR}/LGCProjectData
					${CMAKE_SOURCE_DIR}/Measurements
					${CMAKE_SOURCE_DIR}/LGCObjectWriters
					${CMAKE_SOURCE_DIR}/LocalTransformations)

set(READER_HEADERS
	AdjObjectsReader.h
	InstrumentReaders.h
	TReader.h
	MeasurementReaders.h
	OptionReaders.h
	ReaderBase.h
	KeywordRights.h
	MeasurementReader_lgc1.h
)

set(READER_SOURCES
	TReader.cpp
	AdjObjectsReader.cpp
	InstrumentReaders.cpp
	MeasurementReaders.cpp
	OptionReaders.cpp
	MeasurementReader_lgc1.cpp
)

add_library(Readers STATIC ${READER_HEADERS} ${READER_SOURCES})
