# Mandatory in order to set the devtoolset2 (redhat specific) where compilers gcc & g++ are located
if(UNIX)
	SET(CMAKE_C_COMPILER   "/opt/rh/devtoolset-2/root/usr/bin/gcc")
	SET(CMAKE_CXX_COMPILER "/opt/rh/devtoolset-2/root/usr/bin/g++")
endif()

# Declare a project
PROJECT(LGC2)

# Set version number and provide a header file with corresponding defines
set (LGC_VERSION_MAJOR \"2\")
set (LGC_VERSION_MINOR \"00\")
set (LGC_VERSION_PATCH \"08\")

# Allows c++ 11 standard
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
endif()

# Versions files
configure_file (
  "${CMAKE_SOURCE_DIR}/LGCVersion.in"
  "${CMAKE_SOURCE_DIR}/Version.h"
)

# Minimum version of CMake
cmake_minimum_required(VERSION 2.8)
CMAKE_POLICY(SET CMP0015 OLD)

# We need some CMake macros
INCLUDE(CheckIncludeFileCXX)

# Enable languages
enable_language("CXX")

# Set the default build type:
SET(CMAKE_BUILD_TYPE DEBUG)

#######################################################################
# Define Doxigen configuration if exists
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

# REQ: BOOST_ROOT VCREDIST_INSTALLER_PATH SURVEYLIB_ROOT SURVEYLIB_LIBRARY_DIR EIGEN_INCLUDE_PATH
INCLUDE(../../ext_libs.txt)

# Make surveylib sublibraries avaliable
SET(SURVEYLIB_ROOT ${PROJECT_SOURCE_DIR}/../SurveyLib)

FIND_PACKAGE(Boost 1.49.0 REQUIRED)

IF(MSVC)
	ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
	SET(CMAKE_CXX_FLAGS "/openmp /EHsc /O2 /W4  /wd4224")
ENDIF(MSVC)

# Add options to compile with GCC to get more opinions on the code
IF(MINGW)
message("    Using MinGW")
	ADD_DEFINITIONS(-D_USE_MATH_DEFINES) #defined variable added FRK 17/11/2016
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -Wno-reorder -Wfatal-errors -Wno-comments")  
ENDIF(MINGW)

INCLUDE("${SURVEYLIB_ROOT}/source/SourceSurveyLib.cmake")

include_directories (
	SYSTEM
	${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCAdjustableObjects
	${CMAKE_SOURCE_DIR}/LGC_Core/headers/LSCalculation
	${CMAKE_SOURCE_DIR}/LGC_Core/headers/Readers
	${CMAKE_SOURCE_DIR}/LGC_Core/headers/ContributionsGenerators
	${CMAKE_SOURCE_DIR}/LGC_Core/headers/Measurements
	${CMAKE_SOURCE_DIR}/LGC_Core/headers/LocalTransformations
	${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCProject
	${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCProjectData
	${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCCalculation
	${CMAKE_SOURCE_DIR}/LGC_Core/headers/Writers
	${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCObjectWriters
	${CMAKE_SOURCE_DIR}/LGC_Tests/headers
	${Boost_INCLUDE_DIRS}
	${SURVEYLIB_ROOT}/source/LSAlgorithms/headers/AdjustableObjects
	${SURVEYLIB_ROOT}/source/LSAlgorithms/headers/CalcElements
	${SURVEYLIB_ROOT}/source/LSAlgorithms/headers/CalcParams
	${SURVEYLIB_ROOT}/source/ProjectFramework/headers/Converters
	${SURVEYLIB_ROOT}/source/ProjectFramework/headers/FiltersAndFormats
	${SURVEYLIB_ROOT}/source/ProjectFramework/headers/ProjectData
	${SURVEYLIB_ROOT}/source/ProjectFramework/headers/StreamFormatters
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/AffineTransformations
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/MathematicalConcepts
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/ReferenceSurfaces
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/RefFrameTransformations
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/ShortestPath
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/SpatialMeasurments
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/SpatialObj
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/SpatialSystems
	${SURVEYLIB_ROOT}/source/Tools/headers
	${TUT_INCLUDE_PATH}
	${EIGEN_INCLUDE_PATH}
)


# add subdirectories
ADD_SUBDIRECTORY(LGC_Core)
ADD_SUBDIRECTORY(LGC_Tests)
#ADD_SUBDIRECTORY(LGC_UI)


#include the CPack file allows to have PACKAGE in the solution and then to create the package
include(${PROJECT_SOURCE_DIR}/CPack.cmake)

# INSTALL for creation of the LGC2 installer
add_executable(LGC ${PROJECT_SOURCE_DIR}/LGC_Core/sources/LGCProject/main.cpp)
target_link_libraries(LGC    LGC_Core  LSAlgorithms ProjectFramework SpatialObjects)
install(TARGETS LGC DESTINATION "." COMPONENT application)
if(!UNIX)
	install(FILES
			"${VCREDIST_INSTALLER_PATH}"
			DESTINATION "."
			COMPONENT application
		)
endif()

#copy test files for unit tests
if(UNIX)
	execute_process(COMMAND copy_test_files_for_UnitTests.sh
					RESULT_VARIABLE rv)
else()
	execute_process(COMMAND copy_test_files_for_UnitTests.bat
					RESULT_VARIABLE rv)
endif()
