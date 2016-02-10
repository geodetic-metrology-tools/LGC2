include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/AdjustableObjects
					${CMAKE_SOURCE_DIR}/LGCProject
					${CMAKE_SOURCE_DIR}/LGCProjectData
					${CMAKE_SOURCE_DIR}/Measurements
					${CMAKE_SOURCE_DIR}/LGCObjectWriters
					${CMAKE_SOURCE_DIR}/LocalTransformations
					${CMAKE_SOURCE_DIR}/readers
					${CMAKE_SOURCE_DIR}/Writers)

# Create a variable containing all .h files:
set(ContributionsGenerators_HEADERS 	
	TContributionsGenerator.h
	ContributionStructures.h
	TPointTransformer.h
	TObservationGenerator.h
	TAllfixedParamGenerator.h
	TLibrCnstrGenerator.h
)

# Create a variable containing all .cpp files:
set(ContributionsGenerators_SOURCES
	TContributionsGenerator.cpp
	TPointTransformer.cpp
	TObservationGenerator.cpp
	TAllfixedParamGenerator.cpp
	TLibrCnstrGenerator.cpp
)


# Create a static library from sources:
add_library(ContributionsGenerators STATIC ${ContributionsGenerators_HEADERS} ${ContributionsGenerators_SOURCES})
# Link static libraries to the current one
target_link_libraries(ContributionsGenerators MathematicalConcepts)

