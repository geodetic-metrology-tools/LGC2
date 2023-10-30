
cmake_minimum_required(VERSION 3.12)
PROJECT (pyLib)

set(CMKE_CXX_STANDARD 17)
find_package(pybind11 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers/)

SET(pyLib_INCLUDE_PATH 	${CMAKE_CURRENT_SOURCE_DIR}/headers/)
SET(pyLib_SOURCES_PATH 	${CMAKE_CURRENT_SOURCE_DIR}/sources/)
					
					
# Create a variable containing all .h files:
set(pyLib_HEADERS 	
	${pyLib_INCLUDE_PATH}/LGCController.h
)

# Create a variable containing all .cpp files:
set(pyLib_SOURCES
	${pyLib_SOURCES_PATH}/LGCController.cpp
	${pyLib_SOURCES_PATH}/binding.cpp
)


# Define VS folders:
file(GLOB pyLib_H ${pyLib_INCLUDE_PATH}/*.*)

file(GLOB pyLib_CPP ${pyLib_SOURCES_PATH}/*.*)

source_group("\\Headers" FILES ${pyLib_H})

source_group("\\Sources" FILES ${pyLib_CPP})




# Create python library from sources:
add_library(${PROJECT_NAME} MODULE ${pyLib_SOURCES} ${pyLib_HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::module
	LGC_Core
	LSAlgorithms
	ProjectFramework
	SpatialObjects
	Logs
	Plugins
)

get_target_property(dirs ${PROJECT_NAME} INCLUDE_DIRECTORIES)
message(STATUS "Include directories of pyLib: ${dirs}")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()



