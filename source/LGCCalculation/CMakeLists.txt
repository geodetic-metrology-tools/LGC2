include_directories(${LGC_MODULE_PATH}
					${LGC_MODULE_PATH}/AdjustableObjects
					${LGC_MODULE_PATH}/ContributionsGenerators
					${LGC_MODULE_PATH}/LGCProject
					${LGC_MODULE_PATH}/LGCProjectData
					${LGC_MODULE_PATH}/LSCalculation
					${LGC_MODULE_PATH}/Measurements
					${LGC_MODULE_PATH}/LGCObjectWriters
					${LGC_MODULE_PATH}/LocalTransformations
					${LGC_MODULE_PATH}/readers
					${LGC_MODULE_PATH}/Writers)

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
