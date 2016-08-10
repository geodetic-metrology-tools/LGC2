include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/LGCAdjustableObjects
					${CMAKE_SOURCE_DIR}/ContributionsGenerators
					${CMAKE_SOURCE_DIR}/LGCCalculation
					${CMAKE_SOURCE_DIR}/LGCProject
					${CMAKE_SOURCE_DIR}/LGCProjectData
					${CMAKE_SOURCE_DIR}/Measurements
					${CMAKE_SOURCE_DIR}/LGCObjectWriters
					${CMAKE_SOURCE_DIR}/LocalTransformations
					${CMAKE_SOURCE_DIR}/Readers
					${CMAKE_SOURCE_DIR}/Writers
					)

# Create a variable containing all .h files:
set(LGCProject_HEADERS 	
	TLGCApp.h
	Global.h
	Defaults.h
	DoxygenModulesDefinitions.h
)

# Create a variable containing all .cpp files:
set(LGCProject_SOURCES
	main.cpp
	TLGCApp.cpp
)

# Create a static library from sources:
add_library(LGCProject STATIC ${LGCProject_HEADERS} ${LGCProject_SOURCES})

target_link_libraries(LGCProject Writers)