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


# Create a dynamic library from sources:
add_library(${PROJECT_NAME} SHARED ${API_HEADERS} ${API_SOURCES})
target_link_libraries(	${PROJECT_NAME}
	LGC_Core
	LSAlgorithms
	ProjectFramework
	SpatialObjects
	Logs
	Plugins
)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


# set default build type
#if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
#endif()
#message(${CMAKE_CURRENT_BINARY_DIR})
if(WIN32)
# how to access the build types set by visual studio to make sure the dll is copied to the debug/release folder of visual studio?
# for now: hard coded: Debug mode
 add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/Debug/LGC_API.dll" "${CMAKE_BINARY_DIR}/LGC_API.dll")
endif()

