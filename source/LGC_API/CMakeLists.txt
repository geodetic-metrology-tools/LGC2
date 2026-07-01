PROJECT (LGC_API)

# Do not use the serializer in the API
set(USE_SERIALIZER 0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers/)

SET(API_INCLUDE_PATH 	${CMAKE_CURRENT_SOURCE_DIR}/headers/)
SET(API_SOURCES_PATH 	${CMAKE_CURRENT_SOURCE_DIR}/sources)
					
# Create a variable containing all .h files:
set(API_HEADERS
	${API_INCLUDE_PATH}/Monitor.h       # header-only C++ facade over the C ABI (shipped)
	${API_INCLUDE_PATH}/MonitorCore.h   # internal implementation class (not exported)
	${API_INCLUDE_PATH}/MonitorImpl.h
	${API_INCLUDE_PATH}/Monitor_C.h     # flat C ABI (shipped)
)

# Create a variable containing all .cpp files:
set(API_SOURCES
	${API_SOURCES_PATH}/MonitorImpl.cpp
	# The flat C ABI (moni*) is the ONLY exported surface of the DLL; both the
	# C++ facade (Monitor.h) and the ctypes Python wrapper sit on top of it.
	${API_SOURCES_PATH}/Monitor_C.cpp
)

# Define VS folders:
file(GLOB API_H ${API_INCLUDE_PATH}/*.*)
file(GLOB API_CPP ${API_SOURCES_PATH}/*.*)

source_group("\\Headers" FILES ${API_H})
source_group("\\Sources" FILES ${API_CPP})

# Create a dynamic library from sources:
add_library(${PROJECT_NAME} SHARED ${API_HEADERS} ${API_SOURCES})

# Export the flat C ABI (moni* functions) — the only exported surface of this DLL.
target_compile_definitions(${PROJECT_NAME} PRIVATE MONITOR_C_EXPORTS)

target_link_libraries(${PROJECT_NAME}
	LGC_Core
	LSAlgorithms
	ProjectFramework
	SpatialObjects
	Logs
	Plugins
)

##if(NOT MSVC)
##  target_compile_options(${PROJECT_NAME} PRIVATE -D_GLIBCXX_USE_CXX11_ABI=0)
##endif()

# Find OpenMP
find_package(OpenMP)
if (OPENMP_FOUND)
    target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)
endif()

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Python C-ABI bindings for the monitoring API (plain shared library — no
# Python or pybind11 needed to build). Mirrors source/pyLGC.
ADD_SUBDIRECTORY(python)


