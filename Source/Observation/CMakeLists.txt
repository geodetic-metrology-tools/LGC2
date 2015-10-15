# Create a variable containing all .cpp files:
set(Observation_SOURCES
	TGyroOrientationObservation.cpp
	THorAngleObservation.cpp
	THorizontalDistObservation.cpp
	TOffsetToSpaLineObservation.cpp
	TOffsetToTheoPlaneObservation.cpp
	TOffsetToVerLineObservation.cpp
	TOffsetToVerPlaneObservation.cpp
	TOrientationCnstrObservation.cpp
	TRadialOffsetCnstrObservation.cpp
	TSpatialDistObservation.cpp
	TVertDistObservation.cpp
	TZenithDistObservation.cpp
)

# Create a variable containing all .h files:
set(Observation_HEADERS 	
	${LGC_INCLUDE_PATH}/TGyroOrientationObservation.h
	${LGC_INCLUDE_PATH}/THorAngleObservation.h
	${LGC_INCLUDE_PATH}/THorizontalDistObservation.h
	${LGC_INCLUDE_PATH}/TOffsetToSpaLineObservation.h
	${LGC_INCLUDE_PATH}/TOffsetToTheoPlaneObservation.h
	${LGC_INCLUDE_PATH}/TOffsetToVerLineObservation.h
	${LGC_INCLUDE_PATH}/TOffsetToVerPlaneObservation.h
	${LGC_INCLUDE_PATH}/TOrientationCnstrObservation.h
	${LGC_INCLUDE_PATH}/TRadialOffsetCnstrObservation.h
	${LGC_INCLUDE_PATH}/TSpatialDistObservation.h
	${LGC_INCLUDE_PATH}/TVertDistObservation.h
	${LGC_INCLUDE_PATH}/TZenithDistObservation.h	
)

# Create a static library from sources:
add_library(Observation STATIC ${Observation_SOURCES} ${Observation_HEADERS})
