include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .h files:
set(LGCCalculation_HEADERS 	
	${LGC2_INCLUDE_PATH}/TLGCCalculation.h
	${LGC2_INCLUDE_PATH}/TSimulationCalculation.h
	${LGC2_INCLUDE_PATH}/TSimFrameSummary.h
	${LGC2_INCLUDE_PATH}/TSimPointSummary.h
)

# Create a variable containing all .cpp files:
set(LGCCalculation_SOURCES
	TLGCCalculation.cpp
	TSimulationCalculation.cpp
	TSimPointSummary.cpp
	TSimFrameSummary.cpp
)


# Create a static library from sources:
add_library(LGCCalculation STATIC ${LGCCalculation_HEADERS} ${LGCCalculation_SOURCES})
# Link static libraries to the current one
target_link_libraries(LGCCalculation LGCProjectData)
