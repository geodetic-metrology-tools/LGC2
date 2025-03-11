#WARNING:   copied from the Evaluator branch on the 10 march 2025. Could need some additional correction.

cmake_minimum_required(VERSION 3.12)
PROJECT (pyLGC)

set(CMAKE_CXX_STANDARD 14)

# Find pybind11 package (#TODO need to be understood how to done it ine general for other computers, because on my machine i need to set it manually)
set(pybind11_DIR "C:/Users/mcarotta/AppData/Local/Programs/Python/Python313/Lib/site-packages/pybind11/share/cmake/pybind11")
find_package(pybind11 REQUIRED)

# Find Eigen package (#TODO need to be understood how to done it ine general for other computers, because on my machine i need to set it manually)
#set(Eigen3_DIR "C:\\dev\\vcpkg\\packages\\eigen3_x64-windows\\share\\eigen3")
#find_package(Eigen3 REQUIRED)

# Add headers folder to include paths
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers/
	${EIGEN_INCLUDE_PATH}
)

# Set paths for sources and headers
SET(pyLGC_INCLUDE_PATH 	${CMAKE_CURRENT_SOURCE_DIR}/headers/)
SET(pyLGC_SOURCES_PATH 	${CMAKE_CURRENT_SOURCE_DIR}/sources/)

# Define the header files
set(pyLGC_HEADERS 
    ${pyLGC_INCLUDE_PATH}/TLSEvaluator.h
)

# Define the source files
set(pyLGC_SOURCES
    ${pyLGC_SOURCES_PATH}/TLSEvaluator.cpp
    ${pyLGC_SOURCES_PATH}/bindings.cpp
)

# Add sources for Python module
file(GLOB pyLGC_H ${pyLGC_INCLUDE_PATH}/*.*)
file(GLOB pyLGC_CPP ${pyLGC_SOURCES_PATH}/*.*)

# Group the files in Visual Studio (if using VS)
source_group("\\Headers" FILES ${pyLGC_H})
source_group("\\Sources" FILES ${pyLGC_CPP})

# Create the Python library from sources
add_library(${PROJECT_NAME} MODULE ${pyLGC_SOURCES} ${pyLGC_HEADERS})

# Link libraries that pybind11 and other dependencies require
target_link_libraries(${PROJECT_NAME} PRIVATE 
    pybind11::module
    LGC_Core          # Assuming these are external libraries your project depends on
    LSAlgorithms
    ProjectFramework
    SpatialObjects
    Logs
    Plugins
    #Eigen3::Eigen    # Link Eigen (if required by your project)
)

# Set the target properties (e.g., create .pyd file for Python)
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pyd")

# If OpenMP is required (optional)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

