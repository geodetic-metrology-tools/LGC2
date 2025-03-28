
cmake_minimum_required(VERSION 3.12)
PROJECT (pyLGC)

set(CMKE_CXX_STANDARD 14)
set(pybind11_DIR "C:/Users/mcarotta/AppData/Local/Programs/Python/Python313/Lib/site-packages/pybind11/share/cmake/pybind11")
find_package(pybind11 REQUIRED)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers/)
include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCAdjustableObjects
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCConstraintObjects
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/ContributionsGenerators
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCCalculation
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCObjectWriters
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCProject
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCProjectData
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LocalTransformations
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LSCalculation
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/Measurements
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/readers
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/Writers
					${CMAKE_SOURCE_DIR}/LGC_Tests/headers/
					)

SET(pyLGC_INCLUDE_PATH 	${CMAKE_CURRENT_SOURCE_DIR}/headers/)
SET(pyLGC_SOURCES_PATH 	${CMAKE_CURRENT_SOURCE_DIR}/sources/)
					
					
# Create a variable containing all .h files:
set(pyLGC_HEADERS 	
    ${pyLGC_INCLUDE_PATH}/../../LGC_Core/headers/LSCalculation/TLSEvaluator.h
)

# Create a variable containing all .cpp files:
set(pyLGC_SOURCES
    ${pyLGC_SOURCES_PATH}/../../LGC_Core/sources/LSCalculation/TLSEvaluator.cpp
	${pyLGC_SOURCES_PATH}/bindings.cpp
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




