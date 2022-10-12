include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/LGC_API/headers/
)

SET(API_INCLUDE_PATH 	${PROJECT_SOURCE_DIR}/LGC_API/headers)
SET(API_SOURCES_PATH 	${PROJECT_SOURCE_DIR}/LGC_API/sources)
					
					
# Create a variable containing all .h files:
set(API_HEADERS 	
	${API_INCLUDE_PATH}/TDummy.h
)

# Create a variable containing all .cpp files:
set(API_SOURCES
	${API_SOURCES_PATH}/TDummy.cpp
)


# Define VS folders:
file(GLOB API_H ${API_INCLUDE_PATH}/*.*)

file(GLOB API_CPP ${API_SOURCES_PATH}/*.*)

source_group("\\Headers" FILES ${API_H})

source_group("\\Sources" FILES ${API_CPP})


# Create a static library from sources:
add_library(LGC_API STATIC ${API_HEADERS} ${API_SOURCES})
# Link static libraries to the current one
#target_link_libraries(LGC_Core  LSAlgorithms ProjectFramework SpatialObjects Tools)
