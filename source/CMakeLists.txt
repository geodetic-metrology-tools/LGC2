#
# CMake file for LGC2UI
#
# To create a solution with this CMake file, you need to first define the following variables in a CMake style file.
# The path to this file can be setup in the below variable `EXT_LIBS_TXT_PATH`
#
# - `BOOST_ROOT`: The path to Boost folder
# - `EIGEN_INCLUDE_PATH`: The path to Eigen folder
# - `REFRAME_LIBRARY_DIR`: The path to Reframe folder
# - `TUT_INCLUDE_PATH`: The path to Tut folder
# - `VCREDIST_INSTALLER_PATH`: The path to the folder holding the VCRedist installer

cmake_minimum_required(VERSION 3.6)

#############################################################
#
# PROJECT NAME
#

set(PROJECT_NAME LGC)

#############################################################
#
# GLOBAL CONFIGURATION
#

set(CREATE_DOC ON)
set(DOXYGEN_CONF_PATH ".")
set(ENABLE_CONSOLE ON)

include("../../SUSoftCMakeCommon/CMakeLists.txt")

# Set version number and provide a header file with corresponding defines
# /!\ IMPORTANT : Modify it int LGC_Core/LGCCore.cmake AS WELL !
set (LGC_VERSION_MAJOR 2)
set (LGC_VERSION_MINOR 01)
set (LGC_VERSION_PATCH 02)

# Versions files
configure_file (
  "${CMAKE_SOURCE_DIR}/LGCVersion.in"
  "${CMAKE_SOURCE_DIR}/Version.h"
)

# Make surveylib sublibraries available
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
	${SURVEYLIB_ROOT}/source/Tools/headers
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
	${TUT_INCLUDE_PATH}
	${EIGEN_INCLUDE_PATH}
)

INCLUDE("${SURVEYLIB_ROOT}/source/SourceSurveyLib.cmake")

# add subdirectories
ADD_SUBDIRECTORY(LGC_Core)
ADD_SUBDIRECTORY(LGC_Tests)


#include the CPack file allows to have PACKAGE in the solution and then to create the package
if(WIN32)
	include(${PROJECT_SOURCE_DIR}/CPack.cmake)
endif()

# INSTALL for creation of the LGC2 installer
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/LGC_Core/sources/LGCProject/main.cpp)
target_link_libraries(${PROJECT_NAME}
	LGC_Core
	LSAlgorithms
	ProjectFramework
	SpatialObjects
)

############################
# MSVC:
if(MSVC)
	set_vs_default_startup_project(${PROJECT_NAME})
endif()

############################
# INSTALLER PACKAGE:
if(WIN32)
	create_default_installer()
endif()
