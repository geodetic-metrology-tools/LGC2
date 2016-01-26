include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/AdjustableObjects
					${CMAKE_SOURCE_DIR}/ContributionsGenerators
					${CMAKE_SOURCE_DIR}/LGCProjectData
					${CMAKE_SOURCE_DIR}/LocalTransformations
					${CMAKE_SOURCE_DIR}/Measurements)

# Create a variable containing all .h files:
set(LSCalculation_HEADERS 	
	TLSInputMatricesFiller.h
	TLSResultsMatricesExtractor.h
	TLSAlgorithm.h
	TLSSimulation.h
	TLSAllfixed.h
)

# Create a variable containing all .cpp files:
set(LSCalculation_SOURCES
	TLSInputMatricesFiller.cpp
	TLSResultsMatricesExtractor.cpp
	TLSAlgorithm.cpp
	TLSSimulation.cpp
	TLSAllfixed.cpp
)

# Create a static library from sources:
add_library(LSCalculation STATIC ${LSCalculation_HEADERS} ${LSCalculation_SOURCES})

# Link static libraries to the current one
target_link_libraries(LSCalculation CalcElements)