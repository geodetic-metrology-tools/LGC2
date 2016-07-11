include_directories(${LGC_MODULE_PATH}
					${LGC_MODULE_PATH}/AdjustableObjects
					${LGC_MODULE_PATH}/ContributionsGenerators
					${LGC_MODULE_PATH}/LGCCalculation
					${LGC_MODULE_PATH}/LGCProject
					${LGC_MODULE_PATH}/LGCProjectData
					${LGC_MODULE_PATH}/Measurements
					${LGC_MODULE_PATH}/LGCObjectWriters
					${LGC_MODULE_PATH}/LocalTransformations
					${LGC_MODULE_PATH}/Readers
					${LGC_MODULE_PATH}/Writers
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