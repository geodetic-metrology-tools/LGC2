include_directories(${LGC_MODULE_PATH}
					${LGC_MODULE_PATH}/AdjustableObjects
					${LGC_MODULE_PATH}/ContributionsGenerators
					${LGC_MODULE_PATH}/LGCProject
					${LGC_MODULE_PATH}/LGCProjectData
					${LGC_MODULE_PATH}/Measurements
					${LGC_MODULE_PATH}/LGCObjectWriters
					${LGC_MODULE_PATH}/LocalTransformations
					${LGC_MODULE_PATH}/Readers
					${LGC_MODULE_PATH}/Writers)

# Create a variable containing all .h files:
set(LGCProjectData_HEADERS 	
	tree.h
	TTreeEntry.h
	TLGCStatistic.h
	TLGCData.h
	TLGCConfig.h
	TInstrumentData.h
	TDataAnalyzer.h
	TFileLogger.h
	TLSCalcRelativeError.h
)

# Create a variable containing all .cpp files:
set(LGCProjectData_SOURCES
	TLGCStatistic.cpp
	TLGCData.cpp
	TDataAnalyzer.cpp
	TFileLogger.cpp
	TLSCalcRelativeError.cpp
)

# Create a static library from sources:
add_library(LGCProjectData STATIC ${LGCProjectData_HEADERS} ${LGCProjectData_SOURCES})
