include_directories(${CMAKE_SOURCE_DIR})
# Create a variable containing all .cpp files:
set(Writers_SOURCES
	TAFileWriter.cpp
	TResultsFileWriter.cpp
	TPunchFileWriter.cpp
	TFautFileWriter.cpp
	TResSimFileWriter.cpp
	TSimFileWriter.cpp
)

# Create a variable containing all .h files:
set(Writers_HEADERS 	
	TAFileWriter.h
	TResultsFileWriter.h
	TPunchFileWriter.h
	TFautFileWriter.h
	TResSimFileWriter.h
	TSimFileWriter.h
)

# Create a static library from sources:
add_library(Writers STATIC ${Writers_SOURCES} ${Writers_HEADERS})


# Link static libraries to the current one
target_link_libraries(Writers LGCObjectWriters Converters)
