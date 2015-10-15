# Create a variable containing all .cpp files:
set(ContributionsGenerators_SOURCES
	TLGCObsLSContributionGenerator.cpp
	TLSFreeCnstrCG.cpp
)

# Create a variable containing all .h files:
set(ContributionsGenerators_HEADERS 	
	${LGC_INCLUDE_PATH}/TLGCObsLSContributionGenerator.h
	${LGC_INCLUDE_PATH}/TLSFreeCnstrCG.h
	
)

# Create a static library from sources:
add_library(ContributionsGenerators STATIC ${ContributionsGenerators_SOURCES} ${ContributionsGenerators_HEADERS})