include_directories(${LGC_MODULE_PATH}
					${LGC_MODULE_PATH}/AdjustableObjects
					${LGC_MODULE_PATH}/ContributionsGenerators
					${LGC_MODULE_PATH}/LGCProject
					${LGC_MODULE_PATH}/LGCProjectData
					${LGC_MODULE_PATH}/Measurements
					${LGC_MODULE_PATH}/LGCObjectWriters
					${LGC_MODULE_PATH}/Readers
					${LGC_MODULE_PATH}/Writers)

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
	TLA2MLATransformation.h
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
	TLA2MLATransformation.cpp
)

# Create a static library from sources:
add_library(LocalTransformations STATIC ${LocalTransformations_HEADERS} ${LocalTransformations_SOURCES})
# Link static libraries to the current one
# target_link_libraries(LocalTransformations LocalTransformations)
