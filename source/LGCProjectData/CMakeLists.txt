include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .h files:
set(LGCProjectData_HEADERS 	
	${LGC2_INCLUDE_PATH}/tree.h
	${LGC2_INCLUDE_PATH}/TTreeEntry.h
	${LGC2_INCLUDE_PATH}/TLSConstraintIdentifier.h
	${LGC2_INCLUDE_PATH}/TLGCStatistic.h
	${LGC2_INCLUDE_PATH}/TLGCRefFrame.h
	${LGC2_INCLUDE_PATH}/TLGCData.h
	${LGC2_INCLUDE_PATH}/TLGCConfig.h
	${LGC2_INCLUDE_PATH}/TInstrumentData.h
	${LGC2_INCLUDE_PATH}/TLGCAngle.h
	${LGC2_INCLUDE_PATH}/TDataAnalyzer.h
	${LGC2_INCLUDE_PATH}/TFileLogger.h
)

# Create a variable containing all .cpp files:
set(LGCProjectData_SOURCES
	TLGCStatistic.cpp
	TLGCData.cpp
	TLGCAngle.cpp
	TDataAnalyzer.cpp
	TFileLogger.cpp

)


# Create a static library from sources:
add_library(LGCProjectData STATIC ${LGCProjectData_HEADERS} ${LGCProjectData_SOURCES})
