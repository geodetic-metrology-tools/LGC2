include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .h files:
set(LocalTransformations_HEADERS 	
	${SURVEYLIB_INCLUDE_PATH}/TTransformParameters.h
	${SURVEYLIB_INCLUDE_PATH}/TTransformation.h
	${SURVEYLIB_INCLUDE_PATH}/TDirectTransformation.h
	${SURVEYLIB_INCLUDE_PATH}/TInverseTransformation.h
	${SURVEYLIB_INCLUDE_PATH}/TDerivativeTransformation.h
	${SURVEYLIB_INCLUDE_PATH}/TLOR2LOR.h
)

# Create a variable containing all .cpp files:
set(LocalTransformations_SOURCES
	TTransformation.cpp
	TDirectTransformation.cpp
	TInverseTransformation.cpp
	TDerivativeTransformation.cpp
	TLOR2LOR.cpp
)


# Create a static library from sources:
add_library(LocalTransformations STATIC ${LocalTransformations_HEADERS} ${LocalTransformations_SOURCES})
