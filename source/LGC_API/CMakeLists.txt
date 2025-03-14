PROJECT (LGC_API)

# Do not use the serializer in the API
set(USE_SERIALIZER 0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers/)

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

target_link_libraries(${PROJECT_NAME}
	LGC_Core
	LSAlgorithms
	ProjectFramework
	SpatialObjects
	Logs
	Plugins
)

if(NOT MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE -D_GLIBCXX_USE_CXX11_ABI=0)
endif()

# Find OpenMP
find_package(OpenMP)
if (OPENMP_FOUND)
    target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)
endif()

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# Ensure correct DLL copying on Windows, independent of build type
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
        "$<TARGET_FILE:${PROJECT_NAME}>" 
        "${CMAKE_BINARY_DIR}/LGC_API.dll")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
        "$<TARGET_FILE:${PROJECT_NAME}>" 
        "${CMAKE_BINARY_DIR}/LGC_Tests/LGC_API.dll")
endif()
