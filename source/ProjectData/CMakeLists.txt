include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .cpp files:
set(ProjectData_SOURCES 
	TADataSet.cpp
	TDataParameters.cpp
	TFileParameters.cpp
	TPtListBroadcaster.cpp
	TWorkingPoints.cpp
)

# Create a variable containing all .h files:
set(ProjectData_HEADERS 
	${SURVEYLIB_INCLUDE_PATH}/TADataSet.h
	${SURVEYLIB_INCLUDE_PATH}/TDataParameters.h
	${SURVEYLIB_INCLUDE_PATH}/TFileParameters.h
	${SURVEYLIB_INCLUDE_PATH}/TPtListBroadcaster.h
	${SURVEYLIB_INCLUDE_PATH}/TVPtListListener.h
	${SURVEYLIB_INCLUDE_PATH}/TWorkingPoints.h
)

# Create a static library from sources:
add_library(ProjectData STATIC ${ProjectData_SOURCES} ${ProjectData_HEADERS})
#install(FILES ${ProjectData_HEADERS} DESTINATION ${INSTALL_PREFIX}/include)
#####

#target_link_libraries(ProjectData ${PROJECTFRAMEWORK_LIBRARIES} ${SPATIAL_LIBRARIES})