include_directories(${LGC_MODULE_PATH}
					${LGC_MODULE_PATH}/AdjustableObjects
					${LGC_MODULE_PATH}/LGCProject
					${LGC_MODULE_PATH}/LGCProjectData
					${LGC_MODULE_PATH}/Measurements
					${LGC_MODULE_PATH}/LGCObjectWriters
					${LGC_MODULE_PATH}/LocalTransformations)

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
