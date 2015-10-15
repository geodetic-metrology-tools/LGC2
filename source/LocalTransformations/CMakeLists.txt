include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/LGCProject)

# Create a variable containing all .h files:
set(LocalTransformations_HEADERS 
	TTransformation.h
	TTransformParameters.h
	TDirectTransformation.h
	TInverseTransformation.h
	TDerivativeTransformation.h
	TLOR2LOR.h
	TCCS2CGRFTransformation.h
	TCGRF2LGTransformation.h
	TILG2ILATransformation.h
)

# Create a variable containing all .cpp files:
set(LocalTransformations_SOURCES
	TTransformation.cpp
	TDirectTransformation.cpp
	TInverseTransformation.cpp
	TDerivativeTransformation.cpp
	TLOR2LOR.cpp
	TCCS2CGRFTransformation.cpp
	TCGRF2LGTransformation.cpp
	TILG2ILATransformation.cpp
)

# Create a static library from sources:
add_library(LocalTransformations STATIC ${LocalTransformations_HEADERS} ${LocalTransformations_SOURCES})
