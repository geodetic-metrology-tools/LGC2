include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/AdjustableObjects
					${CMAKE_SOURCE_DIR}/ContributionsGenerators
					${CMAKE_SOURCE_DIR}/LGCProject
					${CMAKE_SOURCE_DIR}/LGCProjectData
					${CMAKE_SOURCE_DIR}/LSCalculation
					${CMAKE_SOURCE_DIR}/Measurements
					${CMAKE_SOURCE_DIR}/LGCObjectWriters
					${CMAKE_SOURCE_DIR}/LocalTransformations
					${CMAKE_SOURCE_DIR}/readers
					${CMAKE_SOURCE_DIR}/Writers)

# Create a variable containing all .h files:
set(LGCCalculation_HEADERS 	
	TLGCCalculation.h
	TSimFrameSummary.h
	TSimPointSummary.h
	TVAbractAlgorithm.h
)

# Create a variable containing all .cpp files:
set(LGCCalculation_SOURCES
	TLGCCalculation.cpp
	TSimPointSummary.cpp
	TSimFrameSummary.cpp
)

# Create a static library from sources:
add_library(LGCCalculation STATIC ${LGCCalculation_HEADERS} ${LGCCalculation_SOURCES})

# Link static libraries to the current one
target_link_libraries(LGCCalculation LGCProjectData)
