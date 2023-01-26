PROJECT (LGC_API)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers/
)

SET(API_INCLUDE_PATH 	${CMAKE_CURRENT_SOURCE_DIR}/headers/)
SET(API_SOURCES_PATH 	${CMAKE_CURRENT_SOURCE_DIR}/sources)
					
					
# Create a variable containing all .h files:
set(API_HEADERS 	
	${API_INCLUDE_PATH}/Moni.h
	${API_INCLUDE_PATH}/MoniImpl.h
)

# Create a variable containing all .cpp files:
set(API_SOURCES
	${API_SOURCES_PATH}/MoniImpl.cpp
)


# Define VS folders:
file(GLOB API_H ${API_INCLUDE_PATH}/*.*)

file(GLOB API_CPP ${API_SOURCES_PATH}/*.*)

source_group("\\Headers" FILES ${API_H})

source_group("\\Sources" FILES ${API_CPP})


# Create a static library from sources:
add_library(${PROJECT_NAME} SHARED ${API_HEADERS} ${API_SOURCES})
target_link_libraries(	${PROJECT_NAME}
	LGC_Core
	LSAlgorithms
	ProjectFramework
	SpatialObjects
	Logs
	Plugins
)
# Link static libraries to the current one
#target_link_libraries(LGC_Core  LSAlgorithms ProjectFramework SpatialObjects Tools)
