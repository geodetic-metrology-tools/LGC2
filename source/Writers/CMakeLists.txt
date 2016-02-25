include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/AdjustableObjects
					${CMAKE_SOURCE_DIR}/LGCCalculation
					${CMAKE_SOURCE_DIR}/LGCObjectWriters
					${CMAKE_SOURCE_DIR}/LGCProject
					${CMAKE_SOURCE_DIR}/LGCProjectData
					${CMAKE_SOURCE_DIR}/LocalTransformations
					${CMAKE_SOURCE_DIR}/Measurements
					${CMAKE_SOURCE_DIR}/Writers
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
