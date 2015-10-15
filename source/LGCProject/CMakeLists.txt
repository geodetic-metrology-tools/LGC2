include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .h files:
set(LGCProject_HEADERS 	
	${LGC2_INCLUDE_PATH}/TLGCApp.h
	${LGC2_INCLUDE_PATH}/Global.h
	${LGC2_INCLUDE_PATH}/Defaults.h
	${LGC2_INCLUDE_PATH}/DoxygenModulesDefinitions.h
)

# Create a variable containing all .cpp files:
set(LGCProject_SOURCES
	TLGCApp.cpp
)


# Create a static library from sources:
add_library(LGCProject STATIC ${LGCProject_HEADERS} ${LGCProject_SOURCES})
