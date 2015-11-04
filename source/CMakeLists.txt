# Mandatory in order to set the devtoolset2 (redhat specific) where compilers gcc & g++ are located
if(UNIX)
	SET(CMAKE_C_COMPILER   "/opt/rh/devtoolset-2/root/usr/bin/gcc")
	SET(CMAKE_CXX_COMPILER "/opt/rh/devtoolset-2/root/usr/bin/g++")
endif()

# Declare a project
PROJECT(LGC2)

# Set version number and provide a header file with corresponding defines
set (LGC_VERSION_MAJOR 2)
set (LGC_VERSION_MINOR 0)
set (LGC_VERSION_PATCH 0)

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
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -Wno-reorder -Wfatal-errors -Wno-comments")
ENDIF(MINGW)

INCLUDE("${SURVEYLIB_ROOT}/source/SourceSurveyLib.cmake")

include_directories (
	SYSTEM
	${CMAKE_SOURCE_DIR}/LGCProject
	${Boost_INCLUDE_DIRS}
	${SURVEYLIB_ROOT}/source/LSAlgorithms/CalcElements
	${SURVEYLIB_ROOT}/source/LSAlgorithms/CalcParams
	${SURVEYLIB_ROOT}/source/ProjectFramework/Converters
	${SURVEYLIB_ROOT}/source/ProjectFramework/FiltersAndFormats
	${SURVEYLIB_ROOT}/source/ProjectFramework/ProjectData
	${SURVEYLIB_ROOT}/source/ProjectFramework/StreamFormatters
	${SURVEYLIB_ROOT}/source/SpatialObjects/AffineTransformations
	${SURVEYLIB_ROOT}/source/SpatialObjects/MathematicalConcepts
	${SURVEYLIB_ROOT}/source/SpatialObjects/ReferenceSurfaces
	${SURVEYLIB_ROOT}/source/SpatialObjects/RefFrameTransformations
	${SURVEYLIB_ROOT}/source/SpatialObjects/ShortestPath
	${SURVEYLIB_ROOT}/source/SpatialObjects/SpatialMeasurments
	${SURVEYLIB_ROOT}/source/SpatialObjects/SpatialObj
	${SURVEYLIB_ROOT}/source/SpatialObjects/SpatialSystems
	${TUT_INCLUDE_PATH}
	${EIGEN_INCLUDE_PATH}
)


add_subdirectory(AdjustableObjects)
add_subdirectory(LSCalculation)
add_subdirectory(Readers)
add_subdirectory(tests)
add_subdirectory(ContributionsGenerators)
add_subdirectory(Measurements)
add_subdirectory(LocalTransformations)
add_subdirectory(LGCProject)
add_subdirectory(LGCProjectData)
add_subdirectory(LGCCalculation)
add_subdirectory(Writers)
add_subdirectory(LGCObjectWriters)


#copy test files for unit tests
if(UNIX)
	execute_process(COMMAND copy_test_files_for_UnitTests.sh
					RESULT_VARIABLE rv)
else()
	execute_process(COMMAND copy_test_files_for_UnitTests.bat
					RESULT_VARIABLE rv)
endif()