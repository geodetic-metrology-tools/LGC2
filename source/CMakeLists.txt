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

# REQ: BOOST_ROOT VCREDIST_INSTALLER_PATH SURVEYLIB_INCLUDE_PATH SURVEYLIB_LIBRARY_DIR EIGEN_INCLUDE_PATH
INCLUDE(../../ext_libs.txt)

# Make surveylib sublibraries avaliable
#INCLUDE(${SURVEYLIB_ROOT}/source/SubLibraries.cmake)
SET(SURVEYLIB_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/../includeSurveyLib")
SET(LGC2_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/../includeLGC2")

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

include_directories (
	${LGC2_INCLUDE_PATH}
	${Boost_INCLUDE_DIRS}
	${SURVEYLIB_INCLUDE_PATH}
	${SURVEYLIB_INCLUDE_PATH}/lsalgo
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/AdjustableObjects
	${CMAKE_SOURCE_DIR}/refframetransformations
	${CMAKE_SOURCE_DIR}/ContributionsGenerators
	${CMAKE_SOURCE_DIR}/LSCalculation
	${CMAKE_SOURCE_DIR}/Measurements
	${CMAKE_SOURCE_DIR}/readers
	${CMAKE_SOURCE_DIR}/Writers
)

# Check for external libraries
SET(CMAKE_REQUIRED_INCLUDES ${TUT_INCLUDE_PATH})
CHECK_INCLUDE_FILE_CXX("tut.h" HAVE_TUT_H)
SET(CMAKE_REQUIRED_INCLUDES "")
# SET(CMAKE_REQUIRED_INCLUDES ${EIGEN_INCLUDE_PATH})
# CHECK_INCLUDE_FILE_CXX("eigen/Core" HAVE_EIGEN_H)
# SET(CMAKE_REQUIRED_INCLUDES "")

# IF(NOT HAVE_EIGEN_H)
    # MESSAGE(FATAL_ERROR "[ERROR] Cannot find eigen/Core. Set EIGEN_INCLUDE_PATH")
# ELSE()
    # MESSAGE(STATUS "[STATUS] Found eigen/Core")
# ENDIF()
#IF(NOT HAVE_TUT_H)
#    MESSAGE(FATAL_ERROR "[ERROR] Cannot find tut.h. Set TUT_INCLUDE_PATH")
#ELSE()
#    MESSAGE(STATUS "[STATUS] Found tut.h")
#ENDIF()
#LINK SurveyLib projects
#INCLUDE("${SURVEYLIB_ROOT}/source/SourceSurveyLib.cmake")
#INCLUDE("${CMAKE_SOURCE_DIR}/CPack.cmake") #LGC INSTALLER: Description of properties and options


include_directories(SYSTEM  ${TUT_INCLUDE_PATH} ${EIGEN_INCLUDE_PATH})
#add surveylib_include_path into directory (local copy of header files)
INCLUDE_DIRECTORIES(${SURVEYLIB_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${LGC2_INCLUDE_PATH})

add_subdirectory (AdjustableObjects)
add_subdirectory(CalcElements)
add_subdirectory(LSCalculation)
add_subdirectory(readers)
add_subdirectory(tests)
add_subdirectory(ContributionsGenerators)
add_subdirectory(affinetransformations)
add_subdirectory(mathematicalconcepts)
add_subdirectory(Measurements)
add_subdirectory(LocalTransformations)
add_subdirectory(referencesurfaces)
add_subdirectory(refframetransformations)
add_subdirectory(spatialobj)
add_subdirectory(spatialsystems)
add_subdirectory(shortestpath)
add_subdirectory(LGCProject)
add_subdirectory(LGCProjectData)
add_subdirectory(LGCCalculation)

#Writers related projects
add_subdirectory(Writers)
add_subdirectory(LGCObjectWriters)
add_subdirectory(Converters)
add_subdirectory(FiltersAndFormats)
add_subdirectory(ProjectData)
add_subdirectory(StreamFormatters)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(LGC main.cpp)

target_link_libraries(LGC LGCProject LGCProjectData LGCCalculation Readers AdjustableObjects LocalTransformations CalcElements ProjectData LGCObjectWriters StreamFormatters FiltersAndFormats Converters ContributionsGenerators AffineTransformations LSCalculation MathematicalConcepts Measurements Writers ShortestPath ReferenceSurfaces RefFrameTransformations SpatialObj SpatialSystems)

# INSTALL for creation of the LGC2 installer
install(TARGETS LGC DESTINATION "." COMPONENT application)
install(FILES
		"${VCREDIST_INSTALLER_PATH}"
		DESTINATION "."
		COMPONENT application
	)
	
#copy test files for unit tests
if(UNIX)
	execute_process(COMMAND copy_test_files_for_UnitTests.sh
					RESULT_VARIABLE rv)
else()
	execute_process(COMMAND copy_test_files_for_UnitTests.bat
					RESULT_VARIABLE rv)
endif()