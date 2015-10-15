# Declare a project
PROJECT(LGC2)

# Set version number and provide a header file with corresponding defines
set (LGC_VERSION_MAJOR 2)
set (LGC_VERSION_MINOR 0)
set (LGC_VERSION_PATCH 0)
configure_file (
  "${CMAKE_SOURCE_DIR}/LGCVersion.in"
  "${CMAKE_SOURCE_DIR}/Version.h"
)

# Minimum version of CMake
cmake_minimum_required(VERSION 2.8)

INCLUDE(CheckIncludeFileCXX)

# Enable languages
enable_language("CXX")


find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)


# default build type
SET(CMAKE_BUILD_TYPE DEBUG)

# REQ: BOOST_ROOT VCREDIST_INSTALLER_PATH SURVEYLIB_INCLUDE_PATH SURVEYLIB_LIBRARY_DIR EIGEN_INCLUDE_PATH
INCLUDE(../../ext_libs.txt)

# Make surveylib sublibraries avaliable
#INCLUDE(${SURVEYLIB_ROOT}/source/SubLibraries.cmake)
SET(SURVEYLIB_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/../include")

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
	${Boost_INCLUDE_DIRS}
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
IF(NOT HAVE_TUT_H)
    MESSAGE(FATAL_ERROR "[ERROR] Cannot find tut.h. Set TUT_INCLUDE_PATH")
ELSE()
    MESSAGE(STATUS "[STATUS] Found tut.h")
ENDIF()
#LINK SurveyLib projects
#INCLUDE("${SURVEYLIB_ROOT}/source/SourceSurveyLib.cmake")

include_directories(SYSTEM  ${TUT_INCLUDE_PATH} ${EIGEN_INCLUDE_PATH})
#add surveylib_include_path into directory (local copy of header files)
INCLUDE_DIRECTORIES(${SURVEYLIB_INCLUDE_PATH})

add_subdirectory (AdjustableObjects)
add_subdirectory(CalcElements)
add_subdirectory(LSCalculation)
add_subdirectory(readers)
add_subdirectory(tests)
add_subdirectory(ContributionsGenerators)
add_subdirectory(affinetransformations)
add_subdirectory(mathematicalconcepts)
add_subdirectory(Measurements)
add_subdirectory(referencesurfaces)
add_subdirectory(refframetransformations)
add_subdirectory(spatialobj)
add_subdirectory(spatialsystems)
add_subdirectory(shortestpath)
add_subdirectory(Writers)

#Writers related projects
add_subdirectory(LGCConverters)
add_subdirectory(Converters)
add_subdirectory(FiltersAndFormats)
add_subdirectory(ProjectData)
add_subdirectory(StreamFormatters)

include_directories(${CMAKE_SOURCE_DIR})

set(LGC2ROOT_HEADERS
	TInstrumentData.h
	Legacy.h
	TLGCAngle.h
	TLGCApp.h
	TLGCData.h
	Defaults.h
	Global.h
	TLGCConfig.h
	Version.h
	tree.h
	TDataAnalyzer.h
	TFileLogger.h
	TLGCCalculation.h
	TLSConstraintIdentifier.h
)

set(LGC2ROOT_SOURCES
	TDataAnalyzer.cpp
	TLGCCalculation.cpp
	TLGCAngle.cpp
	TLGCApp.cpp
	TLGCData.cpp
	TFileLogger.cpp
)

add_library(LGCRoot STATIC ${LGC2ROOT_HEADERS} ${LGC2ROOT_SOURCES})

add_executable(LGC2 main.cpp)

target_link_libraries(LGC2 Readers AdjustableObjects CalcElements ProjectData LGCConverters StreamFormatters FiltersAndFormats Converters ContributionsGenerators AffineTransformations LSCalculation MathematicalConcepts LGCRoot Measurements Writers ShortestPath ReferenceSurfaces RefFrameTransformations SpatialObj SpatialSystems)
