include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .cpp files:
set(FiltersAndFormats_SOURCES 
	T100MicroGonsFilter.cpp
	T2DPlusHFilter.cpp
	T3DCartesianFilter.cpp
	TCCsFilter.cpp
	TDMSFilter.cpp
	TGeodeticFilter.cpp
	TGonsFilter.cpp
	TKilometresFilter.cpp
	TLengthFilter.cpp
	TMetresFilter.cpp
	TMillimetresFilter.cpp
	TRadiansFilter.cpp
	
	# TWPS2DFilter.cpp
	# TWPSTFilter.cpp
	# TWPSVFilter.cpp
	# TWTW2DFilter.cpp
	# TWTWHFilter.cpp
	# TWTWVFilter.cpp
)

# Create a variable containing all .h files:
set(FiltersAndFormats_HEADERS 
	${SURVEYLIB_INCLUDE_PATH}/setformat.h
	${SURVEYLIB_INCLUDE_PATH}/T100MicroGonsFilter.h
	${SURVEYLIB_INCLUDE_PATH}/T2DPlusHFilter.h
	${SURVEYLIB_INCLUDE_PATH}/T3DCartesianFilter.h
	${SURVEYLIB_INCLUDE_PATH}/TAngleFilter.h
	${SURVEYLIB_INCLUDE_PATH}/TCCsFilter.h
	${SURVEYLIB_INCLUDE_PATH}/TDMSFilter.h
	${SURVEYLIB_INCLUDE_PATH}/TGeodeticFilter.h
	${SURVEYLIB_INCLUDE_PATH}/TGonsFilter.h
	${SURVEYLIB_INCLUDE_PATH}/TKilometresFilter.h
	${SURVEYLIB_INCLUDE_PATH}/TLengthFilter.h
	${SURVEYLIB_INCLUDE_PATH}/TMetresFilter.h
	${SURVEYLIB_INCLUDE_PATH}/TMillimetresFilter.h
	${SURVEYLIB_INCLUDE_PATH}/TRadiansFilter.h
	${SURVEYLIB_INCLUDE_PATH}/TSpatialPositionFilter.h
	
	# ${SURVEYLIB_INCLUDE_PATH}/TWPS2DFilter.h
	# ${SURVEYLIB_INCLUDE_PATH}/TWPSTFilter.h
	# ${SURVEYLIB_INCLUDE_PATH}/TWPSVFilter.h
	# ${SURVEYLIB_INCLUDE_PATH}/TWTW2DFilter.h
	# ${SURVEYLIB_INCLUDE_PATH}/TWTWHFilter.h
	# ${SURVEYLIB_INCLUDE_PATH}/TWTWVFilter.h
)

# Create a static library from sources:
add_library(FiltersAndFormats STATIC ${FiltersAndFormats_SOURCES} ${FiltersAndFormats_HEADERS})
