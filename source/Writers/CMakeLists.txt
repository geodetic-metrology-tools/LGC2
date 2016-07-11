include_directories(${LGC_MODULE_PATH}
					${LGC_MODULE_PATH}/AdjustableObjects
					${LGC_MODULE_PATH}/LGCCalculation
					${LGC_MODULE_PATH}/LGCObjectWriters
					${LGC_MODULE_PATH}/LGCProject
					${LGC_MODULE_PATH}/LGCProjectData
					${LGC_MODULE_PATH}/LocalTransformations
					${LGC_MODULE_PATH}/Measurements
					${LGC_MODULE_PATH}/Writers
					)

# Create a variable containing all .cpp files:
set(Writers_SOURCES
	TAFileWriter.cpp
	TResultsFileWriter.cpp
	TPunchFileWriter.cpp
	TFautFileWriter.cpp
	TSimulationOutputFileWriter.cpp
	TSimFileWriter.cpp
	TDefaFileWriter.cpp
)

# Create a variable containing all .h files:
set(Writers_HEADERS 	
	TAFileWriter.h
	TResultsFileWriter.h
	TPunchFileWriter.h
	TFautFileWriter.h
	TSimulationOutputFileWriter.h
	TSimFileWriter.h
	TDefaFileWriter.h
)

# Create a static library from sources:
add_library(Writers STATIC ${Writers_SOURCES} ${Writers_HEADERS})


# Link static libraries to the current one
target_link_libraries(Writers LGCObjectWriters Converters)
