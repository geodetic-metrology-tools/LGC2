include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .h files:
set(ContributionsGenerators_HEADERS 	
	TContributionsGenerator.h
	TLOR2LOR.h
	ContributionStructures.h
)

# Create a variable containing all .cpp files:
set(ContributionsGenerators_SOURCES
	TContributionsGenerator.cpp
	TLOR2LOR.cpp
)


# Create a static library from sources:
add_library(ContributionsGenerators STATIC ${ContributionsGenerators_HEADERS} ${ContributionsGenerators_SOURCES})
