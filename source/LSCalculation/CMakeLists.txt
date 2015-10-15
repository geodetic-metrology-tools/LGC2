include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .h files:
set(LSCalculation_HEADERS 	
	TLSInputMatricesFiller.h
	TLSResultsMatricesExtractor.h
	#QuantileFunctions.h
	TLGCCalcParams.h
	
)

# Create a variable containing all .cpp files:
set(LSCalculation_SOURCES
	TLSInputMatricesFiller.cpp
	TLSResultsMatricesExtractor.cpp
	#QuantileFunctions.cpp
	TLGCCalcParams.cpp
)


# Create a static library from sources:
add_library(LSCalculation STATIC ${LSCalculation_HEADERS} ${LSCalculation_SOURCES})
