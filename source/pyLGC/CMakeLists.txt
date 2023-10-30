
cmake_minimum_required(VERSION 3.12)
PROJECT (pyLGC)

set(CMKE_CXX_STANDARD 14)
find_package(pybind11 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers/)

SET(pyLGC_INCLUDE_PATH 	${CMAKE_CURRENT_SOURCE_DIR}/headers/)
SET(pyLGC_SOURCES_PATH 	${CMAKE_CURRENT_SOURCE_DIR}/sources/)
					
					
# Create a variable containing all .h files:
set(pyLGC_HEADERS 	
	${pyLGC_INCLUDE_PATH}/LGCController.h
)

# Create a variable containing all .cpp files:
set(pyLGC_SOURCES
	${pyLGC_SOURCES_PATH}/LGCController.cpp
	${pyLGC_SOURCES_PATH}/binding.cpp
)


# Define VS folders:
file(GLOB pyLGC_H ${pyLGC_INCLUDE_PATH}/*.*)

file(GLOB pyLGC_CPP ${pyLGC_SOURCES_PATH}/*.*)

source_group("\\Headers" FILES ${pyLGC_H})

source_group("\\Sources" FILES ${pyLGC_CPP})




# Create python library from sources:
add_library(${PROJECT_NAME} MODULE ${pyLGC_SOURCES} ${pyLGC_HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::module
	LGC_Core
	LSAlgorithms
	ProjectFramework
	SpatialObjects
	Logs
	Plugins
)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pyd")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()



