include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .cpp files:
set(SpatialObj_SOURCES 
	TASpatialPtListener.cpp
	TSpatialLine.cpp
	TSpatialLineName.cpp
	TSpatialPlane.cpp
	TSpatialPlaneName.cpp
	TSpatialPoint.cpp
	TSpatialPointName.cpp
	TVSpatialPtListener.cpp
)

# Create a variable containing all .h files:
set(SpatialObj_HEADERS 
	${SURVEYLIB_INCLUDE_PATH}/TPtrWrapper.h
	${SURVEYLIB_INCLUDE_PATH}/TASpatialPtListener.h
	${SURVEYLIB_INCLUDE_PATH}/TSpatialLine.h
	${SURVEYLIB_INCLUDE_PATH}/TSpatialLineName.h
	${SURVEYLIB_INCLUDE_PATH}/TSpatialPlane.h
	${SURVEYLIB_INCLUDE_PATH}/TSpatialPlaneName.h
	${SURVEYLIB_INCLUDE_PATH}/TSpatialPoint.h
	${SURVEYLIB_INCLUDE_PATH}/TSpatialPointName.h
	${SURVEYLIB_INCLUDE_PATH}/TVSpatialPtListener.h
)

# Create a static library from sources:
add_library(SpatialObj STATIC ${SpatialObj_SOURCES} ${SpatialObj_HEADERS})
#install(FILES ${SpatialObj_HEADERS} DESTINATION ${INSTALL_PREFIX}/include)
