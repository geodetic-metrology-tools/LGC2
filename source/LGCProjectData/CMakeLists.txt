include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .h files:
set(LGCProjectData_HEADERS 	
	tree.h
	TTreeEntry.h
	TLSConstraintIdentifier.h
	TLGCStatistic.h
	TLGCRefFrame.h
	TLGCData.h
	TLGCConfig.h
	TInstrumentData.h
	TLGCAngle.h
	TDataAnalyzer.h
	TFileLogger.h
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
