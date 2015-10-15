include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .cpp files:
set(StreamFormatters_SOURCES 
	TAStreamFormatter.cpp
	TFixedColFormatTStream.cpp
	TSeparatedFormatTStream.cpp
	
	# TAFixedColFormatTStream.cpp
	# TBDFixedColFormatTStream.cpp
	# TFreeFormatTStream.cpp
	# TIStreamFormatterCatalogue.cpp
	TStreamFormatterFactory.cpp
)

# Create a variable containing all .h files:
set(StreamFormatters_HEADERS 
	${SURVEYLIB_INCLUDE_PATH}/TAStreamFormatter.h
	${SURVEYLIB_INCLUDE_PATH}/TFixedColFormatTStream.h
	${SURVEYLIB_INCLUDE_PATH}/TSeparatedFormatTStream.h
	
	# ${SURVEYLIB_INCLUDE_PATH}/TAFixedColFormatTStream.h
	# ${SURVEYLIB_INCLUDE_PATH}/TBDFixedColFormatTStream.h
	# ${SURVEYLIB_INCLUDE_PATH}/TFreeFormatTStream.h
	# ${SURVEYLIB_INCLUDE_PATH}/TIStreamFormatterCatalogue.h
	${SURVEYLIB_INCLUDE_PATH}/TStreamFormatterFactory.h
)

# Create a static library from sources:
add_library(StreamFormatters STATIC ${StreamFormatters_SOURCES} ${StreamFormatters_HEADERS})

# Link static libraries to the current one
target_link_libraries(StreamFormatters FiltersAndFormats)
