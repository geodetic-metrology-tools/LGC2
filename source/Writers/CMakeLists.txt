include_directories(${CMAKE_SOURCE_DIR})
# Create a variable containing all .cpp files:
set(Writers_SOURCES
	TAFileWriter.cpp
	TResultsFileWriter.cpp
)

# Create a variable containing all .h files:
set(Writers_HEADERS 	
	TAFileWriter.h
	TResultsFileWriter.h
)

# Create a static library from sources:
add_library(Writers STATIC ${Writers_SOURCES} ${Writers_HEADERS})