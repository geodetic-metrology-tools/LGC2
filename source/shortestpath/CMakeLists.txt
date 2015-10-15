include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .cpp files:
set(ShortestPath_SOURCES 
	TGraph.cpp
	TLocalSystemOrigin.cpp
	TRefFrameInfo.cpp
	TRefFrameWrapper.cpp
	TRefSystemFactory.cpp
	TTransformWrapper.cpp
	TCoordSysInfo.cpp
	TAngularUnitsInfo.cpp
)

# Create a variable containing all .h files:
set(ShortestPath_HEADERS 
	${SURVEYLIB_INCLUDE_PATH}/DynamicPriorityQueue.h
	${SURVEYLIB_INCLUDE_PATH}/PtrGreater.h
	${SURVEYLIB_INCLUDE_PATH}/TGraph.h
	${SURVEYLIB_INCLUDE_PATH}/TLocalSystemOrigin.h
	${SURVEYLIB_INCLUDE_PATH}/TRefFrameInfo.h
	${SURVEYLIB_INCLUDE_PATH}/TRefFrameWrapper.h
	${SURVEYLIB_INCLUDE_PATH}/TRefSystemFactory.h
	${SURVEYLIB_INCLUDE_PATH}/TTransformWrapper.h
	${SURVEYLIB_INCLUDE_PATH}/TNotInGraphException.h
	${SURVEYLIB_INCLUDE_PATH}/TCoordSysInfo.h
	${SURVEYLIB_INCLUDE_PATH}/TAngularUnitsInfo.h
)


# Create a static library from sources:
add_library(ShortestPath STATIC ${ShortestPath_SOURCES} ${ShortestPath_HEADERS})
#install(FILES ${ShortestPath_HEADERS} DESTINATION ${INSTALL_PREFIX}/include)
