include_directories(${LGC_MODULE_PATH}
					${LGC_MODULE_PATH}/AdjustableObjects
					${LGC_MODULE_PATH}/LGCProject
					${LGC_MODULE_PATH}/LGCProjectData
					${LGC_MODULE_PATH}/Measurements
					${LGC_MODULE_PATH}/LGCObjectWriters
					${LGC_MODULE_PATH}/LocalTransformations
					${LGC_MODULE_PATH}/readers
					${LGC_MODULE_PATH}/Writers)

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
target_link_libraries(ContributionsGenerators LocalTransformations MathematicalConcepts)

