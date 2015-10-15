include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .cpp files:
set(Converters_SOURCES 
	TAConverter.cpp
	TObservationFormat.cpp
	TPointConverter.cpp
	TPointFormat.cpp
)

# Create a variable containing all .h files:
set(Converters_HEADERS 
	${SURVEYLIB_INCLUDE_PATH}/TAConverter.h
	${SURVEYLIB_INCLUDE_PATH}/TObservationFormat.h
	${SURVEYLIB_INCLUDE_PATH}/TPointConverter.h
	${SURVEYLIB_INCLUDE_PATH}/TPointFormat.h
)

# Create a static library from sources:
add_library(Converters STATIC ${Converters_SOURCES} ${Converters_HEADERS})
