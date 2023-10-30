#
# CMake file for LGC2
#
# To create a solution with this CMake file, you need to first define the following variables in a CMake style file.
#
# - `EIGEN_INCLUDE_PATH`: The path to Eigen folder
# - `REFRAME_LIBRARY_DIR`: The path to Reframe folder
# - `TUT_INCLUDE_PATH`: The path to Tut folder

cmake_minimum_required(VERSION 3.6)

#############################################################
#
# PROJECT NAME
#

project(LGC)

# Set version number and provide a header file with corresponding defines
# /!\ IMPORTANT : Modify it in LGC_Core/LGCCore.cmake AS WELL !
set (EXE_VERSION_MAJOR "2")
set (EXE_VERSION_MINOR "8")
set (EXE_VERSION_PATCH "0")

#############################################################
#
# GLOBAL CONFIGURATION
#

set(CREATE_DOC ON)
set(DOXYGEN_CONF_PATH ".")
set(ENABLE_CONSOLE ON)
set(CPACK_BUILD ON)
set(USE_SERIALIZER 1)
add_compile_definitions(USE_SERIALIZER=${USE_SERIALIZER})

set(LIBRARIES_PATH "${CMAKE_SOURCE_DIR}/../lib")
include(${LIBRARIES_PATH}/SUSoftCMakeCommon/CMakeLists.txt)

# Versions files
configure_file (
  "${CMAKE_SOURCE_DIR}/LGCVersion.in"
  "${CMAKE_SOURCE_DIR}/Version.h"
)

# Make surveylib sublibraries available
SET(SURVEYLIB_ROOT ${LIBRARIES_PATH}/SurveyLib)


IF(MSVC)
	ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ENDIF(MSVC)

# Add options to compile with GCC to get more opinions on the code
IF(MINGW)
message("    Using MinGW")
	ADD_DEFINITIONS(-D_USE_MATH_DEFINES) #defined variable added FRK 17/11/2016
ENDIF(MINGW)

include_directories (
	SYSTEM
	${CMAKE_SOURCE_DIR}/pyLib/headers
	${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCAdjustableObjects
	${CMAKE_SOURCE_PATH}/LGC_Core/headers/LGCConstraintObjects
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
	${SURVEYLIB_ROOT}/source/Tools/headers
	${SURVEYLIB_ROOT}/source/Logs
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
ADD_SUBDIRECTORY(pyLib)

# INSTALL for creation of the LGC2 installer
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/LGC_Core/sources/LGCProject/main.cpp)
target_link_libraries(${PROJECT_NAME}
	pyLib
	LGC_Core
	LSAlgorithms
	ProjectFramework
	SpatialObjects
	Logs
	Plugins
)

############################
# MSVC:
if(MSVC)
	set_vs_default_startup_project(${PROJECT_NAME})
endif()

############################
# INSTALLER PACKAGE:
if(WIN32)
	include(${CMAKE_CURRENT_LIST_DIR}/CPack.cmake)
	include(CPack)
	create_default_installer()
endif()
