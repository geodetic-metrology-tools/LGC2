# Create a variable containing all .cpp files:
set(LGCConverters_SOURCES
	TALGCConverter.cpp
	TAngleObsConverter.cpp
	TGyroOrientationConverter.cpp
	THorAngleConverter.cpp
	THorDistConverter.cpp
	TLengthObsConverter.cpp
	TObservationConverter.cpp
	TOffsetToSpaLineConverter.cpp
	TOffsetToTheoPlaneConverter.cpp
	TOffsetToVerLineConverter.cpp
	TOffsetToVerPlaneConverter.cpp
	TRadialOffsetCnstrConverter.cpp
	TRelativeErrorConverter.cpp
	TSpatialDistConverter.cpp
	TVertDistConverter.cpp
	TZenithDistConverter.cpp
)


# Create a variable containing all .h files:
set(LGCConverters_HEADERS 	
	${LGC_INCLUDE_PATH}/TALGCConverter.h
	${LGC_INCLUDE_PATH}/TAngleObsConverter.h
	${LGC_INCLUDE_PATH}/TGyroOrientationConverter.h
	${LGC_INCLUDE_PATH}/THorAngleConverter.h
	${LGC_INCLUDE_PATH}/THorDistConverter.h
	${LGC_INCLUDE_PATH}/TLengthObsConverter.h
	${LGC_INCLUDE_PATH}/TObservationConverter.h
	${LGC_INCLUDE_PATH}/TOffsetToSpaLineConverter.h
	${LGC_INCLUDE_PATH}/TOffsetToTheoPlaneConverter.h
	${LGC_INCLUDE_PATH}/TOffsetToVerLineConverter.h
	${LGC_INCLUDE_PATH}/TOffsetToVerPlaneConverter.h
	${LGC_INCLUDE_PATH}/TRadialOffsetCnstrConverter.h
	${LGC_INCLUDE_PATH}/TRelativeErrorConverter.h
	${LGC_INCLUDE_PATH}/TSpatialDistConverter.h
	${LGC_INCLUDE_PATH}/TVertDistConverter.h
	${LGC_INCLUDE_PATH}/TZenithDistConverter.h
)

# Create a static library from sources:
add_library(LGCConverters STATIC ${LGCConverters_SOURCES} ${LGCConverters_HEADERS})