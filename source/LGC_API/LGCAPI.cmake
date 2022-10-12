# Configure version when called from UI
# /!\ IMPORTANT : Modify it int ../CMakeLists.txt AS WELL !
set (LGC_VERSION_MAJOR "2")
set (LGC_VERSION_MINOR "4")
set (LGC_VERSION_PATCH "0")

# Versions files
configure_file (
    ${CMAKE_SOURCE_DIR}/LGCVersion.in
    ${CMAKE_SOURCE_DIR}/Version.h
)

if(MSVC)
	add_definitions(-D_SCL_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)

# Add options to compile with GCC to get more opinions on the code
if(MINGW)
    message("    Using MinGW")
endif(MINGW)

# Define a variable that contains the path to LGC modules
set(LGCAPI_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
set(LGCAPI_INCLUDE_PATH ${LGCAPI_MODULE_PATH}/headers)
set(SURVEYLIB_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../SurveyLib)

include_directories (
	SYSTEM
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
    ${LGCAPI_INCLUDE_PATH}
)

INCLUDE(${SURVEYLIB_ROOT}/source/SourceSurveyLib.cmake)

set(API_INCLUDE_PATH 	${LGCAPI_INCLUDE_PATH})
set(API_SOURCES_PATH 	${LGCAPI_MODULE_PATH}/sources)
					
					
# Create a variable containing all .h files:
set(API_HEADERS 	
	${API_INCLUDE_PATH}/TDummy.h
)

# Create a variable containing all .cpp files:
set(API_SOURCES
	${API_SOURCES_PATH}/TDummy.cpp
)

# Define VS folders:
file(GLOB API_H ${API_INCLUDE_PATH}/*.*)

file(GLOB API_CPP ${API_SOURCES_PATH}/*.*)

source_group("\\Headers" FILES ${API_H})

source_group("\\Sources" FILES ${API_CPP})


# Create a static library from sources:
add_library(LGCAPI STATIC 
    ${API_HEADERS}
    ${API_SOURCES}
)

target_link_libraries(LGCCore 
    LSAlgorithms
    ProjectFramework
    SpatialObjects
    Tools
)

